#include<stdio.h>
#include<pthread.h>
#include<semaphore.h>

sem_t order_mutex, access_mutex, readers_mutex;
int data = 0,readers_count = 0;

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

int main(){
  // array of threads of readers, writers
  pthread_t readers[10],writers[10];
  //initializing the semaphores declared earlier at address pointed by respective semaphores (mentioned as first argument in sem_init())
  //the value 0 mentioned in second argument in sem_init indicates that the semaphore is shared between the threads of a process and is located at some address such that it is visible to all the threads
  //the third argument specifies the intializing value of each semaphore -> all the semaphores are initialized with 1
  sem_init(&order_mutex,0,1);
  sem_init(&readers_mutex,0,1);
  sem_init(&access_mutex,0,1);
  
  //create new threads for readers and writers using pthread_create()
  //NULL specifies that thread is created with default attributes
  // the newly created thread starts execution by invoking respective start routines as mentioned in 3rd argument of the fn and the arguments to that routine are mentioned in 4th argument of the pthread_create()
  //The ID of the new thread is stored in the buffer pointed to by the 1st argument of pthread_create()
  int i;
  for(i=0;i<=9;i++)
  {
    pthread_create(&writers[i],NULL,writer,(void *)&i);
    pthread_create(&readers[i],NULL,reader,(void *)&i);
  }
  //The pthread_join() function suspends execution of the calling thread until the target thread (first argument of the function) terminates, unless the target thread has already terminated. 
  // the NULL value represnts that exit status of joined thread is not required.
  // this function helps to reclaim the resorces earlier used by the threads
  for(i=0;i<=9;i++)
  {
    pthread_join(writers[i],NULL);
    pthread_join(readers[i],NULL);
  }
  return 0;
}
