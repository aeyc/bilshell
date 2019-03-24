#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>   
#include <errno.h>    
#include <sys/wait.h> 
#include <fcntl.h>


int i;
char *word;
char *array[512];
char *firstPipe[512];
char *secondPipe[512];
int pipeBool;
int seperateIndex;

int pipe1[2];
int pipe2[2];

int n;

char *input = NULL;
size_t capline = 0; 

void seperate(char *input){
    i = 0;
    word = strtok(input, "\n ");
    while (word != NULL) {
        array[i++] = word; 
        word = strtok(NULL, "\n ");
    }
    array[i] = NULL;

    /*
     SEPERATING IF PIPE CHAR IS THERE
     */
    pipeBool = 0; //will be used as boolean

    char sample[i];
    int j = 0;

    while( array[j] != NULL){
        sample[j] = *array[j]; //ps aux | ls -> [p a | l]
        if (sample[j] == '|' ){
            pipeBool = 1;
            printf("seperated: true\n");
        }
        j++;
    }

    int k = 0;
    if (pipeBool == 1){
        while( sample[k] != '|'){
            firstPipe[k] = array[k];
            seperateIndex = k;
            k++;
        }
        firstPipe[k] = NULL;

        /*print firstpipe*/
        int f1=0;
        printf("\nfirst command for pipe1: ");
        while ( firstPipe[f1] != NULL ){
            printf(firstPipe[f1]);
            printf(" ");
            f1++;
        }
        printf("\n");
        /**/

        seperateIndex++;
        seperateIndex++;

        int p2 =0;
        while(array[seperateIndex] != NULL){
            secondPipe[p2] = array[seperateIndex];
            seperateIndex++;
            p2++;

        }
        secondPipe[p2] = NULL;

        /*print secondpipe*/
        int f2=0;
        printf("\nsecond command for pipe2: ");
        while ( secondPipe[f2] != NULL ){
            printf(secondPipe[f2]);
            f2++;
        }
        printf("\n");
        /**/
    }
}



int main(int margc, char **margv){
    if ( margc == 2){
        printf("bilshell interactive mode invoked.\n");
        int length = (int)strlen(margv[1]);
        for ( int i = 0; i < length; i ++){
            n = n * 10 + (margv[1][i] - '0');
        }

        printf("\nN IS =%d\n", n);
        while(1){
            printf(" enter a command to execute: \n");
            getline(&input, &capline, stdin);
            seperate(input);

            if (pipeBool == 0) {
                int oneChildPID = fork();
                if(oneChildPID != 0){
                    wait(NULL);
                }
                else{
                    if(execvp(array[0], array) == -1){
                        printf("\nerror in comment\n");
                    }
                }
            }
            if( pipeBool != 0){
                /*
                 OPENNING PIPES ERROR
                 */
                if (pipe(pipe1) < 0){
                    printf("error openning pipe1");
                }
                if (pipe(pipe2) < 0){
                    printf("error openning pipe2");
                }

                /*
                 fork to execute 2 children process
                 */
                int pid1 = fork();
                if (pid1 == 0){
                    dup2(pipe1[1],1);
                    close(pipe1[0]);
                    close(pipe1[1]);
                    close(pipe2[0]);
                    close(pipe2[1]);

                    if(execvp(firstPipe[0], firstPipe) == -1){
                        printf("\nerror in comment\n");

                    }
                }
                else{
                    int pid2 = fork();
                    if(pid2 == 0){
                        dup2(pipe2[0],0);
                        close(pipe1[0]);
                        close(pipe1[1]);
                        close(pipe2[0]);
                        close(pipe2[1]);
                        if(execvp(secondPipe[0], secondPipe) == -1){
                            printf("\nerror in comment\n");
                        }
                    }
                    else{
                        close(pipe1[1]);
                        close(pipe2[0]);
                        int readCount = 0;
                        int charCount =0;

                        char* buf = (char*)malloc(sizeof(int)*(n));
                        while(read(pipe1[0],buf,n)){
                            write(pipe2[1],buf, n);
                            readCount++;
                        }
                        //printf("read-call-count:%d\n", readCount);
                        //wait(NULL);
                        wait(NULL);
                        charCount = n * readCount;
                        printf("character-count: %d\n", charCount);
                        printf("read-call-count: %d\n", readCount);

                        close(pipe1[0]);
                        close(pipe2[1]);
                    }
                }
            }
        }
    }

    else if ( margc >= 3){
        printf("\nbatch mode\n");
        int length = (int)strlen(margv[1]);
        int m =0;
        for ( int i = 0; i < length; i ++){
            m = m * 10 + (margv[1][i] - '0');
        }
        printf("m =============== %d\n", m);

        FILE* file = fopen(margv[2], "r");
        char *line = NULL;
        ssize_t len =0;
        ssize_t read1;

        if( file == NULL ){
            printf("\n file cannot found\n");
        }
        while((read1 = getline(&line,&len, file)) != -1 ){
            seperate(line);
            if (pipeBool == 0) {
                int oneChildPID = fork();
                if(oneChildPID != 0){
                    wait(NULL);
                }
                else{
                    if(execvp(array[0], array) == -1){
                        printf("\nerror in comment\n");
                    }
                }
            }
            if( pipeBool != 0){
                /*
                 OPENNING PIPES ERROR
                 */
		printf("\npipebool != 0 \n");
                if (pipe(pipe1) < 0){
                    printf("error openning pipe1");
                }
                if (pipe(pipe2) < 0){
                    printf("error openning pipe2");
                }

                /*
                 fork to execute 2 children process
                 */
                int pid1 = fork();
                if (pid1 == 0){
		    printf("\nchild1 proccess..\n");
                    dup2(pipe1[1],1);
                    close(pipe1[0]);
                    close(pipe1[1]);
                    close(pipe2[0]);
                    close(pipe2[1]);

                    if(execvp(firstPipe[0], firstPipe) == -1){
                        printf("\nerror in comment\n");

                    }
                }
                else{
                    int pid2 = fork();
                    if(pid2 == 0){
			printf("\nchildprocess2\n");
                        dup2(pipe2[0],0);
                        close(pipe1[0]);
                        close(pipe1[1]);
                        close(pipe2[0]);
                        close(pipe2[1]);
                        if(execvp(secondPipe[0], secondPipe) == -1){
                            printf("\nerror in comment\n");
                        }
                    }
                    else{
                        close(pipe1[1]);
                        close(pipe2[0]);
                        int readCount = 0;
                        int charCount =0;

                        char* buf = (char*)malloc(sizeof(int)*(m));
                        while(read(pipe1[0],buf,m)){
                            write(pipe2[1],buf, m);
                            readCount++;
                        }
                        
                        wait(NULL);
                        charCount = m * readCount;
                        printf("character-count: %d\n", charCount);
                        printf("read-call-count: %d\n", readCount);

                        close(pipe1[0]);
                        close(pipe2[1]);
                    }
                }
            }
        }

    }


    else{
        printf("\n Usage: bilshell N(int) or \n bilshell N(int) inputfile(txt).\n");
    }
}

