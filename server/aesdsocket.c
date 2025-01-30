#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <syslog.h>
#include <errno.h>

#define PORT 9000
#define BACKLOG 5
#define DATA_FILE "/var/tmp/aesdsocketdata"

int server_socket = -1;
int client_socket = -1;
int data_file_fd = -1;

void handle_signal(int signo) {
    syslog(LOG_INFO, "Caught signal, exiting");

    if(signo!=SIGINT && signo!=SIGTERM)
        return;

    if (client_socket >= 0) {
        close(client_socket);
    }
    if (server_socket >= 0) {
        close(server_socket);
    }
    if (data_file_fd >= 0) {
        close(data_file_fd);
        unlink(DATA_FILE);
    }

    closelog();
    exit(0);
}

void daemonize() {
    pid_t pid = fork();

    if (pid < 0) {
        perror("fork");
        exit(EXIT_FAILURE);
    }

    if (pid > 0) {
        exit(0);
    }

    if (setsid() < 0) {
        perror("setsid");
        exit(EXIT_FAILURE);
    }

    if (chdir("/") < 0) {
        perror("chdir");
        exit(EXIT_FAILURE);
    }

    close(STDIN_FILENO);
    close(STDOUT_FILENO);
    close(STDERR_FILENO);
}

int main(int argc, char *argv[]) {
    struct sockaddr_in server_addr, client_addr;
    socklen_t client_addr_len;
    char buffer[1024];
    ssize_t bytes_received, bytes_written;
    
    openlog("aesdsocket", LOG_PID, LOG_USER);

    if (argc == 2 && strcmp(argv[1], "-d") == 0) {
        daemonize();
    }
    
    signal(SIGINT, handle_signal);
    signal(SIGTERM, handle_signal);

    server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket < 0) {
        syslog(LOG_ERR, "Socket creation failed: %s", strerror(errno));
        return -1;
    }

    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    server_addr.sin_port = htons(PORT);

    if (bind(server_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        syslog(LOG_ERR, "Bind failed: %s", strerror(errno));
        close(server_socket);
        return -1;
    }

    if (listen(server_socket, BACKLOG) < 0) {
        syslog(LOG_ERR, "Listen failed: %s", strerror(errno));
        close(server_socket);
        return -1;
    }

    data_file_fd = open(DATA_FILE, O_CREAT | O_RDWR | O_APPEND, 0644);
    if (data_file_fd < 0) {
        syslog(LOG_ERR, "Failed to open data file: %s", strerror(errno));
        close(server_socket);
        return -1;
    }

    //*****************************************
   
    while (1) {
        client_addr_len = sizeof(client_addr);
        client_socket = accept(server_socket, (struct sockaddr *)&client_addr, &client_addr_len);
        if (client_socket < 0) {
            syslog(LOG_ERR, "Accept failed: %s", strerror(errno));
            continue;
        }

        syslog(LOG_INFO, "Accepted connection from %s", inet_ntoa(client_addr.sin_addr));

        while ((bytes_received = recv(client_socket, buffer, sizeof(buffer), 0)) > 0) {
            syslog(LOG_INFO, "Recebidos %ld bytes", bytes_received);

            if(bytes_received!=sizeof(buffer)){
                syslog(LOG_INFO, "Ultimo byte e %c (%d)", buffer[bytes_received-1], buffer[bytes_received-1]);
		    
		if(buffer[bytes_received-1]!='\n'){
                    buffer[bytes_received]='\n';
                    bytes_received++;
		}
	    }
            bytes_written = write(data_file_fd, buffer, bytes_received);
	    syslog(LOG_INFO, "Enviados %ld bytes", bytes_written);

            if (bytes_written < 0) {
                syslog(LOG_ERR, "Failed to write to data file: %s", strerror(errno));
                break;
            }

	    if(bytes_received!=sizeof(buffer)){
                lseek(data_file_fd, 0, SEEK_SET);

                while ((bytes_written = read(data_file_fd, buffer, sizeof(buffer))) > 0) {
                    send(client_socket, buffer, bytes_written, 0);
                }
	    }
        }

        syslog(LOG_INFO, "Closed connection from %s", inet_ntoa(client_addr.sin_addr));
        close(client_socket);
        client_socket = -1;
    }

    return 0;
}

