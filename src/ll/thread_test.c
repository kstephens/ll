/* Test to see if stack data above thread spawning point
 * is COW mapped into the thread's stack's address space
 */
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>


typedef const char *data_t;

volatile data_t *thread_safe_globalp = 0;
#define thread_safe_global (*thread_safe_globalp)

static 
int sys_main(int *argcp, char **argvp, int (*mainp)(int argc, char **argv))
{
  data_t thread_safe_var = "<<default>>";
  thread_safe_globalp = &thread_safe_var;
  return mainp(*argcp, *argvp);
}


#define ASSERT_DEFAULT_OR(X) \
  assert(strcmp(thread_safe_global, "<<default>>") == 0 || \
	 strcmp(thread_safe_global, X) == 0)

#define ASSERT_DEFAULT_OR(X)

static
void *thread_main(void *ptr)
{
  void *stackp = &stackp;
  void *save = thread_safe_global;
  int i;

  for ( i = 0; i < 20; ++ i ) {
    ASSERT_DEFAULT_OR(ptr);

    printf("%s: %p: tsg = %s\n", ptr, stackp, (char*) thread_safe_global);
    ASSERT_DEFAULT_OR(ptr);
    thread_safe_global = ptr;
    ASSERT_DEFAULT_OR(ptr);
    sleep(rand() % 4);

    ASSERT_DEFAULT_OR(ptr);
    printf("%s: %p: tsg = %s\n", ptr, stackp, (char*) thread_safe_global);
    ASSERT_DEFAULT_OR(ptr);
    thread_safe_global = save;
    ASSERT_DEFAULT_OR(ptr);
    sleep(rand() % 4);
    ASSERT_DEFAULT_OR(ptr);
  }

  return ptr;
}


static
int prog_main(int argc, char **argv)
{
  pthread_t thread1, thread2;
  int iret1, iret2;

  /* Create independent threads each of which will execute function */
  
  iret1 = pthread_create( &thread1, NULL, thread_main, (void*) "foo");
  iret2 = pthread_create( &thread2, NULL, thread_main, (void*) "bar");
  
  /* Wait till threads are complete before main continues. Unless we  */
  /* wait we run the risk of executing an exit which will terminate   */
  /* the process and all threads before the threads have completed.   */
  
  pthread_join( thread1, NULL);
  pthread_join( thread2, NULL); 
  
  printf("Thread 1 returns: %d\n", iret1);
  printf("Thread 2 returns: %d\n", iret2);

  return 0;
}


int main(int argc, char **argv)
{
  return sys_main(&argc, &argv, prog_main);
}
