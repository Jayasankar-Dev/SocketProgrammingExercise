#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <netinet/in.h>

#define COPY_EXTENTION(file_name)   copy_##file_name

int main(void)
{
    struct sockaddr_in client_addr, server_add;
    int server_fd, client_fd;
    int ret, ret_val = 0;
    char buffer[256], file_name[32];
    FILE *copy_file = NULL;
    
    client_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (client_fd < 0)
    {
        ret_val = -1;
        printf("failed in creating socket %d<%s>\n", errno, strerror(errno));
    }
    else
    {
        server_add.sin_family = AF_INET;
        server_add.sin_port = htons(5001);
        server_add.sin_addr.s_addr = INADDR_ANY;

        ret = connect(client_fd, (struct sockaddr *)&server_add, sizeof(server_add));
        if (ret < 0)
        {
            ret_val = -1;
            printf("failed in connectng to server %d<%s>\n", errno, strerror(errno));
        }
        else
        {
            while (1)
            {
                printf("enter file name to copy from server: ");
                gets(file_name);

                ret = send(client_fd, file_name, sizeof(file_name), 0);
                if (ret < 0)
                {
                    ret_val = -1;
                    printf("failed in sending req to server %d<%s>\n", errno, strerror(errno));
                }
                else
                {
                    ret = recv(client_fd, buffer, sizeof(buffer), 0);
                    if (ret < 0)
                    {
                        ret_val = -1;
                        printf("failed in receive response from server %d<%s>\n", errno, strerror(errno));
                    }
                    else
                    {
                        printf("response from server %s\n", buffer);
                        if (0 != strcmp(buffer, "cannot open file\n"))
                        {

                            copy_file = fopen(file_name, "w");
                            if (copy_file == NULL)
                            {
                                ret_val = -1;
                                printf("failed to create a file %d<%s>\n", errno, strerror(errno));
                            }
                            else
                            {
                                fwrite(buffer, strlen(buffer), 1, copy_file);
                            }
                            fclose(copy_file);
                            copy_file = NULL;
                        }
                    }
                }
            }
        }
    }

    return ret_val;
}