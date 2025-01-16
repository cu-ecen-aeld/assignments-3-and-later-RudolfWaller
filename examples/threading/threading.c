#include "threading.h"
#include "pthread.h"
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>


// Optional: use these functions to add debug or error prints to your application
//#define DEBUG_LOG(msg,...)
#define DEBUG_LOG(msg,...) printf("threading: " msg "\n" , ##__VA_ARGS__)
#define ERROR_LOG(msg,...) printf("threading ERROR: " msg "\n" , ##__VA_ARGS__)

void* threadfunc(void* thread_param)
{

    // TODO: wait, obtain mutex, wait, release mutex as described by thread_data structure
    // hint: use a cast like the one below to obtain thread arguments from your parameter
    //struct thread_data* thread_func_args = (struct thread_data *) thread_param;
    
    struct thread_data* thread_func_args = (struct thread_data *) thread_param;

    DEBUG_LOG("Waiting to obtain mutex");
    usleep(1000*thread_func_args->wait_to_obtain_ms);

    if(pthread_mutex_lock(thread_func_args->mutex)==0)
    {
	DEBUG_LOG("Mutex obtained by thread");

	usleep(1000*thread_func_args->wait_to_release_ms);

     	pthread_mutex_unlock(thread_func_args->mutex);
	DEBUG_LOG("Mutex released by thread");

	thread_func_args->thread_complete_success=true;
    }
    else
    {
        thread_func_args->thread_complete_success=false;

	ERROR_LOG("Failed to obtain mutex");
    }
   
    //free(thread_func_args);

    return thread_param;
}


bool start_thread_obtaining_mutex(pthread_t *thread, pthread_mutex_t *mutex,int wait_to_obtain_ms, int wait_to_release_ms)
{
    /**
     * TODO: allocate memory for thread_data, setup mutex and wait arguments, pass thread_data to created thread
     * using threadfunc() as entry point.
     *
     * return true if successful.
     *
     * See implementation details in threading.h file comment block
     */

	struct thread_data* data=(struct thread_data *) malloc(sizeof(struct thread_data));
	if(data==NULL)
	{
		ERROR_LOG("Failed to allocate memory for thread data");
		return false;
	}

	// Thread configuration
	data->mutex = mutex;
	data->wait_to_obtain_ms  = wait_to_obtain_ms;
	data->wait_to_release_ms = wait_to_release_ms;

	int rc=pthread_create(thread, NULL, &threadfunc, data);

	if(rc!=0)
	{
		ERROR_LOG("Failed to create thread");
		free(data);
		return false;
	}

	DEBUG_LOG("Thread successfully created");
	return true;
}

/*
int main() {
    pthread_t thread;
    pthread_mutex_t mutex;

    pthread_mutex_init(&mutex, NULL);

    // Inicia o thread com tempos de espera de 500ms e 1000ms
    if (start_thread_obtaining_mutex(&thread, &mutex, 500, 1000)) {
        pthread_join(thread, NULL);
    } else {
        ERROR_LOG("Failed to start thread");
    }

    pthread_mutex_destroy(&mutex);
    return 0;
}
*/
