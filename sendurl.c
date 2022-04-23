#include <netinet/in.h>
#define _GNU_SOURCE
#include <stdio.h>
#include <getopt.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/stat.h>
#include <string.h>
#include <unistd.h>

struct fbi_url
{
	uint32_t len;
	char *bytes;
	int string_len;
};

int
main(int argc, char **argv)
{
	char *remote_ip = NULL;
	int port = 5000; /* FBI uses port 5000 by default */
	char *url_to_send = NULL;

	if(argc < 2) {
		fprintf(stderr,"USAGE: %s -i <IP ADDRESS> -u <URL TO SEND>\n",argv[0]);
		return -1;
	}

	int c;
	while((c = getopt(argc, argv, "i:u:")) != -1) {
		switch(c) {
			case 'i':
				remote_ip = optarg;
				break;
			case 'u':
				url_to_send = optarg;
				break;
		}
	}
	if(url_to_send == NULL || remote_ip == NULL) {
		fprintf(stderr,"USAGE: %s -i <IP ADDRESS> -u <URL TO SEND>\n",argv[0]);
		return -1;
	}

	int sockfd = socket(AF_INET, SOCK_STREAM, 0);

	if(sockfd == -1) {
		perror("socket");
		return -1;
	}

	struct sockaddr_in addr;
	addr.sin_port = htons(port);
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = inet_addr(remote_ip);
	socklen_t socklen = sizeof(addr);

	if(connect(sockfd, (struct sockaddr *)&addr, socklen) == -1) {
		perror("connect");
		return -1;
	}

	struct fbi_url data;
	int remote_str_len = strlen(url_to_send);
	puts(url_to_send);
	
	data.string_len = remote_str_len;
	data.len = ntohl(data.string_len);
	
	data.bytes = url_to_send;

	puts("SENDING URL...");
	
	write(sockfd, &data.len, sizeof(uint32_t));
	write(sockfd, data.bytes, data.string_len);
	puts("URL sent. See ya!\n");
	
	return 0;
}
