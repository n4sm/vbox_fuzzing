#!/usr/bin/python3

import os

CORPUS_FILE = "workdir/corpus/timeout/payload_00042"
OUTPUT_PROGRAM = "poc/main.c"

def _compile():
    os.system("gcc poc/main.c -static -o poc/main")

def main():
    f = open(CORPUS_FILE, "rb").read()
    out = open(OUTPUT_PROGRAM, "w")

    request = int.from_bytes(f[0:4], byteorder='little') % 37
    size = int.from_bytes(f[4:8], byteorder='little')

    os.system(f"xxd -i {CORPUS_FILE} > /tmp/out")

    content = f"""
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

{open("/tmp/out", "r").read()}

#define SIZE {hex(size)}
#define REQUEST {hex(request)}
    
/* assign to a CPU core */
void assign_to_core(int core_id) {{
	cpu_set_t mask;
	CPU_ZERO(&mask);
	CPU_SET(core_id, &mask);
	if(sched_setaffinity(getpid(), sizeof(mask), &mask) < 0)
		exit(-1);
	return;
}}

int main ()
{{
    assign_to_core(0);
    int fd = socket(AF_INET, SOCK_STREAM, IPPROTO_SCTP);

    setsockopt(fd, SOL_SCTP, REQUEST, {CORPUS_FILE.replace("/", "_")}+8, SIZE);

    return 0;
}}
"""

    out.write(content)
    _compile()
    
if __name__ == '__main__':
    main()