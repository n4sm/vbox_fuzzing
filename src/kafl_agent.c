
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


#include "../include/nyx_api.h"

/* assign to a CPU core */
void assign_to_core(int core_id) {
	cpu_set_t mask;
	CPU_ZERO(&mask);
	CPU_SET(core_id, &mask);
	if(sched_setaffinity(getpid(), sizeof(mask), &mask) < 0)
		exit(-1);
	return;
}

int kafl_agent_init(bool verbose)
{
    host_config_t host_config = {0};

    hprintf("Initialize kAFL agent\n");
    // set ready state
    kAFL_hypercall(HYPERCALL_KAFL_ACQUIRE, 0);
    kAFL_hypercall(HYPERCALL_KAFL_RELEASE, 0);

    // filters
    kAFL_hypercall(HYPERCALL_KAFL_SUBMIT_CR3, 0);
    kAFL_hypercall(HYPERCALL_KAFL_USER_SUBMIT_MODE, KAFL_MODE_64);

    kAFL_hypercall(HYPERCALL_KAFL_GET_HOST_CONFIG, (uintptr_t)&host_config);

    if (verbose) {
        hprintf("GET_HOST_CONFIG\n");
        hprintf("\thost magic:  0x%x, version: 0x%x\n",
            host_config.host_magic, host_config.host_version);
        hprintf("\tbitmap size: 0x%x, ijon:    0x%x\n",
            host_config.bitmap_size, host_config.ijon_bitmap_size);
        hprintf("\tpayload size: %u KB\n",
            host_config.payload_buffer_size / 1024);
        hprintf("\tworker id: %d\n", host_config.worker_id);
    }

    if (host_config.host_magic != NYX_HOST_MAGIC) {
        hprintf("HOST_MAGIC mismatch: %08x != %08x\n",
            host_config.host_magic, NYX_HOST_MAGIC);
        habort("HOST_MAGIC mismatch!");
    }

    if (host_config.host_version != NYX_HOST_VERSION) {
        hprintf("HOST_VERSION mismatch: %08x != %08x\n",
            host_config.host_version, NYX_HOST_VERSION);
        habort("HOST_VERSION mismatch!");
    }

    // if (host_config.payload_buffer_size > PAYLOAD_MAX_SIZE) {
    // 	hprintf("Fuzzer payload size too large: %lu > %lu\n",
    // 		host_config.payload_buffer_size, PAYLOAD_MAX_SIZE);
    // 	habort("Host payload size too large!");
    // 	return -1;
    // }

    agent_config_t agent_config = { 0 };
    agent_config.agent_magic = NYX_AGENT_MAGIC;
    agent_config.agent_version = NYX_AGENT_VERSION;
    //agent_config.agent_timeout_detection = 0; // timeout by host
    //agent_config.agent_tracing = 0; // trace by host
    //agent_config.agent_ijon_tracing = 0; // no IJON
    agent_config.agent_non_reload_mode = 0; // no persistent mode
    //agent_config.trace_buffer_vaddr = 0xdeadbeef;
    //agent_config.ijon_trace_buffer_vaddr = 0xdeadbeef;
    agent_config.coverage_bitmap_size = host_config.bitmap_size;
    //agent_config.input_buffer_size;
    //agent_config.dump_payloads; // set by hypervisor (??)

    kAFL_hypercall(HYPERCALL_KAFL_SET_AGENT_CONFIG, (uintptr_t)&agent_config);

    // kAFL_payload* payload_buffer = aligned_alloc((size_t)sysconf(_SC_PAGESIZE), host_config.payload_buffer_size);
    // if (!payload_buffer) {
    //     habort("failed to allocate payload buffer !\n");
    // }

    // // ensure in resident memory
    // mlock(payload_buffer, host_config.payload_buffer_size);
    // // ↔️ mmap shared buffer between QEMU and the fuzzer
    // kAFL_hypercall(HYPERCALL_KAFL_GET_PAYLOAD, (uintptr_t)payload_buffer);

    return 0;
}

#define SCTP_FUZZ 0x1337 

int main ()
{
    assign_to_core(0);
    int fd = socket(AF_INET, SOCK_STREAM, IPPROTO_SCTP);

    // kafl_agent_init(false);

    setsockopt(fd, SOL_SCTP, SCTP_FUZZ, NULL, 0);

    return 0;
}