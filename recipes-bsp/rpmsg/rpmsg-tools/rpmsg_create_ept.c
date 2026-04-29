/*
 * Copyright (C) 2025, STMicroelectronics
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 * this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 * this list of conditions and the following disclaimer in the documentation
 * and/or other materials provided with the distribution.
 *
 * 3. Neither the name of the copyright holder nor the names of its contributors
 * may be used to endorse or promote products derived from this software without
 * specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */
#include <sys/ioctl.h>

#include <err.h>
#include <fcntl.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

struct rpmsg_endpoint_info {
	char name[32];
	uint32_t src;
	uint32_t dst;
};
/*
 * Local addresses can be dynamically allocated on-demand
 * range from 0 to 1024 is reserve predefined services.
 * we allow only reseved endpoint
 */
#define RPMSG_RESERVED_ADDRESSES	(1024)

#define RPMSG_CREATE_EPT_IOCTL  _IOW(0xb5, 0x1, struct rpmsg_endpoint_info)

static void usage(int error_code)
{
	extern char *__progname;

	fprintf(stderr, "Usage: %s /dev/rpmsg_ctrl<x> <name> <addr>\n", __progname);
	fprintf(stderr, "   <addr> value must be lower than %d (decimal) or 0x%x (hex)\n",
	        RPMSG_RESERVED_ADDRESSES, RPMSG_RESERVED_ADDRESSES);
	exit(error_code);
}

int main(int argc, char **argv)
{
	struct rpmsg_endpoint_info ept;
	int ret;
	int fd;
	char *endptr;

	if (argc != 4)
		usage(EXIT_FAILURE);

	fd = open(argv[1], O_RDWR);
	if (fd < 0)
		err(1, "failed to open %s\n", argv[1]);

	strncpy(ept.name, argv[2], sizeof(ept.name));
	ept.name[sizeof(ept.name)-1] = '\0';

	// Use base 0 to auto-detect decimal or hex
	ept.src = strtoul(argv[3], &endptr, 0);
	ept.dst = ept.src;

	if (*endptr != '\0' || ept.src >= RPMSG_RESERVED_ADDRESSES)
		usage(EXIT_FAILURE);

	ret = ioctl(fd, RPMSG_CREATE_EPT_IOCTL, &ept);
	if (ret < 0) {
		perror("failed to create endpoint");
		exit(EXIT_FAILURE);
	}
	fprintf(stderr, "Usaendpoint created\n");

	close(fd);
	return 0;
}
