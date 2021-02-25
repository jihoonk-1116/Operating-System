/*
CS340 : Operating System
Dr. John Svadlenka
Section 33 : Tue, Thr 5:00 - 6:15
Chihoon Kim
Stu# 23694627

Project 3 made by Chihoon Kim 

Command: 
gcc KChihoon_schd.c -o KChihoon_schd.exe
./KChihoon_schd.exe myInput.txt
*/

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>

#define Q 10    //Quntum Time for Round Robin is defined 10

FILE *fp;

struct Task{        //This structure contains information of thread or process.
    int index;      //Thread or process identification number
    int priority;   
    int burst;
    int static_burst; //To calculate wait time after bursting
    int arrivalTime;  //The time a thread or a process activating.
    int waitTime;     //Wait Time = current_time - arrivalTime - static_burst
};
struct Node{          //To implement linked list, Node including thread as data field 
    struct Task data;
    struct Node *next;
};
//Linked list methods to insert node in the first, the last and the middle of a list
void insertLast(struct Node **ptr, struct Task thread){
    struct Node *node = (struct Node*) malloc(sizeof(struct Node));
    if((*ptr) == NULL){
        node->data = thread;
        *ptr = node;
        return;
    }
    struct Node *current = *ptr;

    node->data = thread;
    node->next = NULL;

    current->next = node;
}
void insertLast_for_readfile(struct Node **ptr, struct Task thread){
    struct Node *node = (struct Node*) malloc(sizeof(struct Node));
    node->data = thread;
    node->next = NULL;
    if((*ptr) == NULL){
        *ptr = node;
        return;
    }
    struct Node *current = *ptr;
    while(current->next !=NULL){
        current = current->next;
    }
    current->next = node;
}
void insertBetween(struct Node **ptr, struct Task thread){
    struct Node *node = (struct Node*) malloc(sizeof(struct Node));
    struct Node *current = *ptr;

    node->data = thread;
    node->next = current->next; // cur ->  t  ->next
    current->next = node;
}
//This program will use insert_waitingList function 
//to keep proper order that the highest priority thread becomes always head node
void insert_WaitingList(struct Node **head, struct Task thread){ //reference head address

    struct Node *ptr_node = *head;
    
    if((*head) == NULL){    //If the list is empty, the inserted thread becomes the first.
        struct Node *node = (struct Node*) malloc(sizeof(struct Node));
        node->data = thread;
        *head = node;
        return;
    }
    if(ptr_node->data.priority > thread.priority){ //If the inserted thread has higher priority than head, 
                                                   //the new thread becomes first.            
        struct Node *node = (struct Node*) malloc(sizeof(struct Node));
        node->data = thread;
        node->next = *head;
        *head = node;
        
    }else{
        while(1){
            if(ptr_node->next == NULL){         //If the pointer node is the last element, the new thread is inserted at the last. 
                insertLast(&ptr_node, thread);
                break;
            }                                   //If the next of the pointer indicates lower priority than and equal the new one , 
                                                //the new thread is inserted in the middle of current and next. 
            if(ptr_node->next->data.priority >= thread.priority){
                insertBetween(&ptr_node, thread);
                break;
            }
            ptr_node = ptr_node->next;          //Update pointer
        }
    }
}
struct Node* pop_List(struct Node **head){      //If the head node that has the highest priorty is return, the head of the list will be the next.

    struct Node *temp = (struct Node*) malloc(sizeof(struct Node));
    struct Node *new_first = *head;

    temp = *head;
    *head = new_first->next;

    return temp;
}
void printList(struct Node *x) { 
  while (x != NULL) { 
     printf(" %d,  %d \n", x->data.index, x->data.priority); 
     x = x->next; 
  } 
}
int get_size(char * filename){      //This function is to know how many threads are coming. 
    int count=0;
    char buff;
    FILE *fp;
    fp = fopen(filename, "r");
    while(!feof(fp)){               //while file pointer does not point null
        buff = fgetc(fp);
        if(buff == '\n'){           //If the read data is a enter command, count ++
        count++;
        }
    }
    fclose(fp);
    return count;
}
struct Node* read_file(char * filename, int size) { //Read file and store the file's data in linked list. 
    FILE *fp;                                       //Then this function returns the head of the list. 
    struct Node *first = NULL;
    struct Task temp;
    char buff[128];
    int index = 0;
    
    fp = fopen(filename, "r");
    fscanf(fp, "%[^\n]", buff);                     //Skip the first line.

