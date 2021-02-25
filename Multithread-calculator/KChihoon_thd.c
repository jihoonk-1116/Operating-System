/*
CS340 : Operating System
Dr. John Svadlenka
Section 33 : Tue, Thr 5:00 - 6:15
Chihoon Kim
Stu# 23694627

Project 2 made by Chihoon Kim
Command: 
gcc KChihoon_thd.c -o KChihoon_thd.exe
./KChihoon_thd.exe [number] [the number of child] [the number of grand child]
*/

#include <pthread.h>  // All pthread program must include the pthread.h header file.
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

void *create_grand(void *param);
void *runner_for_part1(void *param);
void *runner_for_part2(void * param);
void *runner_for_part3(void *param);
int *my_parser(int index, int s, int e);
long int *sums; //Shared int array for part2
long int sum;   //Shared int for part1

FILE *fp;       //Global file pointer to log print out.

struct sum_struct{          //Part 3 uses structure to store data for summing. 
    long int limit;         //Each structure has upper bound(limit), start point and summing result and index. 
    long int s;
    long int answer;
    int num_of_grand_thr;   //The number of grand child for setting increment in "create_child" function.
    int index;
};
void run_part1(char* number){

    pthread_t tid;         //Declear the identifier for a thread.
    pthread_attr_t attr;   //Each thread has a set of attributes, including stack size and scheduling information.

    pthread_attr_init(&attr); //Set the attributes in the function call pthread_attr_init(&attr), default attributes provided.
    pthread_create(&tid, &attr, runner_for_part1, number); //A thread is created with this function call
                                                           //with passing the thread identifier and attributes, and the name of function
                                                           //where the new thread will begin execution 
    pthread_join(tid,NULL);   //The parent thread is waiting when its child terminates.

    fprintf(fp, "Part1 ---> sum: %ld \n\n" , sum); //Print the shared data sum that is calculated by child thread.
    
}
void *runner_for_part1(void *param){   //This function is executed by a child thread where is created in run_part1.

    int i, upper = atoi(param);     //The given number from its parent is converted ASCII to integer and split into first and last number.
    sum = 0;    //Initialize shared global data.

    for(int i =1;i<=upper;i++){
        sum+=i; //Calculate 
    }
    pthread_exit(0); //Terminate child thread.
}
//run_part2 and run_part3 use multi threads to calculate a large number efficiently
void run_part2(const char *input_num, const char* num_thrs){ //Number and the number of child thread that will be created.
    fprintf(fp,"Part 2 ------> Parent : Start of  Execution------\n ");
    int high = atoi(input_num);         //Input number is converted ASCII to integer by atoi
    int num_threads = atoi(num_thrs);   //num_threads is converted ASCII to integer by atoi
    int increment = high / num_threads; //Declear increment to the dividing value of upper bound (high) by the number of thread
    int end=increment, start=1;         //Declear first end point and first start point to distribute sum bound to each child.
    int *parameters[num_threads];       //Declear parameters int pointer array with the length of num_thread in order to use as argument to execute children
    sums = malloc(num_threads);         //Initialize sums int array using malloc in order to store the result of children summation

    pthread_t threads[num_threads];     //Declear child threads as input.

    for(int i = 0;i<num_threads;i++){
        parameters[i] = my_parser(i, start, end); //The function my_parser is required thread's index, the start point and the end point for performing summation, 
                                                  //and the function will return an integer array containing data for summation. 
        pthread_attr_t attr;                      //Declear attribute for each child thread
        pthread_attr_init(&attr);                 //Set the attributes in the function call pthread_attr_init(&attr), default attributes provided.
        pthread_create(&threads[i], &attr, runner_for_part2, parameters[i]); //Create child thread with parameters[i] and each child execute runner_for_part2.
        start += increment;                       //Update start and end point for the next child.
        end += increment;
    }
    
    long int total_sum = 0;     //Initialize long int total sum to store a large number and prevent overflow.

    for(int i=0;i<num_threads;i++){
        pthread_join(threads[i], NULL); //Parent thread is waiting until its children is terminated.
        total_sum += sums[i];           //Each child will store own summation in the sums array with own index after it completes own job.
                                        //And parent thread is gathering each child's sum. 
    }
    if(high != end){                    //When the input number is not divisible the number of thread, this code will be started. 
        for(int i = end-increment+1; i<=high;i++){
            printf("%d  ", i);
            total_sum+=i;
        }
    }
    fprintf(fp, "Parent: Total sum is %ld \n " ,total_sum);
    fprintf(fp, "Parent: End of  Execution\n ")  ; 
    //pthread_exit(0);                 //Parent thread is terminated.
}
void * runner_for_part2(void * param){ //Child threads coming from run_part2 will execute this function.
    long int sum = 0;
    int *temp = param;                 //Casting generic pointer argument to int pointer.
    int index = temp[0], s = temp[1], e = temp[2];  //Split the array into index, start, end.
    for(int i = s;i<=e;i++){
        sum += i;                      //Execute sum.
    }
    sums[index] = sum;                 //Assign a child's sum to sums global array using own index.
    fprintf(fp, "Child thread number %d: \t child start=%d \t end = %d \t sum = %ld \n ", index, s, e, sum); 
    pthread_exit(0);                   //Terminate child thread.
}
int *my_parser(int index, int s, int e){ //This function stores its arguments in an int array, and return the array.
    int *temp = malloc(3);
    temp[0] = index;
    temp[1] = s;
    temp[2] = e;
    return temp;
}
void run_part3(const char *number, const char *num_thr, const char *grand_num_thr){ //run_part3 uses child and grand child threads and structure.
    fprintf(fp, "\nPart 3 -----> Parent : Start of  Execution------\n ") ; 
    long int total_sum = 0;                      //Long int to take a large number. Code line from 117 to 121 is same as run_part2.
    int num_threads = atoi(num_thr);
    int grand_num_threads = atoi(grand_num_thr); //The number of grand child
    int upper = atoi(number);
    int increment = upper / num_threads;
    int start =1, end=increment;

    struct sum_struct parameters[num_threads];   //Declear structure array for child threads.

    pthread_t tids[num_threads];

    for(int i =0;i<num_threads;i++){
        //Assign end, start point, the number of grand child and index to each structure's data fleids, like the parameter array in part2.
        parameters[i].limit = end;
        parameters[i].s = start;
        parameters[i].num_of_grand_thr = grand_num_threads;
        parameters[i].index = i;

        pthread_attr_t attr;
        pthread_attr_init(&attr);
        pthread_create(&tids[i], &attr, create_grand ,&parameters[i]);  //Child thread is created with a child structure. 

        end += increment;
        start += increment;
    }
    for(int i = 0;i<num_threads;i++){
        pthread_join(tids[i], NULL);        //Wait until its child is done.
        fprintf(fp, "Child thread number %d: \t child start=%ld \t end = %ld \t  childsum = %ld \n ", i, parameters[i].s, parameters[i].limit, parameters[i].answer); 
        total_sum += parameters[i].answer;  //Bring each child's summation from a child's structure's data field, and cumulate into total_sum.
    }

    fprintf(fp, "\nParent: Total sum = %ld \n " ,total_sum);
    fprintf(fp, "Parent: End of  Execution\n ");
    fclose(fp);     //Close file pointer. 
    pthread_exit(0);//Terminate child threads.    
}
void * create_grand(void *param){

    struct sum_struct *child_struct = (struct sum_struct*) param; //Casting generic pointer param to a structure pointer.

    long int grand_sum = 0;         //Declear grand_sum to collect grand child's summation as long int to prevent overflow.
    int start = child_struct->s;    //Assign start point using child structure.
    int increment = ((child_struct->limit) - start + 1) / (child_struct->num_of_grand_thr); //Because each child has different start and end point,
    int end = start + increment -1; //Initialize end point                                  //calculate increment like this. Every grand child must have same increment. 

    struct sum_struct grand_parameters[child_struct->num_of_grand_thr]; //Generate structure array as the number of grand child

    pthread_t grand_tids[child_struct->num_of_grand_thr];               //Declear grand child threads as the number of grand child.

    for(int i =0;i<child_struct->num_of_grand_thr;i++){

        grand_parameters[i].limit = end;    //Assign start and end point to grand child's structure.
        grand_parameters[i].s = start;

        pthread_attr_t attr;
        pthread_attr_init(&attr);
        pthread_create(&grand_tids[i], &attr, runner_for_part3 ,&grand_parameters[i]); //Child thread is created with a grand child structure. 

        end += increment;   //Increase end and start point as increment.
        start += increment;
    }
     for(int i = 0;i<child_struct->num_of_grand_thr;i++){
        pthread_join(grand_tids[i], NULL);      //Child threads are waiting when its grand children are done.
        fprintf(fp, "Child thread number %d, GrandChild thread number %d: \t grandchild start=%ld \t grandchildend = %ld \t grand childsum = %ld \n ", child_struct->index, (child_struct->index)*(child_struct->num_of_grand_thr)+i, grand_parameters[i].s, grand_parameters[i].limit, grand_parameters[i].answer)  ;
        grand_sum += grand_parameters[i].answer;//Collect grand child's summation stored in grand child's structure into grand_sum.
    }

    child_struct->answer = grand_sum;           //Store grand_sum in child structure's answer.

    pthread_exit(0);                            //Terminate child thread.

}
void * runner_for_part3(void* param){           //Grand child threads coming from run_part2 will execute this function.
                                
    struct sum_struct *arg_struct = (struct sum_struct*) param; //Casting generic pointer argument to int pointer.
    
    long int sum = 0;
    for(long int i = arg_struct -> s;i<=arg_struct->limit;i++){
        sum += i;
    }
    arg_struct -> answer = sum;

    pthread_exit(0);
}
int main(int argc, char **argv){
    // Arguments check to prevent an error.
    if(argc != 4){
        printf("Wrong number of command line arguments\n");
        printf("[command] [number] [the number of child] [the number of grand child]");
        exit(1);
        return 1;
    }
    if(atoi(argv[1]) > 10000000){
        printf("Too large number\n");
        printf("Please enter less than and equal to 10000000\n");
        printf("[command] [number] [the number of child] [the number of grand child]");
        exit(1);
        return 1;
    }
    if(atoi(argv[2]) > 50){
        printf("Too many child threads\n");
        printf("Please enter less than and equal to 50 of child threads\n");
        printf("[command] [number] [the number of child] [the number of grand child]");
        exit(1);
        return 1;
    }

    if(atoi(argv[3])>50){
        printf("Too many grand child threads.\n");
        printf("Please enter less than and equal to 50 of grand child threads\n");
        printf("[command] [number] [the number of child] [the number of grand child]");
        exit(1);
        return 1;
    }

    fp = fopen("KChihoon_rslt.txt", "w"); // Create a text file with writing mode. 
    
    run_part1(argv[1]);                   //run part1 with number
    run_part2(argv[1], argv[2]);          //run part2 with number and the number of child
    run_part3(argv[1], argv[2], argv[3]); //run part3 with number and thr number of child and grand child

    return 0;
}