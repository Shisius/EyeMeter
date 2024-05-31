#ifndef _UDSUNICOMM_H_
#define _UDSUNICOMM_H_

#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/poll.h>
#include <sys/file.h>
//#include <netdb.h>
#include <netinet/in.h> 
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <netdb.h>
#include <stdio.h>
#include <sys/un.h>
#include <functional>
#include <thread>
#include <atomic>
// #define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>

#include "threadsafe_queue.h"
#include "types_map.hpp"

static const std::string UDS_UNI_NAMES_FILEPATH = "/etc/roles.names";
static const std::string UDS_UNI_SOCK_FOLDER = "/tmp";
static const std::string UDS_UNI_SOCK_EXT = ".sock";

struct UdsUniPack
{
	UdsUniMsg msg;
	void * data_ptr = nullptr;

	UdsUniPack() : data_ptr(nullptr) {}
	~UdsUniPack() {if (data_ptr) free(data_ptr);}

	template<typename T>
	int get_data(T & data);

	void put_data(void * ptr);
};

class UdsUniComm
{
public:

	UdsUniComm(unsigned char role_id);
	~UdsUniComm();

	int start();
	void stop();

	/**
	 * Overloaded send for sendind any legal stuff
	 */
	int send(unsigned char title, unsigned char role = EYEMETER_ROLE_ALL);

	template<typename T>
	int send(unsigned char title, T data, unsigned char role = EYEMETER_ROLE_ALL);

	int try_recv(UdsUniPack & pack);
	int wait_recv(UdsUniPack & pack);

protected:

	unsigned char d_role_id;
	int d_sock;
	struct pollfd d_pollfd;
	int d_poll_timeout_ms;
	std::string d_sockpath;
	std::map<unsigned char, struct sockaddr_un> d_othersocks;

	std::map<std::type_index, unsigned char> d_types_map;
	// std::map<unsigned char, std::type_index> d_types_map_rev;

	//std::function<int(UdsUniMsg msg)> d_cmd_handler;

	std::atomic<bool> d_is_alive;
	std::thread d_recv_thread;
	threadsafe_queue<UdsUniPack> d_pack_queue;

	int get_sockpath();
	void recv_process();

};

#endif //_UDSUNICOMM_H_
