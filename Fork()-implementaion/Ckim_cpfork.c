/*
CS340 : Operating System
Dr. John Svadlenka
Chihoon Kim
Stu# 23694627

Project 1 made by Chihoon Kim

Command: 
gcc Ckim_cpfork.c -o Ckim_cpfork.exe
./Ckim_cpfork.exe input.txt, parent.txt, child.txt

*/
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>

#include <sys/wait.h> //The sys/wait.h header file declares the wait() function, used for holding processes, in POSIX.

//Function to make file copy
int func_cpfile(const char *srcname, const char *tgtname){
    //This function is called by a parent process.
    //These arguments, srcname and tgtname, comes from the main function.
    int f1, f2;

    if((f1 = open(srcname, O_RDONLY, 0)) == -1){ //Open a given source file(=srcname) using the system call of open(), read only. 
        printf("Can't open %s \n", srcname);     //If the system call is fail, the call will return -1. Then, print this sentence out.
        return 2;
    }
    if((f2 = creat(tgtname, 0644)) == -1){     //Create a file with a given file name(=tgtname) using the systam call of creat().
        printf("Can't create %s \n", tgtname); //If the call is fail, if will return -1. Then print this sentence out.
        return 3;
    }

    int size = 128;
    char buff[size];  //Declare buffer char array with size 128.
    int n;            //Declare integer n to store the return value of read() call. 
    /*
    read() attempts to read up to size bytes from file descriptor f1 into the buffer starting at buff.
    The system call returns the number of bytes read, and it returns 0 at the end of the file. 
    */
    while((n = read(f1,buff,size)) > 0) //The syscall of write() writes up to size bytes from the buff starting at buff
        if(write(f2,buff,n) != n){      //to the file referred to by the file descriptor f2. 
            printf("Can't write file"); //On success, the number of bytes written is returned. On error, -1 is returned. 
            close(f1);
            close(f2);
            return 4;
        }

    close(f1); //close the opened file
    close(f2); //close the written file
    
    printf("Success! by parent\n");

    return 0;
}

//fork and exec system calls.
//Invoke from command line as : ./Ckim_cpfork.exe ,srcname, tgtname, tgt2name

int main(int argc, const char *argv[]){ //cpfile.exe, srcname, tgtname, tgt2name//

    //Since the command consists of 4 arguments, such as an executable file, source name, target name1 for a parent process,
    //target name2 for a child process, argc should be 4. 
    if(argc != 4){
        printf("Wrong number of command line arguments\n");
        exit(1);
        return 1;
    }
    
    func_cpfile(argv[1], argv[2]);  //The parent process in this program calls the func_cpfile() function.
                                    //Argv[1] and argv[2] mean srcname and tgtname respectivly.
    pid_t pid;  //pid_t datatype stands for process identification and it is used to represent process id.
    pid = fork(); //The system call of fork() creates a child process. And its parent process becomes this program.
                  //If a child process is created successfully, the process id(=pid) becomes 0.
    if (pid < 0){ //If fork() is fail to make a child, the process id becomes negative value.
        fprintf(stderr, "Fork Falied"); 
        return 1;
    }
    else if (pid == 0){ /* The child process does below code up to code line 85.
         The system call of 'execlp()' executes cpfile.exe in this path:./cpfile.exe
         The call has the return value of -1 only if the call to execlp is unsuccessful. 
         The below of the code line 82 will not be run since the call does not return anything when it is successful to run. 
        */
        int ret = execlp("./cpfile.exe","cpfile.exe",argv[1],argv[3],NULL); //execlp(const char *path, const char *arg0, *arg1,...,null)
                                                                            //argv[1] and argv[3] mean srcname and tgtname2 respectivly.
        fprintf(stderr, "execlp Falied %d", ret); //When execlp() is unsuccessful, the child process prints this sentence out.
        return 2; //The child process is terminated. 
    }
    else{           //The parent process does below code.
        wait(NULL); //It waits until the child process that was created by fork() ends.
        printf("Child complete\n"); //When the status of the child process is changed, the parent process prints this sentence out.
    }
    return 0;
}