     while(fscanf(fp, "%s", buff) != EOF){          //Use Mod 4 because the thread data is consisted of 4 data fields.
        if(index % 4 == 0) temp.index = atoi(buff);
        else if(index % 4 == 1) temp.priority = atoi(buff);
        else if(index % 4 == 2) temp.burst = atoi(buff);
        else {
            temp.arrivalTime = atoi(buff);
            temp.static_burst = temp.burst;         //static_burst will not be changed
            insertLast_for_readfile(&first, temp);  //append read data.
        }
        index++;
    }
    fclose(fp);
    return first;
}
void sort_print(struct Task thr[], int size){
    for(int i =1;i<=size;i++){
        for(int j=0;j<size;j++){
            if(i == thr[j].index){
                fprintf(fp,"\t%d \t\t %d \n", thr[j].index, thr[j].waitTime);
                break;
            }
        }
    }
}
/*
Part1 represents non preemptive Round Robind.
Thus, running process will not be preempted utill its burst is done.
*/
void run_part1(char* filename){
    fprintf(fp, "\n  ---- Run Part1 : Non-preemptive Round Robin ----\n");
    int current_time = 0, size = get_size(filename), count = 0;          //Intialize current time, size and count for using processed thread's array
    struct Node *thread_list = read_file(filename,size), *wait_list=NULL;//Intialize two linked list, thread list to manage arrival threads and wait list to store waiting thread.
    struct Task current_thread, processed_threads[size];                 //Declare current_thread acting like one core processor, and processed thread array to store finished threads.
    current_thread.index = -1;                                           //Intialize index -1 to represent to begin
    fprintf(fp, "\n\tTime\t\tProcess\n");
    while(count <= size){                                                //while the number of input threads is not equal as the number of processed threads, count. 
        if(current_thread.index == -1){                                  //If current thread is not assigned,
            current_thread = pop_List(&thread_list)->data;               //Pop first thread.
            if(current_thread.arrivalTime == thread_list->data.arrivalTime && current_thread.priority > thread_list->data.priority){ //If the next thread arrives at the same time of the current
                insert_WaitingList(&wait_list, current_thread);                                                                      //and the next thread has higher priority than current,
                current_thread = pop_List(&thread_list)->data;                                                                       //insert current thread in the wait list and current thread becomes the next.
            }
        }
        if(current_thread.burst == 0){                                         //There are total 4 cases what have to be covered if the burst of current thread is done.
            if(thread_list != NULL && wait_list != NULL){                      //Case 1 : The both of thread list and wait list have thread(s) to burst 
                if(thread_list->data.arrivalTime > current_time){              //If a new thread did not arrive before current, 
                    current_thread = pop_List(&wait_list)->data;               //The first of the wait list has to become current thread to prevent idle 
                }
                else if(thread_list->data.priority < wait_list->data.priority){ //If a new thread is higher than the waiting thread, 
                    current_thread = pop_List(&thread_list)->data;              //current thread will be the new thread.
                }else{                                                          //If a waiting thread is higher or equal,
                    current_thread = pop_List(&wait_list)->data;                //current thread will be the waiting thread.
                }
            } // End case 1
            else if(thread_list != NULL && wait_list == NULL){                  //Case 2 : Only thread list has thread to burst
                current_thread = pop_List(&thread_list)->data;                  //Automatically, A new thread will be current thread.
                if(current_thread.arrivalTime > current_time){                  //If the new thread did not arrive, the core is going to be idle 
                    while(current_thread.arrivalTime > current_time) {printf("idle %d", current_time); current_time++;}
                }
            } //End case 2
            else if(thread_list == NULL && wait_list != NULL){                  //Case 3 :  Only wait list has thread(s) to burst
                current_thread = pop_List(&wait_list)->data;                    //Automatically, a waiting thread will be current thread.
            } //End case 3
            else break; //End case 4                                                //Case 4 : Both of the lists are empty. Then exit the while loop
        }//End select next thread
        for(int i=0;i<Q;i++){                                                   //Burst as the quantum time = 10
            if(i==0){                                                           //Print start bursting notification with current time
                fprintf(fp,"\t%d \t\t %d \n", current_time, current_thread.index);
            }
            current_thread.burst--;                                             //Processing
            current_time++;                                                     //Increment of current time
            if(thread_list != NULL && thread_list->data.arrivalTime == current_time){  //If a new thread arrives, insert the thread in the wait list.
                //printf("\t\t\t ->Detect new thread %d  Time %d\n", thread_list->data.index, current_time);
                insert_WaitingList(&wait_list, pop_List(&thread_list)->data);
            }
            if(current_thread.burst == 0){                                      //If a thread is processed, calculate its waiting time, and assign the thread in the processed threads.
                current_thread.waitTime = current_time - current_thread.arrivalTime - current_thread.static_burst;
                //printf("\t\t\t ->Finish processing %d, Time %d,  Wait: %d \n", current_thread.index, current_time, current_thread.waitTime);
                processed_threads[count++] = current_thread;
                break;                                                          //Exit when a thread is finished processing.
            }
        }
    }
    fprintf(fp, "\t%d  \t\t %d   Finish processing\n", current_time, current_thread.index);
    fprintf(fp, "\n\tProcess \tWait Time\n");
    sort_print(processed_threads, count);
}

