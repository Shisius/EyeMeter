
#include "serial_comm.h"

SerialComm::SerialComm(std::string name, speed_t baudrate, int timeout_ms) : d_name(name), d_baudrate(baudrate), d_timeout_ms(timeout_ms)
{
	;
}

SerialComm::~SerialComm()
{
	destroy();
}

void SerialComm::destroy()
{
    close(d_dev_fd);
    flock(d_dev_fd, LOCK_UN);  /* free the port so that others can use it. */
}

void SerialComm::clear()
{
    tcflush(d_dev_fd, TCIFLUSH);
}

int SerialComm::setup(bool search_device)
{
	// Open
	d_dev_fd = open(d_name.c_str(), O_RDWR);
	if (d_dev_fd < 0) {
        if (search_device) {
            int i = 0;
            while (i < 10) {
                d_name.pop_back();
                d_name.push_back((std::to_string(i)).front());
                d_dev_fd = open(d_name.c_str(), O_RDWR);
                if (d_dev_fd < 0) {
                    printf("SerialComm: device %s open failed\n", d_name.c_str());
                }
                else {
                    printf("SerialComm: device %s opened\n", d_name.c_str());
                    break;
                }
                i++;
            }
            if (i == 10) {
                return SERIAL_COMM_OPEN_FAILED;
            }
        } else {
            printf("SerialComm: device %s open failed\n", d_name.c_str());
            return SERIAL_COMM_OPEN_FAILED;
        }
	}
	// termios

	// Read in existing settings, and handle any error
    // NOTE: This is important! POSIX states that the struct passed to tcsetattr()
    // must have been initialized with a call to tcgetattr() overwise behaviour
    // is undefined
	if(tcgetattr(d_dev_fd, &d_tty) != 0) {
        printf("SerialComm: Error from tcgetattr\n");
    }
    d_tty.c_cflag &= ~PARENB; // Clear parity bit, disabling parity (most common)
    d_tty.c_cflag &= ~CSTOPB; // Clear stop field, only one stop bit used in communication (most common)
    d_tty.c_cflag &= ~CSIZE; // Clear all bits that set the data size
    d_tty.c_cflag |= CS8; // 8 bits per byte (most common)
    // if (d_settings.rts_mode == SERIAL_COMM_RTS_TERMIOS)
    //     d_tty.c_cflag |= CRTSCTS; // Enable RTS/CTS hardware flow control (most common)
    // else
        d_tty.c_cflag &= ~CRTSCTS; // Disable RTS/CTS hardware flow control (most common)
    d_tty.c_cflag |= CREAD | CLOCAL; // Turn on READ & ignore ctrl lines (CLOCAL = 1)

    d_tty.c_lflag &= ~ICANON;
    d_tty.c_lflag &= ~ECHO; // Disable echo
    d_tty.c_lflag &= ~ECHOE; // Disable erasure
    d_tty.c_lflag &= ~ECHONL; // Disable new-line echo
    d_tty.c_lflag &= ~ISIG; // Disable interpretation of INTR, QUIT and SUSP
    d_tty.c_iflag &= ~(IXON | IXOFF | IXANY); // Turn off s/w flow ctrl
    d_tty.c_iflag &= ~(IGNBRK|BRKINT|PARMRK|ISTRIP|INLCR|IGNCR|ICRNL); // Disable any special handling of received bytes

    d_tty.c_oflag &= ~OPOST; // Prevent special interpretation of output bytes (e.g. newline chars)
    d_tty.c_oflag &= ~ONLCR; // Prevent conversion of newline to carriage return/line feed

    d_tty.c_cc[VTIME] = 1;    /* block untill a timer expires (n * 100 mSec.) */
    d_tty.c_cc[VMIN] = 1;     /* block untill n bytes are received */

    if (cfsetispeed(&d_tty, d_baudrate) != 0) { // Set baudrate here
    	printf("SerialComm: wrong baudrate %d\n", d_baudrate);
    	return SERIAL_COMM_WRONG_BAUDRATE;
    }

    // Save tty settings, also checking for error
    if (tcsetattr(d_dev_fd, TCSANOW, &d_tty) != 0) {
        printf("SerialComm: Error from tcsetattr\n");
    }

    // set poll
    d_pollfd.fd = d_dev_fd;
    d_pollfd.events = POLLIN;

    d_pollrc = poll(&d_pollfd, 1, d_timeout_ms); //  1000 milliseconds
    if (d_pollrc < 0) { // connection failed
        printf("SerialComm: Poll has error ...\n");
        close(d_dev_fd);
        return SERIAL_COMM_POLL_ERROR; 
    }

    return 0;
}

// Read bytes into buf
int SerialComm::read_to(char * buf, unsigned int size)
{
	d_pollrc = poll(&d_pollfd, 1, d_timeout_ms);
	if (d_pollrc < 0)
		return SERIAL_COMM_POLL_ERROR;
	if (d_pollrc == 0)
		return 0;
	if (d_pollfd.revents & POLLIN) {
		return read(d_dev_fd, buf, size);
	}
	return 0;
}

// Write bytes from buf
int SerialComm::write_from(char * buf, unsigned int size)
{
    ssize_t result = write(d_dev_fd, buf, size);
    if (result < 0) {
        if(errno == EAGAIN)
        {
            return SERIAL_COMM_EAGAIN_ERROR;
        }
        else
        {
            return SERIAL_COMM_WRITE_ERROR;
        }
    }
	return result;
}

