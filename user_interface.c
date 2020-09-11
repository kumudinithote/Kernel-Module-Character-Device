#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>


#define BUFFER_LENGTH 512

int main(){

    int fd, bytes_read;
    char receive_buffer[BUFFER_LENGTH];

    fd = open("/dev/process_list", O_RDONLY);
    if(fd < 0) {
        printf("Failed to open mynull.\n");
        return -1;
    }

    printf("character device process_list has successfully been opened.\n");
    printf("Reading from process_list...\n");

    while((bytes_read = read(fd,receive_buffer,strlen(receive_buffer)))  > 0)
    {
    		printf("%s\n",receive_buffer);
    		memset(receive_buffer,0,sizeof(char)*BUFFER_LENGTH);
    }
    printf("Reading done from process_list...\n");

    if(bytes_read < 0)
    {
		    perror("ERROR while reading from device character device process_list");
		    return 0;
    }

    close(fd);
    return 0;
}
