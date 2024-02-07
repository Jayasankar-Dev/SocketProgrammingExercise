#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <errno.h>
#include <string.h>
#include <netinet/in.h>

int main (void)
{
    struct sockaddr_in server_addr, client_addr;
    int client_addr_size;
    int server_fd, connected_fd;
    int ret, ret_val = 0;
    char client_req[6];
    FILE *directory_fd;
    char file_data[128], *token, name[16];;
    bool address_found;
    int option;
    
    directory_fd = fopen("./directory.txt", "r");
    if (directory_fd == NULL)
    {
        ret_val = -1;
        printf("Failed in opening the directory file %d<%s>", errno, strerror(errno));
    }
    else
    {
        server_fd = socket(AF_INET, SOCK_STREAM, 0);
        if (server_fd < 0)
        {
            ret_val = -1;
            printf("Failed in opening the socket %d<%s>", errno, strerror(errno));
        }
        else
        {
            printf("Sock created \n");
            option = 1;
            setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &option, sizeof(option));

            server_addr.sin_family = AF_INET;
            server_addr.sin_addr.s_addr = INADDR_ANY;
            server_addr.sin_port = htons(6001);

            ret = bind(server_fd, (struct sockaddr *)&server_addr, sizeof(server_addr));
            if (ret < 0)
            {
                ret_val = -1;
                printf("Failed in binding the socket %d<%s>", errno, strerror(errno));
            }
            else
            {
                printf("Sock binded \n");
                listen(server_fd, 5);

                client_addr_size = sizeof(client_addr);

                printf("Sock wait for connection \n");
                connected_fd = accept(server_fd, (struct sockaddr *)&client_addr, &client_addr_size);
                if (connected_fd < 0)
                {
                    ret_val = -1;
                    printf("Failed in accpepting the socket from waiting queue %d<%s>", errno, strerror(errno));
                }
                else
                {
                    printf("Sock found a connection \n");
                    while(1)
                    {
                        memset(client_req, '\0', sizeof(client_req));
                        ret = recv(connected_fd, &client_req, sizeof(client_req), 0);
                        if (ret < 0)
                        {
                            ret_val = -1;
                            printf("Failed in receiving from connected socket %d<%s>", errno, strerror(errno));
                        }
                        else
                        {
                            printf("address to search %s \n", client_req);
                            if (0 == strcmp(client_req, "break"))
                            {
                                ret = send(connected_fd, &client_req, sizeof(client_req), 0);
                                if (ret < 0)
                                {
                                    ret_val = -1;
                                    printf("Failed in receiving from connected socket %d<%s>", errno, strerror(errno));
                                }
                                else
                                {
                                    printf("Sent break to client \n");
                                }

                                break;
                            }
                            fseek(directory_fd, 0, SEEK_SET);                            
                            address_found = false;
                            memset(name, '\0', sizeof(name));
                            while ((0 == feof(directory_fd)) && (false == address_found))
                            {
                                fgets(file_data, sizeof(file_data), directory_fd);

                                token = strtok(file_data, " ");
                                printf("token %s  \n", token);

                                // if (atoi(client_req) == atoi(token))
                                if (0 == strcmp(client_req, token))
                                {
                                    address_found = true;

                                    token = strtok(NULL, " ");

                                    strcpy(name, token);

                                    printf("name %s \n", name);
                                }
                            }

                            if (address_found == false)
                            {
                                strcpy(name, "not Found!");
                            }

                            printf("Responce to client: %s", name);
                                                    
                            ret = send(connected_fd, &name, sizeof(name), 0);
                            if (ret < 0)
                            {
                                ret_val = -1;
                                printf("Failed in receiving from connected socket %d<%s>", errno, strerror(errno));
                            }
                            else
                            {
                                printf("Sent the response to client \n");
                            }
                        }
                    }
                    fclose(directory_fd);
                    close(connected_fd);
                    close(server_fd);
                }
            }
        }
    }

    return 0;
}