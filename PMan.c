#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <signal.h>
#include <stdbool.h>
#include <readline/readline.h>
#include <readline/history.h>

/*
Jordan steele
v0073362
*/

typedef struct proc{
    struct proc* next;
    struct proc* prev;
    pid_t pid;
    char* cmd;
} proc;

typedef struct{    
    proc* head;
    proc* tail;
} proc_List;

int MAX_INPUT = 100;
int NUM_PROCESS;
int BGJOBS;
char userInput[100];
char parsedInput[100];
char *exec_args[100];

void initList(proc_List* L) {
    L->head = NULL;
    L->tail = NULL;
}

void addProc(proc_List* L ,pid_t newPid, char* command) {
    //create new process
    struct proc *newProc = (struct proc*)malloc(sizeof(struct proc));
    newProc->cmd = malloc(strlen(command) + 1);
    newProc->pid = malloc(sizeof(&newPid) + 1);   

    newProc->pid = newPid;
    strcpy(newProc->cmd, command);

    //add process to bglist
    if(L->head == NULL) {
        L->head = newProc;
        newProc->next = NULL;
        newProc->prev = NULL;
        L->tail = newProc;
    }
    else {
        L->tail->next = newProc;
        newProc->prev = L->tail;
        L->tail = newProc;
    }
    BGJOBS++;
}
//linked list
void remove_Proc(proc_List* L, proc* process) {
    //if pid is in the head proc and list is empty
    //if process is head and only thing in list
    if (L->head == process && L->tail == process) {
        L->head = NULL;
        L->tail = NULL;
        process->next = NULL;
        process->prev = NULL;
        BGJOBS--;
        free(process);
        return;
    }

    //if process is head
    if(process == L->head) {
        L->head = L->head->next;
        L->head->prev = NULL;
        BGJOBS--;
        free(process);
        return;
    }
    //pid is tail
    if(process == L->tail) {
        L->tail = L->tail->prev;
        L->tail->next = NULL;
        BGJOBS--;
        free(process);
        return;
    }
    process->prev->next = process->next;
    process->next->prev = process->prev;
    process->next = NULL;
    process->prev = NULL;
    BGJOBS--;
    free(process);
    return;
    //free the proc
}

void printList(proc_List* L) {
    proc* current = L->head;

    if(current == NULL) {
        printf("No Processes Running\n");
        return;
    }
    while(current != NULL) {
        printf("%d:   %s\n",current->pid, current->cmd);
        current = current->next;
    }
    printf("Total Background Jobs: %d\n\n", BGJOBS);
}

void update_bg_procss(proc_List* L) {

    pid_t id;
    int status;
    setbuf(stdout, NULL);

    //use while loop to monitor all process with -1 option.
    while(1) {
        // to monitor the state of all child process
        //id = waitpid(-1, &status, WNOHANG | WUNTRACED |WCONTINUED);
        id = waitpid(-1, &status, WNOHANG);

        //none of children changed state, no proc_list update needed. break while loop.
        if(id == 0) {  
            break;
        }
        //Inform user that process [id] terminated. remove the item where pid = id in proc_list.
        if(id > 0) {
            proc* curr = L->head;    
            while(curr != NULL) {
                if(curr->pid == id) {
                    remove_Proc(L,curr);

                    /*
                    if(WIFSIGNALED(status)) {
                        remove_Proc(L, curr);
                        printf("Process %d has been terminated\n",id);
                    }
                    else if(WIFSTOPPED(status)) {
                        printf("Process %d has been stopped\n", id);
                    }
                    else if(WIFCONTINUED(status)) {
                        printf("Process %d has been started\n", id);
                    }
                    */

                }
                curr = curr->next;
            }

        }  
        // no more updates
        if (id < 0) {
            break;
        }
    }
    return ;
}


