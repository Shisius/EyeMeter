
#include "udsunicomm.h"

UdsUniComm::UdsUniComm(unsigned char role_id) : d_role_id(role_id)
{
	d_is_alive.store(false);
	d_types_map = get_types_map();
	// d_types_map_rev = get_types_map_reversed(); //get_reversed_map(d_types_map);
	d_poll_timeout_ms = 100;
}

UdsUniComm::~UdsUniComm()
{
	stop();
}

void UdsUniComm::stop()
{
	d_pack_queue.unblock_waiting();
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
	usleep(10000);

	struct sockaddr_un _sockaddr;
	memset(&_sockaddr, 0, sizeof(sockaddr_un));
	_sockaddr.sun_family = AF_UNIX;
	strncpy(_sockaddr.sun_path, d_sockpath.c_str(), d_sockpath.size());

	d_sock = socket(PF_UNIX, SOCK_DGRAM, 0);
	if (d_sock < 0) {
		printf("UdsUniComm:: %s:socket failed\n", d_sockpath.c_str());
		return d_sock;
	}
	//printf("UdsUniComm::sock = %d\n", d_sock);
	int result = bind(d_sock, (struct sockaddr *) &_sockaddr, sizeof(sockaddr_un));
	if (result != 0) {
		printf("UdsUniComm:: %s:bind failed\n", d_sockpath.c_str());
		return -1;
	}
	//printf("UdsUniComm::sock = %d\n", d_sock);
	d_pollfd.fd = d_sock;
    d_pollfd.events = POLLIN;
    //printf("UdsUniComm::sock = %d\n", d_sock);

	d_is_alive.store(true);
	d_recv_thread = std::thread(&UdsUniComm::recv_process, this);

	return 0;
}

int UdsUniComm::get_sockpath()
{
	char * line_ptr = NULL;
    size_t line_len = 0;
    ssize_t line_n_read = 0;
    d_othersocks.clear();
	FILE * fd = fopen(UDS_UNI_NAMES_FILEPATH.c_str(), "r");
	if (fd == NULL) {
		perror("UdsUniComm::read roles file failed\n");
		return -1;
	}
	while ((line_n_read = getline(&line_ptr, &line_len, fd)) != -1) {
		char role_name[32];
		unsigned char role = 0;
        // printf("UdsUniComm::get_sockpath: %s\n", line_ptr);
        if (sscanf(line_ptr, "%hhd %s", &role, role_name) > 0) {
        	if (role == d_role_id) {
        		d_sockpath = UDS_UNI_SOCK_FOLDER + "/" + std::string(role_name) + UDS_UNI_SOCK_EXT;
        		printf("UdsUniComm::my sock is %s\n", d_sockpath.c_str());
        	}
        	else {
        		struct sockaddr_un new_addr;
        		new_addr.sun_family = AF_UNIX;
        		std::string new_sock_name = UDS_UNI_SOCK_FOLDER + "/" + std::string(role_name) + UDS_UNI_SOCK_EXT;
        		strncpy(new_addr.sun_path, new_sock_name.c_str(), new_sock_name.size());
        		d_othersocks.insert(std::pair<unsigned char, struct sockaddr_un>({role, new_addr}));
        		printf("UdsUniComm::other sock %d %s\n", role, new_sock_name.c_str());
        	}
        }
    }
    if (line_ptr) free(line_ptr);
    fclose(fd);
	return 0;
}

int UdsUniComm::send(unsigned char title, unsigned char role)
{
	UdsUniMsg msghdr;
	msghdr.proto = UDSUNI_PROTO_PTTS4;
	msghdr.title = title;
	msghdr.type = 0;
	msghdr.size = 0;
	int result = -1;
	if (d_othersocks.count(role) > 0) {
		result = sendto(d_sock, &msghdr, sizeof(UdsUniMsg), 0, (struct sockaddr *) &(d_othersocks.at(role)), sizeof(sockaddr_un));
	} else if (role == EYEMETER_ROLE_ALL) {
		result = 0;
		for (auto & p : d_othersocks) {
			result += sendto(d_sock, &msghdr, sizeof(UdsUniMsg), 0, (struct sockaddr *) &(p.second), sizeof(sockaddr_un));
		}
	}
	if (result < 0) printf("UdsUniComm:: send error %d\n", errno);
	printf("UdsUniComm::send = %d\n", result);
	return result;
}