void run_part2(char *filename){
    fprintf(fp, "\n  ---- Run Part2 : Preemptive Round Robin ----\n");
    int current_time =0, size = get_size(filename), count = 0, burst =0;
    struct Node *thread_list=read_file(filename, size), *wait_list = NULL;
    struct Task current_thread, processed_threads[size];
    current_thread.index = -1;//Initialize
    fprintf(fp, "\n\tTime\t\tProcess\n");
    while(count <= size){
        //Begin
        if(current_thread.index == -1){
            current_thread = pop_List(&thread_list)->data;
            if(current_thread.priority > thread_list->data.priority){
                insert_WaitingList(&wait_list, current_thread);
                current_thread = pop_List(&thread_list)->data;
            }
        } //End begin
        /*
        There are two total cases what have to be covered if the burst of current thread is not done in Part2 because this part is implementing preemptive Round Robin.
        */
        if(current_thread.burst != 0){
            if(wait_list != NULL && wait_list->data.priority <= current_thread.priority){ //Case1 : If the head of waitlist points a task that has higher or equal priority than the current thread,   
                struct Task swap = current_thread;                                        //Swap the current with the head of waitlist because of the property of preemptive Round Robin scheduling.
                current_thread = pop_List(&wait_list)->data;                              //Pop the head of waitlist and insert the previous thread in waitlist.
                insert_WaitingList(&wait_list, swap);
            }                                                                             //Case2 : If waitList is empty or the head of waitlist points a task that has lower priority than the current thread,
        }                                                                                 //continue current thread to burst. 
        /*
        There are total 4 cases what have to be covered if the burst of current thread is done.
        The cases are same as the part1's cases 
        */
        else if(current_thread.burst == 0){
            if(thread_list != NULL && wait_list != NULL){ //case2-1
                if(thread_list->data.arrivalTime > current_time){ //prevent idle
                    current_thread = pop_List(&wait_list)->data;
                }
                else if(thread_list->data.priority < wait_list->data.priority){ //thread high
                    current_thread = pop_List(&thread_list)->data;
                }else{ //head of wait high or equal
                    current_thread = pop_List(&wait_list)->data;
                }
            } // End case 2-1
            else if(thread_list != NULL && wait_list == NULL){ //case 2-2
                current_thread = pop_List(&thread_list)->data;
                if(current_thread.arrivalTime > current_time){
                    while(current_thread.arrivalTime > current_time) {printf("idle %d", current_time); current_time++;}
                }
            } //End case 2-2
            else if(thread_list == NULL && wait_list != NULL){ //case 2-3
                current_thread = pop_List(&wait_list)->data;
            } //End case 2-3
            else break; //case 2-4
        } //End select thread to burst
        for(int i=0;i<Q;i++){
            if(i == 0){
                fprintf(fp,"\t%d \t\t %d \n", current_time, current_thread.index);
            }
            current_thread.burst--;
            current_time++;
            if(thread_list != NULL && thread_list->data.arrivalTime == current_time){
                //printf("\t\t\t ->Detect new thread %d  Time %d\n", thread_list->data.index, current_time);
                insert_WaitingList(&wait_list, pop_List(&thread_list)->data);
            }
            if(current_thread.burst == 0){
                current_thread.waitTime = current_time - current_thread.arrivalTime - current_thread.static_burst;
                //printf("\t\t\t ->Finish processing %d, Time %d,  Wait: %d \n", current_thread.index, current_time, current_thread.waitTime);
                processed_threads[count++] = current_thread;
                break;
            }
        }
    }
    fprintf(fp, "\t%d  \t\t %d   Finish processing\n", current_time, current_thread.index);
    fprintf(fp, "\n\tProcess \tWait Time\n");
    sort_print(processed_threads, count);
}

int main(int argc, char *argv[]){
    if(argc != 2){
        printf("Wrong number of command line arguments\n");
        printf("[command] [Filename]");
        exit(1);
        return 1;
    }

    fp = fopen("KChihoon_rslt.txt", "w");

    fprintf(fp, "\nChihoon Kim Project3\tSt: 23694627\n");
    
    run_part1(argv[1]);
    run_part2(argv[1]);

    fprintf(fp,"\n\tEnd. \n");

    fclose(fp);

    return 0;
}