int main(int argc, char* argv[]) {

    proc_List L;
    initList(&L);
    pid_t pid;

        while(1) {
                char *token_commands[100];

                fflush(stdout);
                //update_bg_procss(&L);
                setbuf(stdout, NULL);

                //prompt user for input
                printf("PMan: >");
                fgets(userInput, MAX_INPUT, stdin);
                if(strcmp(userInput, "\n") != 0)
                        userInput[strcspn(userInput, "\n")] = '\0';

                //make a copy to keep original string
                char *myCopy = malloc(strlen(userInput) + 1);
                strcpy(myCopy,userInput);

                //tokenized everything from the start
                char  *temp;
                temp = strtok( myCopy," ");
                int i = 0;  
                while(temp != NULL){
                    token_commands[i] = malloc(strlen(temp) + 1);
                    strncpy(token_commands[i], temp, strlen(temp)+1);
                    temp = strtok(NULL, " ");
                    i++;                    
                }

                const char s[2] = " ";
                //check for exit
                if(strcmp(userInput,"exit") == 0) {
                        exit(1);
                }       
                //make a copy to keep original string
                char *userCopy = malloc(strlen(userInput) + 1);
                strcpy(userCopy,userInput);

                char *cmd = malloc(strlen(userInput) + 1);
                strcpy(cmd,userInput);

                //check if string begins with bg, if so it's a command
                //check for system commands bgkill / bgstart / bgstop
                char *token;
                token = strtok(userInput, s);
                if(strcmp(token_commands[0], "bgkill") == 0) {
                        token = strtok(NULL, " ");
                        int p = atoi(token);
                        kill(p ,SIGKILL);
                }
                if(strcmp(token_commands[0], "bgstart") == 0) {
                        token = strtok(NULL, " ");
                        int p = atoi(token);
                        kill(p ,SIGCONT);
                }
                if(strcmp(token_commands[0], "bgstop") == 0) {
                        token = strtok(NULL, " ");
                        int p = atoi(token);
                        kill(p ,SIGSTOP);
                }

                else if(strcmp(token_commands[0], "bg") == 0) {
                        //get command
                        char *curr;
                        curr = strtok(userCopy, " ");
                        curr = strtok(NULL, " ");
                        char *exec = malloc(strlen(curr)+1);
                        strcpy(exec, curr);

                        //getting arguments
                        int i = 0; 
                        while(curr != NULL) {
                            exec_args[i] = malloc(strlen(curr) + 1);
                            strncpy(exec_args[i], curr, strlen(curr)+1);
                            curr = strtok(NULL, " ");
                            i++;
                        }
                        //add NULL terminator to end of array for execvp
                        //exec_args[i] = malloc(sizeof(NULL)+1);
                        exec_args[i] = NULL;
                        pid = fork();
                        //Child
                        if(pid == 0) {
                            printf("...\n");
                            if (execvp(exec_args[0], exec_args) < 0) {
                                perror("Error on execvp");
                                exit(-1);
                            }
                        } 
                        if(pid > 0){
                            //parent process                            
                            char buf[1000];
                            getcwd(buf, 256);

                            //get path of command to store
                            if(exec_args[0][0] == '.') {
                                strcat(buf, exec_args[0]+1);
                            } else {
                                strcat(buf, "/");
                                strcat(buf, exec_args[0]);
                            }

                            addProc(&L, pid, buf);
                            printf("\nThe pid of the child process is: %d\n",pid);
                            
                            //waitpid(-1, NULL, WNOHANG);
                            //create new pcb
                        }    
                }

                //Pstat
                else if(strcmp(token_commands[0], "pstat") == 0) {

                    //check if pid in list
                    bool inList = false;
                    proc *curr = L.head;    
                    while(curr != NULL) {
                        if(curr->pid == atoi(token_commands[1]))
                            inList = true;
                        curr = curr->next;
                    }

                    if(inList == true) {

                        //variables for pstat
                        char command[1000];
                        char state;
                        int utime;
                        int stime;
                        int rss;
                        char voluntary_switches[1000];
                        char nonvoluntary[1000];

                        //get path for stat file
                        pid_t pid;
                        char destination[100] = "/proc/";
                        strcat(destination, token_commands[1]);
                        strcat(destination, "/stat");
                        //sprintf(destination "/proc/%d/stat", token[1]);

                        //get path for status file
                        char destination2[100] = "/proc/";
                        strcat(destination2, token_commands[1]);
                        strcat(destination2, "/status");

                        //printf("%s\n", destination);
                        FILE *fp = fopen(destination, "r");

                        //get voluntary/nonvoluntary from status file
                        char *line = NULL;
                        size_t length = 0;
                        ssize_t read;
                        FILE *fp2;
                        fp2 = fopen(destination2, "r");

                        while((read = getline(&line, &length, fp2)) != -1) {
                            if(strncmp(line, "voluntary_ctxt_switches:", 24) == 0)
                                strcpy(voluntary_switches, line);
                            if(strncmp(line, "nonvoluntary_ctxt_switches:", 27) == 0) 
                                strcpy(nonvoluntary, line);
                        }

                        //get variables from stat file
                        fscanf(fp, "%*d %s %c %*d %*d %*d %*d %*d %*d %*d %*d %*d %*d %d %*d %d %*d %*d %*d %*d %*d %*d %d", command, &state, &utime, &stime, &rss);
                        printf("Command: %s\n", command);
                        printf("State:   %c\n", state);
                        printf("utime:   %d\n", utime);
                        printf("stime:   %d\n", stime);
                        printf("rss:     %d\n", rss);
                        printf("%s", voluntary_switches);
                        printf("%s", nonvoluntary);

                        fclose(fp);
                        fclose(fp2);
                    }
                    else {
                        //pid is not in linked list
                        printf("Invalid PID \n");
                    }
                }

                //bglist lists all the processes in proc_List
                else if(strcmp(userCopy, "bglist") == 0) {
                    printList(&L);
                }

                //handle exceptions
                else {
                    //printf("Invalid Command\n");
                }       
                update_bg_procss(&L);           
                
                
        }
        return 1;
}
