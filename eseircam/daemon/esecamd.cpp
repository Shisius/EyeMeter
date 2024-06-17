
#include <csignal>
#include <getopt.h>
#include "daemon_tools.h"
#include "esecam_master.h"

std::unique_ptr<EseCamMaster> cammaster;
std::mutex cammaster_mut;

void sigint_handler(int signum)
{
	std::lock_guard<std::mutex> lg(cammaster_mut);
	printf("SIGINT. EseCamMaster will be destroyed\n");
	cammaster->stop();
}

int main(int argc, char ** argv)
{
	StdInOutErrFds stdfds;
	bool run_as_daemon = false;
	bool exit_daemon = false;
	int charopt = -1;
	while (true) {
		charopt = getopt(argc, argv, "dxh");
		if (charopt == -1) break;
		switch (charopt) {
			case 'd':
				run_as_daemon = true;
				break;
			case 'x':
				exit_daemon = true;
				run_as_daemon = false;
				if (!is_daemon_self_running()) {
					printf("No daemon running.\n");
					break;
				}
				if (daemon_self_stop(SIGINT)) {
					sleep(1);
					if (is_daemon_self_running()) {
						printf("Cannot stop daemon! Try again\n");
					} else {
						printf("Daemon stopped\n");
					}
				} else {
					printf("No daemon running\n");
				}
				break;
			case 'h':
				printf("EyeMeter: available options:\n \
						h - print this help\n \
						d - run as daemon\n \
						x - stop the daemon\n");
				return 0;
			default:
				printf("Unknown option 0%o\n", charopt);
		}

	}

	// Check if already running
	// if (is_daemon_self_running()) {
	// 	printf("Daemon is already running. \n");
	// 	if (daemon_self_stop(SIGINT)) {
	// 		sleep(1);
	// 		if (is_daemon_self_running()) {
	// 			printf("Cannot stop daemon! Try again\n");
	// 			return 1;
	// 		} else {
	// 			printf("Daemon stopped\n");
	// 		}
	// 	} else {
	// 		printf("No daemon running\n");
	// 	}	
	// 	//return 1;
	// }
	
	if (is_daemon_self_running()) {
		printf("Daemon is already running. Stop it before run again.\n");
		return 0;
	}
	if (exit_daemon) return 0;

	if (run_as_daemon) {
		printf("Starting daemon\n");
		if (!(doublefork())) printf("Double fork failed\n");
		stdfds = redirect_std2null();
		if (!daemon_self_write_pid_file()) return 1;
	}

	signal(SIGINT, sigint_handler);
	
	cammaster = std::make_unique<EseCamMaster>();

	if (cammaster->setup() == 0) 
	{

		while (true) {
			{
				std::lock_guard<std::mutex> lg(cammaster_mut);
				if (!(cammaster->is_alive()))
					break;
			}
			std::this_thread::sleep_for(std::chrono::milliseconds(100));
		}

	} else {
		printf("!!!!!!!!!!! Setup failed!!!!!!!!!!!!!\n");
	}

	printf("Cammaster will be deleted\n");

	cammaster.reset();

	if (run_as_daemon)
		at_daemon_self_exit();

	return 0;
}
 