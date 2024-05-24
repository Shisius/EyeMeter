#ifndef _UDSUNICOMM_H_
#define _UDSUNICOMM_H_

#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
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

#include "threadsafe_queue.h"
#include "types_map.hpp"

static const std::string UDS_UNI_NAMES_FILEPATH = "/etc/usdunicomm.names";
static const std::string UDS_UNI_SOCK_FOLDER = "/tmp";

struct UdsUniPack
{
	UdsUniMsg msg;
	void * data_ptr = nullptr;

	template<typename T>
	int get_data(T & data);


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
	int send(unsigned char title);

	template<typename T>
	int send(unsigned char title, T data);

protected:

	unsigned char d_role_id;
	int d_sock;
	std::string d_sockpath;
	std::vector<struct sockaddr_un> d_othersocks;

	std::map<std::type_index, unsigned char> d_types_map;

	//std::function<int(UdsUniMsg msg)> d_cmd_handler;

	std::atomic<bool> d_is_alive;
	std::thread d_recv_thread;

	int get_sockpath();
	void recv_process();

};

#endif //_UDSUNICOMM_H_
