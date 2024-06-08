#ifndef _SERIAL_COMM_H_
#define _SERIAL_COMM_H_

#include <unistd.h>
#include <fcntl.h>
#include <termios.h>
#include <iomanip>
#include <sys/poll.h>
#include <sys/file.h>
#include <string>
#include "iostream"

enum SerialCommResult
{
	SERIAL_COMM_EAGAIN_ERROR = -5,
	SERIAL_COMM_WRITE_ERROR = -4,
	SERIAL_COMM_OPEN_FAILED = -3,
	SERIAL_COMM_WRONG_BAUDRATE = -2,
	SERIAL_COMM_POLL_ERROR = -1,
	SERIAL_COMM_SUCCESS = 0
};

class SerialComm 
{

protected:

	std::string d_name;
	speed_t d_baudrate;
	int d_timeout_ms;

	int d_dev_fd;
	struct termios d_tty;
	int d_pollrc; 
    struct pollfd d_pollfd; 

public:

	SerialComm(std::string name, speed_t baudrate, int timeout_ms = 10);
	~SerialComm();

	int setup(bool search_device = false);
	void destroy();
	// Clear RX buffer
	void clear();

	// Read bytes into buf. Returns number of bytes
	int read_to(char * buf, unsigned int size);

	// Write bytes from buf. Returns number of bytes
	int write_from(char * buf, unsigned int size);

};

#endif //_SERIAL_COMM_H_
