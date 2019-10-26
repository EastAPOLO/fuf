/* thr.c
 * functions for starting/stopping the loading threads */

#include <pthread.h>
#include <signal.h>
#include <stdbool.h>
#include "thr.h"
#include "sysext.h"

pthread_t load_thr;
pthread_t display_thr;
pthread_t preview_thr;
pthread_cond_t run_preview = PTHREAD_COND_INITIALIZER;
pthread_mutex_t preview_lock = PTHREAD_MUTEX_INITIALIZER;
pid_t preview_pid = 0;
bool items_loading = false;

void
start_load(void *load_items, void *display_load)
{
	items_loading = true;
	pthread_create(&load_thr, NULL, load_items, NULL);
	pthread_create(&display_thr, NULL, display_load, NULL);
}

void
stop_load()
{
	if (items_loading) {
		items_loading = false;
		pthread_join(load_thr, NULL);
		pthread_join(display_thr, NULL);
	}
}

void
init_preview(void *load_preview)
{
	pthread_create(&preview_thr, NULL, load_preview, NULL);
}

void
queue_preview()
{
	pthread_mutex_lock(&preview_lock);
	pthread_cond_signal(&run_preview);
	pthread_mutex_unlock(&preview_lock);
}

void
cancel_preview()
{
	if (preview_pid) {
		ext_kill(preview_pid, SIGKILL);
	}
}
