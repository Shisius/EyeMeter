import numpy as np

# -------------------------------------------------------------------------------
# FIT CIRCLE 2D
# - Find center [xc, yc] and radius r of circle fitting to set of 2D points
# - Optionally specify weights for points
#
# - Implicit circle function:
#   (x-xc)^2 + (y-yc)^2 = r^2
#   (2*xc)*x + (2*yc)*y + (r^2-xc^2-yc^2) = x^2+y^2
#   c[0]*x + c[1]*y + c[2] = x^2+y^2
#
# - Solution by method of least squares:
#   A*c = b, c' = argmin(||A*c - b||^2)
#   A = [x y 1], b = [x^2+y^2]
# -------------------------------------------------------------------------------
def fit_circle_2d(x, y, w=[]):

    A = np.array([x, y, np.ones(len(x))]).T
    b = x** 2 + y ** 2

    # Modify A,b for weighted least squares
    if len(w) == len(x):
        W = np.diag(w)
        A = np.dot(W, A)
        b = np.dot(W, b)

    # Solve by method of least squares
    c = np.linalg.lstsq(A, b, rcond=None)[0]

    # Get circle parameters from solution c
    xc = c[0] / 2
    yc = c[1] / 2
    r = np.sqrt(c[2] + xc ** 2 + yc ** 2)
    return xc, yc, r


def fit_ellipse_2d(P):
    P_mean = P.mean(axis=0)
    P_centered = P - P_mean
    X, Y = P_centered[:, 0][:, None], P_centered[:, 1][:, None]
    # Formulate and solve the least squares problem ||Ax - b ||^2
    A = np.concatenate([X ** 2, X * Y, Y ** 2], axis=1)  # np.concatenate([X ** 2, X * Y, Y ** 2, X, Y], axis=1)
    b = np.ones_like(X)
    x = np.linalg.lstsq(A, b)[0].squeeze()
    a = x[0]
    b = x[1]
    c = x[2]
    a_e = -np.sqrt(abs(2 * (b ** 2 - 4 * a * c) * ((a + c) + np.sqrt((a - c) ** 2 + b ** 2)))) / (b ** 2 - 4 * a * c)
    b_e = -np.sqrt(abs(2 * (b ** 2 - 4 * a * c) * ((a + c) - np.sqrt((a - c) ** 2 + b ** 2)))) / (b ** 2 - 4 * a * c)

    if b != 0:
        arg = (c - a - np.sqrt((a - c) ** 2 + b ** 2)) / b
        angle = 90 - (np.pi / 2 - np.arctan(arg)) / np.pi * 180
    else:
        if a <= c:
            angle = 0.0
        else:
            angle = 90.0
    return P_mean[0], P_mean[1], a_e, b_e, angle

# -------------------------------------------------------------------------------
# RODRIGUES ROTATION
# - Rotate given points based on a starting and ending vector
# - Axis k and angle of rotation theta given by vectors n0,n1
#   P_rot = P*cos(theta) + (k x P)*sin(theta) + k*<k,P>*(1-cos(theta))
# -------------------------------------------------------------------------------
def rodrigues_rot(P, n0, n1):
    # If P is only 1d array (coords of single point), fix it to be matrix
    if P.ndim == 1:
        P = P[np.newaxis, :]

    # Get vector of rotation k and angle theta
    n0 = n0 / np.linalg.norm(n0)
    n1 = n1 / np.linalg.norm(n1)
    k = np.cross(n0, n1)
    k = k / np.linalg.norm(k)
    theta = np.arccos(np.dot(n0, n1))

    # Compute rotated points
    P_rot = np.zeros((len(P), 3))
    for i in range(len(P)):
        P_rot[i] = P[i] * np.cos(theta) + np.cross(k, P[i]) * np.sin(theta) + k * np.dot(k, P[i]) * (1 - np.cos(theta))

    return P_rot


# -------------------------------------------------------------------------------
# ANGLE BETWEEN
# - Get angle between vectors u,v with sign based on plane with unit normal n
# -------------------------------------------------------------------------------
def angle_between(u, v, n=None):
    if n is None:
        return np.arctan2(np.linalg.norm(np.cross(u, v)), np.dot(u, v))
    else:
        return np.arctan2(np.dot(n, np.cross(u, v)), np.dot(u, v))


# -------------------------------------------------------------------------------
# - Make axes of 3D plot to have equal scales
# - This is a workaround to Matplotlib's set_aspect('equal') and axis('equal')
#   which were not working for 3D
# -------------------------------------------------------------------------------
def set_axes_equal_3d(ax):
    limits = np.array([ax.get_xlim3d(), ax.get_ylim3d(), ax.get_zlim3d()])
    spans = abs(limits[:, 0] - limits[:, 1])
    centers = np.mean(limits, axis=1)
    radius = 0.5 * max(spans)
    ax.set_xlim3d([centers[0] - radius, centers[0] + radius])
    ax.set_ylim3d([centers[1] - radius, centers[1] + radius])
    ax.set_zlim3d([centers[2] - radius, centers[2] + radius])


def fit_circle(P, fast=False):
    # -------------------------------------------------------------------------------
    # (1) Fitting plane by SVD for the mean-centered data
    # Eq. of plane is <p,n> + d = 0, where p is a point on plane and n is normal vector
    # -------------------------------------------------------------------------------
    P_mean = P.mean(axis=0)
    P_centered = P - P_mean
    # U, s, V = np.linalg.svd(P_centered)

    # Normal vector of fitting plane is given by 3rd column in V
    # Note linalg.svd returns V^T, so we need to select 3rd row from V^T
    # normal = V[2, :]
    # d = -np.dot(P_mean, normal)  # d = -<p,n>

    # -------------------------------------------------------------------------------
    # (2) Project points to coords X-Y in 2D plane
    # -------------------------------------------------------------------------------
    P_xy = P_centered  # rodrigues_rot(P_centered, normal, [0, 0, 1])

    # -------------------------------------------------------------------------------
    # (3) Fit circle in new 2D coords
    # -------------------------------------------------------------------------------
    if fast:
        xc, yc, r = 0.0, 0.0, (P_xy[:, 0] ** 2 + P_xy[:, 1] ** 2).mean() ** 0.5
    else:
        xc, yc, r = fit_circle_2d(P_xy[:, 0], P_xy[:, 1])

    return xc + P_mean[0], yc + P_mean[1], r