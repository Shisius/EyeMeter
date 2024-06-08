
#include <csignal>
#include <getopt.h>
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

	return 0;
}
 