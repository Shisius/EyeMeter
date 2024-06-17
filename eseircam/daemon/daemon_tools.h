#ifndef _DAEMON_TOOLS_H_
#define _DAEMON_TOOLS_H_

#include <unistd.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/fcntl.h>
#include <signal.h>

typedef struct std_in_out_err_fds
{
	int in;
	int out;
	int err;
} StdInOutErrFds;

__attribute__((unused)) static inline void get_process_name_by_pid(char* name, const int pid)
{
    if(name){
        sprintf(name, "/proc/%d/cmdline", pid);
        FILE* f = fopen(name, "r");
        if(f) {
            size_t size;
            size = fread(name, sizeof(char), 1024, f);
            if(size > 0){
                if( name[size - 1] == '\n')
                    name[size - 1] = '\0';
            }
            fclose(f);
        }
    }
}

__attribute__((unused)) static inline char * get_application_name(void)
{
	const int MAX_APPLICATION_NAME_LENGTH = 1024;
	// Create name
	char * name = (char*)malloc(MAX_APPLICATION_NAME_LENGTH);
	memset(name, 0, MAX_APPLICATION_NAME_LENGTH);
	// Open proc self file
	FILE* f = fopen("/proc/self/comm", "r");
	if (f == NULL) return NULL;
	// Read name
	size_t name_l = fread(name, sizeof(char), MAX_APPLICATION_NAME_LENGTH, f);
	// Delete \n
	if (name_l > 0) {
		if (name[name_l-1] == '\n') name[name_l-1] = '\0';
	}
	fclose(f);
	return name;
}

__attribute__((unused)) static inline bool write_pid_file(const char * name)
{
	// Get pid and convert to string
	pid_t pid = getpid();
	char pid_s[8];
	sprintf(pid_s, "%d\n", pid);
	// Open pid file
	if (name == NULL) return false;
	char fullname[strlen(name) + 5];
	sprintf(fullname, "/tmp/%s.pid", name);
	FILE* f = fopen(fullname, "w");
	if (f == NULL) return false;
	// Write PID and \n
	size_t n_bytes = fwrite(pid_s, sizeof(char), strlen(pid_s), f);
	// Close file and check result
	fclose(f);
	if (n_bytes == strlen(pid_s)) return true;
	return false;
}

__attribute__((unused)) static inline pid_t get_pid_from_file(const char * name)
{
	// Open file
	char fullname[strlen(name) + 5];
	sprintf(fullname, "/tmp/%s.pid", name);
	FILE* f = fopen(fullname, "r");
	if (f == NULL) return -1;
	// Get content
	char pid_s[8];
	size_t n_bytes = fread(pid_s, sizeof(char), 8, f);
	fclose(f);
	if (n_bytes <= 0) return -1;
	// Read PID
	pid_t pid = atoi(pid_s);
	return pid;
}

__attribute__((unused)) static inline bool doublefork(void)
{
	// First fork
	pid_t pid = fork();
	if (pid < 0) return false;
	if (pid > 0) exit(0);

	// Reset environment
	if ((chdir("/")) < 0) return false;
	pid_t sid = setsid();
	if (sid < 0) return false;
    umask(0);

    // Second fork
    pid = fork();
	if (pid < 0) return false;
	if (pid > 0) exit(0);

	// Done
	return true;
}

__attribute__((unused)) static inline void close_stdio(void)
{
	close(STDIN_FILENO);
	close(STDOUT_FILENO);
	close(STDERR_FILENO);
}

__attribute__((unused)) static inline int redirect_std(int stdfd, const char * file_name)
{
	int flags = O_RDWR | O_APPEND;
	if (stdfd == STDIN_FILENO) flags = O_RDONLY;
	if (stdfd != STDIN_FILENO) fsync(stdfd);
	int new_fd = open(file_name, flags);
	if (new_fd < 0) return new_fd;
	if (dup2(new_fd, stdfd) < 0) return -1;
	return new_fd;
}

__attribute__((unused)) static inline StdInOutErrFds redirect_stdfd(const char * stdin_name, const char * stdout_name, const char * stderr_name)
{
	StdInOutErrFds stdfds;
	stdfds.in = redirect_std(STDIN_FILENO, stdin_name);
	stdfds.out = redirect_std(STDOUT_FILENO, stdout_name);
	stdfds.err = redirect_std(STDERR_FILENO, stderr_name);
	return stdfds;
}

__attribute__((unused)) static inline StdInOutErrFds redirect_std2null(void)
{
	return redirect_stdfd("/dev/null", "/dev/null", "/dev/null");
}

__attribute__((unused)) static inline bool is_daemon_self_running(void)
{
	// Get name
	char * name = get_application_name();
	if (name == NULL) return false;
	// Open file
	char fullname[strlen(name) + 5];
	sprintf(fullname, "/tmp/%s.pid", name);
	FILE* f = fopen(fullname, "r");
	if (f == NULL) return false;
	fclose(f);
	if (name) free(name);
	return true;
}

__attribute__((unused)) static inline bool daemon_self_write_pid_file(void)
{
	// Get name
	char * name = get_application_name();
	if (name == NULL) return false;
	bool result = write_pid_file(name);
	if (name) free(name);
	return result;
}

__attribute__((unused)) static inline bool daemon_self_stop(int sig_no)
{
	// Get name
	char * name = get_application_name();
	if (name == NULL) return false;
	// Get pid
	pid_t pid = get_pid_from_file(name);
	if (name) free(name);
	// Send signal
	if (kill(pid, sig_no) == 0) return true;
	return false;
}

__attribute__((unused)) static inline void at_daemon_self_exit(void)
{
	char * name = get_application_name();
	char fullname[strlen(name) + 10];
	sprintf(fullname, "/tmp/%s.pid", name);
	remove(fullname);
	if (name) free(name);
}

#endif //_DAEMON_TOOLS_H_
