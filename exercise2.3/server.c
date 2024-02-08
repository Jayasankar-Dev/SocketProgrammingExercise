#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <netinet/in.h>
#include <sys/wait.h>

int main(void)
{
    struct sockaddr_in server_addr, client_addr;
    int server_fd, client_addr_size, connected_sock_fd;
    int ret, ret_val = 0, option, status;
    FILE *req_file = NULL;
    pid_t child_pid, wait_pid;
    char buffer[256];

    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd < 0)
    {
        ret_val = -1;
        printf("Failed in openning socket %d<%s>\n", errno, strerror(errno));
    }
    else
    {
        option = 1;
        setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &option, sizeof(option));

        server_addr.sin_family = AF_INET;
        server_addr.sin_addr.s_addr = INADDR_ANY;
        server_addr.sin_port = htons(5001);

        ret = bind(server_fd, (struct sockaddr *)&server_addr, sizeof(server_addr));
        if (ret < 0)
        {
            ret_val = -1;
            printf("Failed in binding socket %d<%s>\n", errno, strerror(errno));
        }
        else
        {
            ret = listen(server_fd, 5);
            if (ret < 0)
            {
                ret_val = -1;
                printf("Failed in registering queue size for socket %d<%s>\n", errno, strerror(errno));
            }
            else
            {
                // printf("waiting for client connection \n");
                client_addr_size = sizeof(client_addr);
                while (1)
                {
                    connected_sock_fd = accept(server_fd, (struct sockaddr *)&client_addr, &client_addr_size);
                    if (connected_sock_fd < 0)
                    {
                        ret_val = -1;
                        printf("Failed in creating a new socket for a new request from queue %d<%s>\n", errno, strerror(errno));
                    }
                    else
                    {
                        if ((child_pid = fork()) == 0)
                        {
                            printf("A new child process created for concurrent socket operation for connected socket %d \n", connected_sock_fd);
                            /* Child process */
                            while (1)
                            {
                                ret = recv(connected_sock_fd, buffer, sizeof(buffer), 0);
                                if (ret < 0)
                                {
                                    ret_val = -1;
                                    printf("Failed in reading from connected sock %d<%s>\n", errno, strerror(errno));
                                }
                                else
                                {
                                    req_file = fopen(buffer, "r");
                                    if (req_file == NULL)
                                    {
                                        strcpy(buffer, "cannot open file\n");
                                        ret = send(connected_sock_fd, buffer, sizeof(buffer), 0);
                                        if (ret < 0)
                                        {
                                            ret_val = -1;
                                            printf("Failed in sending response to connected socket %d<%s>\n", errno, strerror(errno));
                                        }
                                        else
                                        {
                                            /* Nothing to do */
                                        }
                                    }
                                    else
                                    {
                                        memset(buffer, '\0', sizeof(buffer));
                                        // strcpy(buffer, "file exists\n");
                                        while ((ret = feof(req_file)) == 0)
                                        {
                                            ret = fread(buffer, sizeof(buffer), 1, req_file);

                                            ret = send(connected_sock_fd, buffer, sizeof(buffer), 0);
                                            if (ret < 0)
                                            {
                                                ret_val = -1;
                                                printf("Failed in sending response to connected socket %d<%s>\n", errno, strerror(errno));
                                            }
                                            else
                                            {
                                                /* Nothing to do */
                                            }

                                        }

                                        fclose(req_file);
                                        req_file = NULL;
                                    }
                                }
                            }
                        }
                        else
                        {
                            /* Parant process */
                            // while ((wait_pid = wait(&status)) > 0);
                        }
                    }
                }                
            }
        }
    }

    return ret_val;
}