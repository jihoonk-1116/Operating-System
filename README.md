# Operating-System

## Project1 : Utilizing fork() function

Created child process using parent's fork() function in order to run another program
![image](https://user-images.githubusercontent.com/76544061/153892527-f9ed04de-f3a8-4741-8f72-8ab2d93b10df.png)

## Project2 : Multithread Calculator
Used total more than 100 children and grandchildren thread to calculate huge sum in short time.

* run_part3() -> create children threads <br>
![image](https://user-images.githubusercontent.com/76544061/153893149-ac3e0e15-14fc-49cc-8a21-ddcd845858e5.png) <br>
* create_grand() -> create grand children and they invokes run_for_part3()<br>
![image](https://user-images.githubusercontent.com/76544061/153893458-46ee38a1-331a-4c51-a2d9-1aa74089195e.png)<br>
* Summation is being proceeding in run_for_part3. And, It returns the result in structure.<br>
![image](https://user-images.githubusercontent.com/76544061/153893489-14b6eef3-42cd-42b3-ba4d-af10b55afc2e.png) <br>

Structure definition. <br>

![image](https://user-images.githubusercontent.com/76544061/153893642-d3e6cbd8-2a02-4906-a3c7-2f2585917428.png)<br>



## Project3 : Round-Robin Process Scheduler
Priority processing, Implemented Insertion sort Linked list
## Project4 : Memory Allocator
Worstfit. Find the biggest avaliable memory space to allocate a task.
