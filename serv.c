#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <pthread.h>
#include <string.h>
#include <stdio.h>

#include "const.c"

int main()
{
	int sock,listener;
	struct sockaddr_in addr;
	int addrlen;
	long int filesize = 0;
	char filename[SIZE], answer[SIZE], ans;
	char buf[BUF_SIZE];
	int bytesread = 0;

	listener = socket(AF_INET, SOCK_STREAM, 0);

	addr.sin_family = AF_INET;
	addr.sin_port = htons(3425);
	addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
	bind(listener, (struct sockaddr *)&addr, sizeof(addr));

	listen(listener, 8);

	while (1) {
		sock = accept(listener, 0, 0);
		recv(sock, filename, SIZE, 0);
		printf("file requested: %s\n", filename);
		FILE *fin = fopen(filename, "rb");
		if (fin) {
			fseek(fin, 0, SEEK_END);
			filesize = ftell(fin);
			snprintf(answer, SIZE, "file size is %d B, do you want to continue? [y/n] ", filesize);
		}
		else {
			strcpy(answer, "error: no such file\n");
			printf("error: no such file\n");
			send(sock, answer, SIZE, 0);
			continue;
		}

		send(sock, answer, SIZE, 0);
		recv(sock, &ans, 1, 0);
		if (ans == 'n') {
			printf("operation cancelled\n");
			close(sock);
			continue;
		}

		fseek(fin, 0, SEEK_SET);
		bytesread = fread(buf, 1, BUF_SIZE, fin);
		while (bytesread) {
			send(sock, buf, bytesread, 0);
			bytesread = fread(buf, 1, BUF_SIZE, fin);
		}
		close(sock);
		fclose(fin);
		printf("file was successfully copied\n");
	}
	return 0;
}
