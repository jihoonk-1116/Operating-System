/*
CS340 : Operating System
Dr. John Svadlenka
Section 33 : Tue, Thr 5:00 - 6:15
Chihoon Kim
Stu# 23694627

Project 4 made by Chihoon Kim

Command: 
gcc KChihoon_mem.c -o KChihoon_mem.exe
./KChihoon_mem.exe 1048576
*/
/*
This program implments the process of allocation in memory 
using worst-fit mode and linked list as its data structure.
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

FILE *fp;

struct Process{  //Process to allocate
    int index;   //Process Identification
    int range;   
    int begin;   //Begin address of a process 
    int end;     //End address of a process
    int size;    //Process size
};
struct Node{    
    struct Process data;    
    struct Node *next;
};
void insertLast(struct Node **ptr, struct Process p){
    struct Node *node = (struct Node*) malloc(sizeof(struct Node));
    if((*ptr) == NULL){
        node->data = p;
        *ptr = node;
        return;
    }
    struct Node *current = *ptr;

    node->data = p;
    node->next = NULL;

    current->next = node;
}
void insertBetween(struct Node **ptr, struct Process p){
    struct Node *node = (struct Node*) malloc(sizeof(struct Node));
    struct Node *current = *ptr;

    node->data = p;
    node->next = current->next; // cur ->  new  -> next
    current->next = node;
}
void allocate(struct Node **head, struct Process p){ //reference head address
                                                     //This function allocates a process in memory order by its begin address.
    struct Node *ptr_node = *head;
    
    if((*head) == NULL){    //If the list is empty, the inserted process becomes the first.
        struct Node *node = (struct Node*) malloc(sizeof(struct Node));
        node->data = p;
        *head = node;
        return;
    }
    if(ptr_node->data.begin > p.begin){ //If the inserted process has a faster address than first process, 
                                                   //the new thread becomes first.            
        struct Node *node = (struct Node*) malloc(sizeof(struct Node));
        node->data = p;
        node->next = *head;
        *head = node;
        
    }else{
        while(1){
            if(ptr_node->next == NULL){         //If the pointer node is the last element, the new process is inserted at the last. 
                insertLast(&ptr_node, p);
                break;
            }                                   //If the next of the pointer indicates a smaller address than the new one , 
                                                //the new thread is inserted in the middle of current and next. 
            if(ptr_node->next->data.begin > p.begin){
                insertBetween(&ptr_node, p);
                break;
            }
            ptr_node = ptr_node->next;          //Update pointer
        }
    }
}
void release_Process(struct Node **head, int index){  
    struct Node *prev_node = NULL;
    struct Node *ptr_node = *head;
    struct Node *temp;
    if(ptr_node->data.index == index){      //If the process is places at the first,
        *head = ptr_node->next;             //Update new head
        free(ptr_node);                     //Deallocate current node
        return;
    }
    else{
        while(ptr_node->next != NULL){
            prev_node = ptr_node;           //Update previous node for preparing the case that the process is in the middle.
            ptr_node = ptr_node->next;      //Update current node.
            if(ptr_node->data.index == index){
                prev_node->next = ptr_node->next; 
                free(ptr_node);
                return;
            }
        }
    }
    printf("There is no %d Process. \n", index);
}
void printList(struct Node *p, int mem_size) { 
    if(p==NULL){                                //The list is NULL if memory is empty.
        printf("Addresses [0 : End] Free ");
        fprintf(fp, "Addresses [0 : End] Free ");
        return;
    }
    if(p->data.begin != 0){                                    //If the first data is not begun at Zero, then that means that Free space exists. 
        printf(" Addresses [0 : %d] Free\n", p->data.begin-1); //The range must be minus one for preventing to overlap ranges. 
        fprintf(fp, "Addresses [0 : %d] Free\n", p->data.begin-1);
    }

    struct Node *prev_node = NULL;

    while (p != NULL) { 
        if(prev_node != NULL && p->data.begin - prev_node->data.end > 1){ //The condition that prev_node != NULL is for skipping the first cycle.  
            printf(" Addresses [%d : %d] Free\n", prev_node->data.end +1, p->data.begin -1); //And if there is a gap between processes, the gap will become Free space.
            fprintf(fp,"Addresses [%d : %d] Free\n", prev_node->data.end +1, p->data.begin -1);
            
        }
        printf(" Addresses [%d : %d]  Process P%d \n", p->data.begin, p->data.end, p->data.index);  //Print current process.
        fprintf(fp,"Addresses [%d : %d]  Process P%d \n", p->data.begin, p->data.end, p->data.index);
        prev_node = p;  //Update previous node
        p = p->next;    //Update current node
        if(p == NULL && prev_node->data.end != mem_size){   //If the node is the last process and its end address is not equal to total memory size,  
            printf(" Addresses [%d - End] Free\n", prev_node->data.end + 1);   //it means that there is Free space. 
            fprintf(fp,"Addresses [%d - End] Free\n", prev_node->data.end + 1);
        }
    } 
}
/*
This program uses Worst-fit mode that only uses the biggest free space 
*/
struct Process find_biggest_hole(struct Node **list, int mem_size){//Find biggest hole to store a new process
    struct Process hole;                                           //Initialize new hole
    struct Node *current_node = *list, *prev_node = NULL;          //Initialzie node pointers
    if(current_node == NULL){                                      //If there is no process, Free size will be total memory size.
        hole.begin =0;
        hole.end = mem_size;
        return hole;                                               //Return biggest hole
    }
    if(current_node->data.begin == 0){                             //If a process's address begins at 0, set hole variable to zero.
        hole.begin = 0;
        hole.end = 0;
        hole.range = 0; 
    }else{                                                         //If not, there is free space.
        hole.begin = 0;
        hole.end = current_node->data.begin -1;                    //hole size = a process begin's address - 0
        hole.range = hole.end - hole.begin;                         
    }
    if(current_node->next == NULL){                                //If the next process does not exist, 
        if(mem_size - current_node->data.end > hole.range){        //there is free space from end address to total memory size.
            hole.begin = current_node->data.end + 1;
            hole.end = mem_size;
        }
        return hole;                                               //Return hole since there is no more process.
    }
    prev_node = current_node;
    current_node = current_node->next;
    while(current_node != NULL){
        if(current_node->data.begin - prev_node->data.end > hole.range){  //if the free space between current process and its previous is larger than current hole,
            hole.begin = prev_node->data.end + 1;                         //Update biggest hole.
            hole.end = current_node->data.begin - 1;                      //Free size = current process's  begin - previous process's end.
            hole.range = hole.end - hole.begin;
        }
        if(current_node->next == NULL && mem_size - current_node->data.end > hole.range){
            hole.begin = current_node->data.end + 1;                      //If the current process is the last process and the free space from current's end to total memory size
            hole.end = mem_size;                                          //is lager than current hole, update biggest hole.
            hole.range = hole.end - hole.begin;
        }
        current_node = current_node->next;                              //Update node pointers.
        prev_node = prev_node->next;
    }
    return hole;                                                   //Return the last updated hole.
}
void command_Compact(struct Node **list,int mem_size){             //Address Compaction function
    struct Node *ptr = *list;                                      //Points Head
    int begin = 0;
    while(ptr != NULL){
        ptr->data.begin = begin;                                   //Current's begin address is updated with new begin.
        ptr->data.end = begin + ptr->data.size;                    //Current's end address is updated as new begin plus its size.
        begin += ptr->data.size + 1;                               //Address update as data's size.
        ptr = ptr->next;
    }
}
void command_Request(struct Node **list, int mem_size, int index, int size, char mode){

    struct Process hole = find_biggest_hole(list, mem_size);    //Find biggest hole to store a new process

    if(hole.end - hole.begin < size){                           //Compare the hole size is fit to the process size
        printf("The process can not be allocated in memory because there is no more storage\n");
        printf("Please release another process to allocate the process \n");
        return;
    }

    hole.index = index;         //The hole is assigned a new process
    hole.size = size;           //Set size
    hole.end = hole.begin + size; // Set end address 

    allocate(list, hole);       //Call allocate function to append to the linked list, the list allocate the new process order by its begin address.

} 
void run_project4(int memory_size){
    struct Node *process_list=NULL;
    int buffer = 128;
    char user_input[buffer], *token;
    const char space[1] = " ";
    while(1){
        printf("\nAllocator) ");
        fprintf(fp, "\nAllocator) ");
        fgets(user_input, buffer, stdin);
        fprintf(fp, "%s", user_input);

        token = strtok(user_input,space);   //Use tokenizer to parse user input
        
        if(strcmp(token, "RQ")== 0){        //RQ command
            token = strtok(NULL, space);
            int index = atoi(&token[1]);
            token = strtok(NULL, space);
            int size = atoi(token);
            token = strtok(NULL, space);
            if(strcmp(token,"W")!=10){
                printf("Only W(Worst fit) mode is available. Type again. \n");
                continue;
            }
            command_Request(&process_list, memory_size, index, size, 'W');
        }
        else if(strcmp(token, "RL")==0){   //RL command
            token = strtok(NULL, space);
            release_Process(&process_list, atoi(&token[1]));
        }
        else if(strcmp(token, "STAT")== 10){
            printf("\n");
            fprintf(fp, "\n");
            printList(process_list, memory_size);
        }
        else if(strcmp(token, "C") == 10){
            command_Compact(&process_list, memory_size);
        }
        else if(strcmp(token, "QUIT") == 10){
            fprintf(fp, "\n-- End");
            fclose(fp);
            break;
        }
        else{ //help message.
            printf("\nInvalid input. Please try again.\n");
            printf("\n|--------------------------------------------------------------\n");
            printf("|Request allocation: RQ [Process ID][Process_size][fit mode] \n|Release process memory : RL [Process ID] \n|See status : Type STAT \n|Compact Memory : Type C ");
            printf("\n|Exit : Type QUIT");
            printf("\n|--------------------------------------------------------------\n");
            continue;
        }
    }
}
int main(int argc, char *argv[]){
    
    if(argc != 2){
        printf("Wrong number of command line arguments\n");
        printf("[command] [Memory_size]");
        exit(1);
        return 1;
    }

    fp = fopen("KChihoon_rslt.txt", "w");

    fprintf(fp, "-- Chihoon Kim Project4\tSt: 23694627\n");

    run_project4(atoi(argv[1]));
    
    return 0;
}