#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <pwd.h>
#include <grp.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MODE_1 (S_IXUSR)
#define MODE_2 (S_IWUSR)
#define MODE_3 (S_IWUSR | S_IXUSR)
#define MODE_4 (S_IRUSR)
#define MODE_5 (S_IRUSR | S_IXUSR)
#define MODE_6 (S_IRUSR | S_IWUSR)
#define MODE_7 (S_IRUSR | S_IWUSR | S_IXUSR)

char c_mode[10]; char tmp[2];
char filename[1024];
int u_mode, g_mode, o_mode;
int modes[3];
struct stat st;

//mychmod [mode] [filename]
int main(int argc, char* argv[]){
    if(argc<3){
        printf("wrong input!: mychmod [mode] [filename]\n");
        exit(0);
    }
    else if(argc >3){
        printf("too many input!: mychmod [mode] [filename]\n");
        exit(0);
    }
    else{
        strcpy(c_mode, argv[1]);
        strcpy(filename, argv[2]);

        //search file
        if(lstat(filename, &st)<0){
            fprintf(stderr, "stat error %s\n", filename);
            exit(0);
        }
            
        for(int i = 0; i<3; i++){
            tmp[0] = c_mode[i];
            modes[i] = atoi(tmp);
        }

        for(int i = 0; i<3; i++){
            if((modes[i]<0) || (modes[i]>7)){
                printf("wrong mode\n");
                exit(0);
            }
        }

        chmod(filename, st.st_mode &=~(S_IRWXU | S_IRWXG | S_IRWXO));
        for(int i = 0; i<3; i++){
            if(modes[i] == 1)
                chmod(filename, st.st_mode |= (MODE_1>>i*3));
            else if(modes[i] == 2)
                chmod(filename, st.st_mode |=(MODE_2>>i*3));
            else if(modes[i] == 3)
                chmod(filename, st.st_mode |=(MODE_3>>i*3));
            else if(modes[i] == 4)
                chmod(filename, st.st_mode |=(MODE_4>>i*3));
            else if(modes[i] == 5)
                chmod(filename, st.st_mode |=(MODE_5>>i*3));
            else if(modes[i] == 6)
                chmod(filename, st.st_mode |=(MODE_6>>i*3));
            else if(modes[i] == 7)
                chmod(filename, st.st_mode |=(MODE_7>>i*3));
        }
        printf("complete change mode\n");
    }
}