#include <sys/types.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <time.h>
#include <utime.h>

//mytouch [option] [filename]
//option: -a, -m, -t
int main(int argc, char *argv[]){
    int fd;
    struct stat st;
    struct utimbuf timebuf;
    struct tm t;
    time_t timer;

    if(argc < 2){
        printf("wrong input!: mytouch [option] [filename]\n");
        exit(0);
    }
    if(argv[1][0] != '-'){ //no option
        for(int i = 1; i<=argc; i++){
            fd = open(argv[i], O_RDWR | O_CREAT, 0664);
            close(fd);
        }
    }
    else if(!strcmp(argv[1], "-a")){
        for(int i = 2; i<=argc; i++){
            fd = open(argv[i], O_RDWR | O_CREAT, 0664);
            stat(argv[i], &st);
            timebuf.actime = time(NULL);
            timebuf.modtime = st.st_mtime;
            utime(argv[i], &timebuf); 
            close(fd);
        }
    }
    else if(!strcmp(argv[1], "-m")){
        for(int i = 2; i<=argc; i++){
            fd = open(argv[i], O_RDWR | O_CREAT, 0664);
            stat(argv[i], &st);
            timebuf.actime = st.st_atime;
            timebuf.modtime = time(NULL);
            utime(argv[i], &timebuf); 
            close(fd);
        }
    }    
    exit(0);
}