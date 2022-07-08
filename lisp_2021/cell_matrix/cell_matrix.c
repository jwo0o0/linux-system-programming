#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <time.h>
#define max 10001

FILE *input;
char *filename = "input.matrix";
int M, N;
char matrix[max][max];
int **next_gen; //다음 세대에 살지 말지가 저장
int **live_neighbors; //이웃한 산 세포 저장
int *cnt_child_process; //process 병렬처리에서 처리할 세포 개수
int choice = 0; //어떤 동작을 할 것인지 결정
int Gen; //반복할 세대 개수
clock_t start, end; //시작시간, 종료시간
float ex_time; //총 수행시간

pid_t *child_ids; //process 병렬처리에서 child process들의 id
int fdes; //gen_n.matrix, output.matrix 파일 생성을 위한 디스크립터
int child_process; //process 병렬처리에서 자식 프로세스의 개수

void input_matrix();
void apply_rule(int start, int end); 
void count_live_neighbor(); 
void check_next_gen(int row, int col);
void make_gen_file(int gen);

int main(){
    //반복 실행
    printf("*Choice(1(exit), 2, 3, 4):");
    scanf("%d", &choice);
    while(choice != 1){
        if(choice == 2){ //순차 처리
            input_matrix();
            printf("*Input(generation):");
            scanf("%d", &Gen);
            for(int cnt = 1; cnt <= Gen; cnt++){ //세대별로 반복
                count_live_neighbor(); //살아있는 이웃세포 계산
                apply_rule(1, M*N); //규칙적용해 matrix[][] 다음세대로
                make_gen_file(cnt); //파일 생성해주기
            }
            make_gen_file(-1); //output.matrix 생성
            //종료 시간 측정
            end = clock();
            ex_time = (float)(end-start);
            printf("*수행 시간(ms):%lf\n\n", ex_time);

            free(live_neighbors);
            free(next_gen);
        }
        else if(choice == 3){ //process 병렬처리
            input_matrix();

            printf("*Input(generation):");
            scanf("%d", &Gen);
            printf("*Input(number of Child Process):");
            scanf("%d", &child_process);

            printf("*process id:\n");
            //각 child process가 처리 할 세포 개수 저장
            cnt_child_process = malloc(sizeof(int)*(child_process+1));
            int cnt_1 = (M*N)/child_process;
            int cnt_2 = (M*N)%child_process;
            for(int i = 1; i<=child_process; i++)
                cnt_child_process[i] = cnt_1;
            for(int i = 1; i<=cnt_2; i++)
                cnt_child_process[i]++;
            
            //child process 생성하며 처리
            child_ids = malloc(sizeof(pid_t)*(child_process+1));
            int status, pid;
            for(int cnt = 1; cnt<= Gen;cnt++){ //세대마다 반복
                count_live_neighbor();
                int start_cel_num = 0;
                for(int p = 1; p<=child_process; p++){
                    start_cel_num+=1;
                    child_ids[p] = vfork();
                    if(child_ids[p] == 0){ //child process
                        apply_rule(start_cel_num, start_cel_num+(cnt_child_process[p]-1));
                        _exit(1);
                    }
                    else{ //parent process
                        pid = wait(&status);
                        printf("%d ", pid);
                    }
                    start_cel_num+=(cnt_child_process[p]-1); 
                }
                make_gen_file(cnt);
                printf("\n");
            }
            make_gen_file(-1); //output.matrix 생성
            //종료 시간 측정
            end = clock();
            ex_time = (float)(end-start);
            printf("*수행 시간(ms):%lf\n\n", ex_time);

            free(live_neighbors);
            free(next_gen);
            free(cnt_child_process);
            free(child_ids);

        }
        else if(choice == 4){ //thread 병렬처리
            printf("thread 병렬처리\n");
        }
        printf("*Choice(1(exit), 2, 3, 4):");
        scanf("%d", &choice);
    }
    
    fclose(input);
    exit(0);
}

