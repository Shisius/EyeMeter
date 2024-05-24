
#include "udsunicomm.h"

UdsUniComm::UdsUniComm(unsigned char role_id) : d_role_id(role_id)
{
	d_is_alive.store(false);
	d_types_map = get_types_map();
}

UdsUniComm::~UdsUniComm()
{
	stop();
}

void UdsUniComm::stop()
{
	d_is_alive.store(false);
	if (d_recv_thread.joinable())
		d_recv_thread.join();
	close(d_sock);
	unlink(d_sockpath.c_str());
}

int UdsUniComm::start()
{
	if (get_sockpath() != 0) return -1;

	unlink(d_sockpath.c_str());

	struct sockaddr_un _sockaddr;
	memset(&_sockaddr, 0, sizeof(sockaddr_un));
	_sockaddr.sun_family = AF_UNIX;
	strncpy(_sockaddr.sun_path, d_sockpath.c_str(), 104);

	int d_sock = socket(AF_UNIX, SOCK_DGRAM, 0);
	if (d_sock < 0) {
		printf("UdsUniComm:: %s:socket failed\n", d_sockpath.c_str());
		return d_sock;
	}
	bind(d_sock, (struct sockaddr *) &_sockaddr, sizeof(sockaddr_un));

	d_is_alive.store(true);
	d_recv_thread = std::thread(&UdsUniComm::recv_process, this);

	return 0;
}

int UdsUniComm::get_sockpath()
{
	return 0;
}

void UdsUniComm::recv_process()
{

}
