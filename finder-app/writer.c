#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <syslog.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>

int main(int argc, char *argv[]) 
{
    openlog("writer", LOG_PERROR | LOG_CONS, LOG_USER);

    if (argc != 3) 
    {
        syslog(LOG_ERR, "Invalid number of arguments. Usage: writer <file> <string>");
        fprintf(stderr, "Usage: %s <file> <string>\n", argv[0]);
        closelog();
        return 1;
    }

    const char *file_path = argv[1];
    const char *write_str = argv[2];

    int fd = open(file_path, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (fd == -1) 
    {
        syslog(LOG_ERR, "Error opening/creating file %s: %s", file_path, strerror(errno));
        perror("Error opening/creating file");
        closelog();
        return 1;
    }

    ssize_t bytes_written = write(fd, write_str, strlen(write_str));
    if (bytes_written == -1) 
    {
        syslog(LOG_ERR, "Error writing to file %s: %s", file_path, strerror(errno));
        perror("Error writing to file");
        close(fd);
        closelog();
        return 1;
    }

    syslog(LOG_DEBUG, "Writing '%s' to '%s'", write_str, file_path);

    close(fd);
    closelog();

    return 0;
}