void input_matrix(){
    //시작 시간 측정
    start = clock();
    //파일 입력
    if((input = fopen(filename, "r")) == NULL){ 
        fprintf(stderr, "File open error for %s\n", filename);
        exit(1);
    }
    //input.matrix 읽어서 matrix[][]에 저장
    int i = 1, j = 1;
    char c;
    while(!feof(input)){
        c = fgetc(input);
        if(c == '0' || c == '1'){
            matrix[i][j] = c;
            j++;
        }
        else if(c == '\n'){
            i++;
            N = j -1;
            j = 1;
        }
        M = i;
    }
    //matrix를 0으로 둘러싸주기
    for(int j = 0; j<=N+1; j++){
        matrix[0][j] = '0';
        matrix[M+1][j] = '0';
    }
    for(int i = 0; i<=M+1; i++){
        matrix[i][0] = '0';
        matrix[i][N+1] = '0';
    }
    //int live_neighbors[][] 할당
    live_neighbors = malloc(sizeof(int*)*(M+1));
    for(int i = 1; i<=M; i++){
        live_neighbors[i] = malloc(sizeof(int)*(N+1));
    }
    //int next_gen[][] 할당
    next_gen = malloc(sizeof(int*)*(M+1));
    for(int i = 1; i<=M; i++){
        next_gen[i] = malloc(sizeof(int)*(N+1));
    }

}

void count_live_neighbor(){
    //int live_neighbors[][]에 살아있는 이웃세포 개수 저장
    int tmp_live = 0;
    for(int i = 1; i<=M; i++){
        for(int j = 1; j<=N; j++){
            //살아있는 이웃세포 계산
            for(int m = i-1; m<=i+1; m++){
                for(int n = j-1; n<=j+1; n++){
                    if(matrix[m][n] == '1')
                        tmp_live++;
                }
            }
            live_neighbors[i][j] = tmp_live;
            tmp_live = 0;
        }
    }
    //자기자신이 1인경우는 -1을 해준다
    for(int i = 1; i<=M; i++){
        for(int j = 1; j<=N; j++){
            if(matrix[i][j] == '1')
                live_neighbors[i][j] -= 1;
        }
    }
    return;
}

void check_next_gen(int row, int col){ //다음 세대에 세포의 상태를 int next_gen[][]에 저장
    if(matrix[row][col] == '1'){ //현재 세포가 살아있으면
        if((live_neighbors[row][col]<=2)||(live_neighbors[row][col]>=7))
            next_gen[row][col] = 0;
        else
            next_gen[row][col] = 1;
    }
    else{ //현재 세포가 죽어있으면
        if(live_neighbors[row][col] == 4)
            next_gen[row][col] = 1;
        else
            next_gen[row][col] = 0;
    }
    return;
}

void apply_rule(int start, int end){
    //start와 end의 행과 열 계산
    int s_row, s_col, e_row, e_col;
    if((start%N) == 0) s_row = start/N;
    else s_row = (start/N) + 1;
    s_col = start - ((s_row-1)*N);
    if((end%N) == 0) e_row = end/N;
    else e_row = (end/N) + 1;
    e_col = end - ((e_row-1)*N);

    for(int i = s_row; i<=e_row; i++){
        int j;
        int j_end;
        if(i==s_row){
            j = s_col, j_end = N;
        }
        else if(i==e_row){
            j = 1; j_end = e_col;
        }   
        else{
            j = 1; j_end = N;
        }    
        for(;j<=j_end; j++){
            check_next_gen(i, j);
                if(next_gen[i][j] == 1)
                    matrix[i][j] = '1';
                else
                    matrix[i][j] = '0';
        }
    }
    return;
}

void make_gen_file(int gen){ //gen_n.matrix, output.matrix 파일 생성

    char new_filename[40] = "gen_";
    char gen_filename[40];
    
    sprintf(gen_filename, "%d", gen);
    strcat(new_filename, gen_filename);
    strcat(new_filename, ".matrix");

    char *space = " "; char *newline = "\n"; char mat[2];
    
    if(gen == -1){ //output.matrix 파일 생성
        char *final_filename = "output.matrix";
        fdes = open(final_filename, O_RDWR | O_CREAT | O_TRUNC, 0644);
    }
    else{
        fdes = open(new_filename, O_RDWR | O_CREAT | O_TRUNC, 0644);
    }
    for(int i = 1; i<=M; i++){
        for(int j = 1; j<=N; j++){
            char *tmp = malloc(sizeof(char)*3);
            mat[0] = matrix[i][j];
            strcat(tmp, mat);
            strcat(tmp, space);
            write(fdes, tmp, strlen(tmp));
            free(tmp);
        }
        write(fdes, newline, strlen(newline));
    }
    
    close(fdes);
    return;
}

