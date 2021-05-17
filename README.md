# Starve-free-reader-writer-problem's-Solution
This repository aims to provide a detailed solution for starve free readers-writers problem.
# Problem Desciption
This is a classical problem of process synchronization and mutual exclusion where a shared file is being accessed by multiple processes (amongst which some want to read it called as readers and others want to write to it called as writers) concurrently.
# Starve Free Solution
All readers and writers will be granted access to the resource in their order of arrival (FIFO order). If a writer arrives while readers are accessing the resource, it will wait until those readers free the resource, and then modify it as soon as the resource is released by the reader. New readers arriving in the meantime will have to wait.
## Initialization
### Readers_count : 
initialized as 0 // indicates number of readers currently accessing the critical section
### Data: 
initialized as 1 // can be initialized by any value , this is the value modified by writer during write process
### Order_mutex: 
Semaphore used for maintaining fifo order of read/write requests made for accessing critical section
### Access_mutex: 
Semaphore used for requesting exclusive access to modify reader_count variable
### Readers_mutex: 
Semaphore used for avoiding conflicting accesses to entry and exit section (aiding in achieving mutual exclusion condition that only one reader can enter/ exit the critical section at a time) as multiple readers can simultaneously access the resource/ critical section. 
### Reader's Code
void *reader(void *id)
{
  //ENTRY SECTION
  /* Pseudo-code for sem_wait()
  sem_wait(){
   value = value-1;
   if(value<0){block();}
   else return;
  }*/
    
  //printf("Reader %d checks its turn\n", *((int *) id));
  sem_wait(&order_mutex); //check for the turn to get executed, puts the blocked process in a fifo queue if it's not its turn currently
  sem_wait(&readers_mutex); // requesting access to modify readers count
  readers_count++;
  //printf("After entry of reader %d ,readers count: %d\n", *((int *) id), readers_count);
  if(readers_count==1) sem_wait(&access_mutex);
  // if it is the first reader then get the access to the reader for accessing the critical section and block the writer from accessing it
   
  /* Pseudo-code for sem_post()
  sem_post(){
   value = value+1;
   if(value<0) wakeup(); // a blocked process
   else return;
   }*/
  sem_post(&order_mutex); // release the semaphore to get the next reader/ writer to be serviced
  sem_post(&readers_mutex); // release access to modify readers count
  printf("Data read by the reader%d is %d\n",*((int *) id),data);
  //EXIT SECTION
  sem_wait(&readers_mutex); // again request access to readers count
  readers_count--; //  decrement the count as a reader is leaving
  //printf("After exit of reader %d ,readers count: %d\n", *((int *) id), readers_count);
  if(readers_count==0) sem_post(&access_mutex);
  // if it is the last reader then release the access to the critical section so that it can be used by any waiting writer
   
  sem_post(&readers_mutex); // release access to modify readers count
  return id;
}
### Writer's Code
void *writer(void *id)
{
  sem_wait(&order_mutex); //check for the turn to get executed, puts the blocked process in a fifo queue if it's not its turn currently
  sem_wait(&access_mutex); // get the access to the writer for accessing the critical section and block the readers from accessing it
  sem_post(&order_mutex); // release the semaphore to get the next reader/ writer to be serviced
  data = data* 2 + 5; //modify the value of data during writing
  printf("Data written by the writer%d is %d\n",*((int *) id),data);
  sem_post(&access_mutex); //release the access to the critical section so that it can be used by any waiting reader
  return id;
}

## Correctness of Solution
### Mutual Exclusion:
The access_mutex semaphore ensure that only a single writer can access the critical section at any moment, aiding in mutual exclusion amongst writers. As, if any other writer try to enter then the value of access_mutex won’t allow it to do so.
Besides, when the first reader enter in critical section it calls wait(access_mutex) that aids in mutual exclusion between readers and writers as at that moment it blocks the writer.

### Bounded Wait:
Before accessing the critical section both reader and writer use order_mutex which checks for the turn of the request made. If currently the request can’t be granted then it blocks that thread and puts it in the FIFO queue. This ensures that when a thread needs to be released,it is done in FIFO sequence so that all get an opportunity of being executed after a finite amount of time and no one starves for an indefinite long  time. This meets the requirement for bounded wait conditions.

### Progress Requirement:
The code is structured so that there won’t be any chances of occurrence of deadlock. Both the reader and writer take a finite amount of time in critical section execution and during exit release the waiting semaphore so that they can release the blocked threads/ processes for re- cheking and entering in critical section. Thus, no process hinders the execution of the other when there is no reader/ writer in the critical section.
