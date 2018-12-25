#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <string.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <regex.h>

#define READ_SIZE 1024

int get_http_socket(const char* host, const char* port) {
	struct addrinfo hints, *res = NULL;
	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;

	int ret = getaddrinfo(host, port, &hints, &res); 
	if (ret) {
		fprintf(stderr, "ERROR! getaddrinfo(%s:%s) failed\n%s\n", host, port, gai_strerror(ret));
		return 0;
	}

	int s = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
	if (connect(s, res->ai_addr, res->ai_addrlen)) {
		fprintf(stderr, "FATAL: connect(%s:%s) failed\n#%d: %s\n", host, port, errno, strerror(errno));
		return 0;
	}

	return s;
}

int send_request(int sockfd, const char* http, char* out) {
	size_t bytes_sent       = 0;
	size_t total_bytes_sent = 0;
	size_t bytes_to_send    = strlen(http);
	while (1) {
		bytes_sent = send(sockfd, http, strlen(http), 0);
		total_bytes_sent += bytes_sent;
		if (total_bytes_sent >= bytes_to_send)
			break;
	}

	size_t bytes_received;
	char tmp[READ_SIZE];
	int ret_val = 1, offset = 0;
	while (1) {
		bytes_received = recv(sockfd, tmp, READ_SIZE + 1, 0);

		if (bytes_received == -1) {
			ret_val = -1;
			break;
		} else if (bytes_received == 0)
			break;

		if (bytes_received > 0) {
			memcpy(out + offset, tmp, bytes_received);
			offset += bytes_received;
		}
	}
	out[offset + 1] = '\0';

	return ret_val;
}

char* replace_str(char *str, char *orig, char *rep) {
	static char buffer[4096];
	char *p;
	int i = 0;

	while (str[i]){
		if (!(p=strstr(str+i,orig)))
			return str;

		strncpy(buffer + strlen(buffer), str + i, (p - str) - i);
		buffer[p-str] = '\0';
		strcat(buffer, rep);
		i = (p - str) + strlen(orig);
	}

	return buffer;
}

int main(int argc, const char* argv[]) {
	if (argc <= 1) {
		fprintf(stderr, "ERROR! No arguments passed\n");
		return 1;
	}

	regex_t url_reg;
	if (regcomp(&url_reg, "^[a-zA-Z]+.[a-zA-Z]+.[a-zA-Z]+:[0-9]+$", REG_EXTENDED)) {
		fprintf(stderr, "ERROR! regcomp() failed\n%d: %s\n", errno, strerror(errno));
		return 1;
	}

	FILE* fp = NULL;
	char *url, *port, *body;
	char resp[READ_SIZE];
	int sockfd;
	for (int i = 1; i < argc; ++i) {
		fp = fopen(argv[i], "r");
		if (!fp) {
			fprintf(stderr, "ERROR! Failed to open \"%s\"\n#%d: %s\n", argv[i], errno, strerror(errno));
			return 1;
		}

		fseek(fp, 0, SEEK_END);
		size_t length = ftell(fp);
		rewind(fp);

		char* data = (char*)malloc(length * sizeof(char));
		fread(data, 1, length, fp);
		char* data_dup = strdup(data);

		url = strtok(data_dup, "\n");
		if (regexec(&url_reg, url, (size_t)0, NULL, 0)) {
			fprintf(stderr, "ERROR! First line of \"%s\" not a URL\n", argv[i]);
			goto END;
		}

		port = strtok(url, ":");
		port = strtok(NULL, ":");

		if (!(sockfd = get_http_socket(url, port)))
			goto END;

		body = data + strlen(url) + strlen(port) + 2;
		printf("%s", body);
		body = replace_str(body, "\n", "\\r\\n");
		if (send_request(sockfd, body, resp) <= 0) {
			fprintf(stderr, "ERROR! send_request() failed\n");
			goto END;
		}
		printf("%s\n", resp);

		shutdown(sockfd, SHUT_RDWR);
		close(sockfd);

END:
		free(data);
		free(data_dup);
		fclose(fp);
	}

	regfree(&url_reg);
	return 0;
}