// template<typename T>
// int UdsUniComm::send(unsigned char title, T data, unsigned char role)
// {
// 	UdsUniMsg msghdr;
// 	msghdr.proto = UDSUNI_PROTO_PTTS4;
// 	msghdr.title = title;
// 	if (d_types_map.count(typeid(T)) > 0) {
// 		msghdr.type = d_types_map.at(typeid(T));
// 	} else msghdr.type = USDUNI_TYPE_UNKNOWN;
// 	msghdr.size = sizeof(T);

// 	int result = -1;
// 	char msg[sizeof(UdsUniMsg) + sizeof(T)];
// 	memcpy(msg, &msghdr, sizeof(UdsUniMsg));
// 	memcpy(msg+sizeof(UdsUniMsg), &data, sizeof(T));
// 	if (d_othersocks.count(role) > 0) {
// 		result = sendto(d_sock, msg, sizeof(UdsUniMsg)+sizeof(T), 0, (struct sockaddr *) &(d_othersocks.at(role)), sizeof(sockaddr_un));
// 	} else if (role == EYEMETER_ROLE_ALL) {
// 		result = 0;
// 		for (auto & p : d_othersocks) {
// 			result += sendto(d_sock, msg, sizeof(UdsUniMsg)+sizeof(T), 0, (struct sockaddr *) &(p.second), sizeof(sockaddr_un));
// 		}
// 	}
// 	return result;
// }

void UdsUniComm::recv_process()
{
	while (d_is_alive.load()) {
		int pollrc = poll(&d_pollfd, 1, d_poll_timeout_ms);
		// printf("UdsUniComm::pol = %d\n", pollrc);
		if (pollrc < 0) {
			printf("UdsUniComm:: poll error %d\n", pollrc);
			continue;
		}
		if (pollrc == 0) continue;
		if (d_pollfd.revents & POLLIN) {
			struct sockaddr_un addr;
			char buf[sizeof(UdsUniMsg)+256];
			socklen_t addr_len = sizeof(struct sockaddr_un);
			// printf("UdsUniComm::before recvfrom %d\n", d_sock);
			int result = recvfrom(d_sock, buf, sizeof(UdsUniMsg)+256, 0, (struct sockaddr*)&addr, &addr_len);
			// printf("UdsUniComm::after recvfrom %d\n", result);
			if (result <= 0) {
				if (errno == EAGAIN) continue;
				printf("UdsUniComm::recfrom error = %d, %s, %d\n", errno, strerror(errno), d_sock);
				break;
			}
			if (result >= int(sizeof(UdsUniMsg))) {
				UdsUniPack pack;
				memcpy(&(pack.msg), buf, sizeof(UdsUniMsg));
				printf("UdsUniComm:: new msg %d\n", pack.msg.title);
				if (pack.msg.proto != UDSUNI_PROTO_PTTS4) {
					printf("UdsUniComm: wrong proto %d", pack.msg.proto);
					continue;
				}
				if (pack.msg.size > 0) {
					if (pack.msg.size + int(sizeof(UdsUniMsg)) != result) {
						printf("UdsUniComm: msg size %d != UdsUniMsg + data\n", result);
						continue;
					}
					if (pack.msg.size != get_size_by_code(pack.msg.type)) {
						printf("UdsUniComm: msg size %d != type size %d\n", pack.msg.size, pack.msg.type);
						continue;
					}
					pack.put_data(buf + sizeof(UdsUniMsg));
				}
				d_pack_queue.push(pack);
			} else {
				printf("UdsUniComm: msg size %d < UdsUniMsg\n", result);
			}
		}
	}
}

int UdsUniComm::try_recv(UdsUniPack & pack)
{
	if (d_pack_queue.try_pop(pack)) return 0;
	return -1;
}
int UdsUniComm::wait_recv(UdsUniPack & pack)
{
	if (d_pack_queue.wait_and_pop(pack)) return 0;
	return -1;
}

void UdsUniPack::put_data(void * ptr)
{
	if (data_ptr != nullptr) free(data_ptr);
	data_ptr = malloc(msg.size);
	memcpy(data_ptr, ptr, msg.size);
}
