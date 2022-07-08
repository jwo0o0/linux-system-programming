#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <pwd.h>
#include <grp.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

char type(mode_t);
char* perm(mode_t);
void printStat(char*, char*, struct stat*);
void printInode(char*, char*, struct stat*);
void sort_by_names(int n);
void sort_by_mtime(int n);
void lsDirectory();
int save_files();

DIR *dp; DIR *dp2;
char *dir; 
struct stat st;
struct dirent *d; 
struct dirent *d2;
char path[1024];
char *pathnames[100]; 
struct dirent *ds[100]; 

//myls
//myls [filename]
//myls [option] [filename]
//option: -i, -l, -t
int main(int argc, char* argv[]){
    //myls
    if(argc == 1){
        dir = ".";
        int n = save_files();
        sort_by_names(n);
        for(int i = 0; i<n; i++){
            printf("%s  ", ds[i]->d_name);
        }
        printf("\n");
    }

    if(argc >= 2){
        //myls [filename]
        if(argv[1][0] != '-'){ //pathname
            for(int i = 1; i<=argc; i++){
                strcpy(path, argv[i]);
                dir = ".";
                if((dp = opendir(dir)) == NULL)
                    perror(dir);
                while((d=readdir(dp)) != NULL){
                    if(strcmp(d->d_name, path) == 0){
                        if(lstat(path, &st)<0)
                            perror(path);
                        struct stat *st2 = &st;
                        if(S_ISDIR(st2->st_mode)){ //directory
                            lsDirectory();
                        }
                        else{ //else
                            printf("%s  ", d->d_name);
                        }      
                    }
                }
            }
            closedir(dp);
            exit(0);
        }
        //myls [option] [filename]
        else{
            if(argv[1][1] == 'l'){ 
                dir = ".";
                int n = save_files();
                //이름순으로 파일정보 정렬
                sort_by_names(n);
                if(argc==2){ //myls -l
                    //출력
                    for(int i = 0; i<n; i++){
                        strcpy(path, (ds[i]->d_name));
                        if(lstat(path, &st) <0)
                            perror(path);
                        printStat(path, (ds[i]->d_name), &st);
                    }
                }
                else{ //myls -l [filename] [filename] ..
                    for(int i = 0; i<n; i++){
                        for(int j = 2; j<=argc; j++){
                            strcpy(path, (ds[i]->d_name));
                            if(!strcmp(path, argv[j])){
                                if(lstat(path, &st) <0)
                                    perror(path);
                                printStat(path, (ds[i]->d_name), &st);
                            }
                        }
                    }
                }
            } 
            //myls -i    
            else if(argv[1][1] == 'i'){ 
                dir = ".";
                int n = save_files();
                //이름순으로 파일정보 정렬
                sort_by_names(n);
                //출력
                for(int i = 0; i<n; i++){
                    strcpy(path, (ds[i]->d_name));
                    if(lstat(path, &st) <0)
                        perror(path);
                    printInode(path, (ds[i]->d_name), &st);
                }
                printf("\n");
            } 
            //myls -t   
            else if(argv[1][1] == 't'){ 
                dir = ".";
                int n = save_files();
                //last modification time 순으로 정렬
                sort_by_mtime(n);
                //출력
                for(int i = 0; i<n; i++){
                    printf("%s  ", ds[i]->d_name);
                }
                printf("\n");
            } 
                
        }
    }

    closedir(dp);
    exit(0);
}

void lsDirectory(){
    if((dp2 = opendir(d->d_name)) == NULL)
        perror(dir);
    int n = 0;
    while((d2 = readdir(dp2)) != NULL){
        if(d2->d_name[0] != '.'){
        ds[n] = d2;
        n++;
        } //파일 저장
    }
    //이름 순 정렬
    sort_by_names(n);
    //출력
    for(int i= 0; i<n; i++){
        printf("%s  ", ds[i]->d_name);
    }
    printf("\n");
}

int save_files(){
    if((dp = opendir(dir)) == NULL)
        perror(dir);
    int n = 0;
    while((d = readdir(dp)) != NULL){ //각 파일정보 저장
        if((d->d_name[0]) != '.'){
			ds[n] = d;
			n++;
		}
    }
    return n;
}

void sort_by_names(int n){
    struct dirent *tmp;
    for(int i = 0; i<n-1; i++){
        for(int j = 0; j<n-i-1; j++){
            if(strcmp((ds[j]->d_name), (ds[j+1]->d_name)) > 0){
                tmp = ds[j];
                ds[j] = ds[j+1];
                ds[j+1] = tmp;
            }
        }
    }
}

void sort_by_mtime(int n){
    struct stat st1, st2;
    struct stat *s1, *s2;
    struct dirent *tmp;
    for(int i = 0; i<n-1; i++){
        for(int j = 0; j<n-i-1; j++){
            lstat(ds[j]->d_name, &st1);
            lstat(ds[j+1]->d_name, &st2);
            s1 = &st1; s2 = &st2;
            if(((s1->st_mtime) - (s2->st_mtime)) < 0){
                tmp = ds[j];
                ds[j] = ds[j+1];
                ds[j+1] = tmp;
            }
        }
    }
}


void printInode(char *pathname, char *file, struct stat *st){
    printf("%ld ", st->st_ino);
    printf("%s  ", file);
}

void printStat(char *pathname, char *file, struct stat *st){
    printf("%c%s ", type(st->st_mode), perm(st->st_mode));
    printf("%3ld ", st->st_nlink);
    printf("%s %s ", getpwuid(st->st_uid)->pw_name, getgrgid(st->st_gid)->gr_name);
    printf("%9ld ", st->st_size);
    printf("%.12s ", ctime(&st->st_mtime)+4);
    printf("%s\n", file);
}

char type(mode_t mode){
    if(S_ISREG(mode)) return ('-');
    if(S_ISDIR(mode)) return ('d');
    if(S_ISCHR(mode)) return ('c');
    if(S_ISBLK(mode)) return ('b');
    if(S_ISLNK(mode)) return ('l');
    if(S_ISFIFO(mode)) return ('p');
    if(S_ISSOCK(mode)) return ('s');
}

char* perm(mode_t mode){
    int i;
    static char perms[10] = "---------";
    for(i = 0; i<3; i++){
        if(mode & (S_IRUSR >> i*3))
            perms[i*3] = 'r';
        if(mode & (S_IWUSR >> i*3))
            perms[i*3+1] = 'w';
        if(mode & (S_IXUSR >> i*3))
            perms[i*3+2] = 'x';
    }
    return(perms);
}

