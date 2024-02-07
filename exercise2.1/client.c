#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <netinet/in.h>


int main (void)
{
    struct sockaddr_in client_addr, server_addr;
    int client_fd;
    int ret, ret_val = 0, person_addr;
    char name[16], request[5];

    client_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (client_fd < 0)
    {
        ret_val = -1;
        printf("Failed in opening client socket %d<%s> \n", errno, strerror(errno));
    }
    else
    {
        server_addr.sin_addr.s_addr = INADDR_ANY;
        server_addr.sin_port = htons(6001);
        server_addr.sin_family = AF_INET;

        ret = connect(client_fd, (struct sockaddr *)&server_addr, sizeof(server_addr));
        if (ret < 0)
        {
            ret_val = -1;
            printf("Failed in connecting to server %d<%s> \n", errno, strerror(errno));
        }
        else
        {
            while (1)
            {
                memset(request, '\0', sizeof(request));
                printf("Enter the house number: ");
                // scanf(" %d", &person_addr);
                gets(request);

                ret = send(client_fd, &request, sizeof(request), 0);
                if (ret < 0)
                {
                    ret_val = -1;
                    printf("Failed in sending data to server %d<%s> \n", errno, strerror(errno));
                }
                else
                {
                    ret = recv(client_fd, &name, sizeof(name), 0);
                    if (ret < 0)
                    {
                        ret_val = -1;
                        printf("Failed in receiving data from server %d<%s> \n", errno, strerror(errno));
                    }
                    else
                    {
                        printf("Person living in address: %s is Mr/Mis/Miss %s \n", request, name);
                        if (0 == strcmp(name, "break"))
                            break;
                    }
                }
            }
            close(client_fd);  
        }
    }

    return ret_val;
}
