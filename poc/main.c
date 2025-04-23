
#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <stdbool.h>
#include <sys/mman.h>

#include <time.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netinet/sctp.h>
#include <arpa/inet.h>
#include <sched.h>

unsigned char workdir_corpus_timeout_payload_00042[] = {
  0x00, 0x00, 0x00, 0x40, 0x10, 0x25, 0x00, 0x00
};
unsigned int workdir_corpus_timeout_payload_00042_len = 8;


#define SIZE 0x2510
#define REQUEST 0xb
    
/* assign to a CPU core */
void assign_to_core(int core_id) {
	cpu_set_t mask;
	CPU_ZERO(&mask);
	CPU_SET(core_id, &mask);
	if(sched_setaffinity(getpid(), sizeof(mask), &mask) < 0)
		exit(-1);
	return;
}

int main ()
{
    assign_to_core(0);
    int fd = socket(AF_INET, SOCK_STREAM, IPPROTO_SCTP);

    setsockopt(fd, SOL_SCTP, REQUEST, workdir_corpus_timeout_payload_00042+8, SIZE);

    return 0;
}
