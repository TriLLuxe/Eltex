#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <errno.h>

void error(const char *msg) {
    // Print error message and exit with failure status
    perror(msg);
    exit(1);
}

void upload_file(int sock) {
    // Receive filename prompt from server
    char buffer[1024];
    int n = recv(sock, buffer, sizeof(buffer) - 1, 0);
    if (n <= 0) {
        printf("Server disconnected\n");
        return;
    }
    buffer[n] = '\0';
    printf("S=>C: %s", buffer);

    // Read filename from user
    printf("C=>S: ");
    if (fgets(buffer, sizeof(buffer), stdin) == NULL) {
        printf("Input error\n");
        return;
    }
    buffer[strcspn(buffer, "\n")] = '\0';
    char *filename = buffer;

    // Open file in binary read mode
    FILE *file = fopen(filename, "rb");
    if (file == NULL) {
        printf("ERROR: Cannot open file %s\n", filename);
        return;
    }
    // Send filename
    if (send(sock, buffer, strlen(buffer), 0) < 0) {
        fclose(file);
        error("ERROR sending filename");
    }

    // Read and send file contents in chunks
    while (!feof(file)) {
        int n = fread(buffer, 1, sizeof(buffer), file);
        if (send(sock, buffer, n, 0) < 0) {
            fclose(file);
            error("ERROR sending file data");
        }
    }
    // Send end-of-file marker
    if (send(sock, "EOF", 3, 0) < 0) {
        fclose(file);
        error("ERROR sending EOF");
    }
    fclose(file);

    // Receive server response
    n = recv(sock, buffer, sizeof(buffer) - 1, 0);
    if (n > 0) {
        buffer[n] = '\0';
        printf("Server response: %s", buffer);
    }
}

void download_file(int sock) {
    // Receive file list and prompt from server
    char buffer[1024];
    while (1) {
        int n = recv(sock, buffer, sizeof(buffer) - 1, 0);
        if (n <= 0) {
            printf("Server disconnected\n");
            return;
        }
        buffer[n] = '\0';
        printf("S=>C: %s", buffer);
        // Break when filename prompt is received
        if (strncmp(buffer, "Enter filename for download", 26) == 0) {
            break;
        }
    }

    // Read filename from user
    printf("C=>S: ");
    if (fgets(buffer, sizeof(buffer), stdin) == NULL) {
        printf("Input error\n");
        return;
    }
    buffer[strcspn(buffer, "\n")] = '\0';
    char *filename = buffer;

    // Send filename
    if (send(sock, buffer, strlen(buffer), 0) < 0) {
        error("ERROR sending filename");
    }

    // Open file in binary write mode
    FILE *file = fopen(filename, "wb");
    if (file == NULL) {
        printf("ERROR: Cannot create file %s\n", filename);
        return;
    }
    // Receive file data until EOF or error
    while (1) {
        int n = recv(sock, buffer, sizeof(buffer), 0);
        if (n <= 0) {
            break;
        }
        if (strncmp(buffer, "EOF", 3) == 0) {
            break;
        }
        if (strncmp(buffer, "ERROR:", 6) == 0) {
            buffer[n] = '\0';
            printf("Server response: %s", buffer);
            fclose(file);
            return;
        }
        // Write received data to file
        fwrite(buffer, 1, n, file);
    }
    fclose(file);
    printf("File %s downloaded successfully\n", filename);
}

int main(int argc, char *argv[]) {
    int my_sock, portno, n;
    struct sockaddr_in serv_addr;
    struct hostent *server;
    char buff[1024];

    // Print client startup message
    printf("TCP DEMO CLIENT\n");

    // Check command-line arguments
    if (argc < 3) {
        fprintf(stderr, "usage %s hostname port\n", argv[0]);
        exit(1);
    }

    // Convert port number and validate
    portno = atoi(argv[2]);
    if (portno <= 0 || portno > 65535) {
        fprintf(stderr, "ERROR, invalid port number\n");
        exit(1);
    }

    // Create TCP socket
    my_sock = socket(AF_INET, SOCK_STREAM, 0);
    if (my_sock < 0) error("ERROR opening socket");

    // Resolve hostname
    server = gethostbyname(argv[1]);
    if (server == NULL) {
        fprintf(stderr, "ERROR, no such host\n");
        exit(1);
    }

    // Set up server address structure
    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    memcpy(&serv_addr.sin_addr.s_addr, server->h_addr_list[0], server->h_length);
    serv_addr.sin_port = htons(portno);

    // Connect to server
    if (connect(my_sock, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0)
        error("ERROR connecting");

    // Main client loop
    while (1) {
        // Receive menu or prompt from server
        n = recv(my_sock, buff, sizeof(buff) - 1, 0);
        if (n <= 0) {
            printf("Server disconnected\n");
            break;
        }
        buff[n] = '\0';
        printf("S=>C: %s", buff);

        // Prompt for user input
        printf("C=>S: ");
        if (fgets(buff, sizeof(buff), stdin) == NULL) {
            printf("Input error\n");
            break;
        }

        // Send command to server
        if (send(my_sock, buff, strlen(buff), 0) < 0) {
            error("ERROR sending to socket");
        }

        // Handle file operations
        char *endptr;
        errno = 0;
        long operation = strtol(buff, &endptr, 10);
        if (endptr != buff && *endptr == '\n' && errno == 0 && operation >= 1 && operation <= 7) {
            if (operation == 5) {
                upload_file(my_sock);
            } else if (operation == 6) {
                download_file(my_sock);
            } else if (operation == 7) {
                printf("Exiting...\n");
                break;
            }
        }
    }

    // Close socket and exit
    close(my_sock);
    return 0;
}