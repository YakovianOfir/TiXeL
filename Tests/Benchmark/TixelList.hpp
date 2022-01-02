#pragma once

#include <assert.h>
#include <getopt.h>
#include <limits.h>
#include <pthread.h>
#include <signal.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/time.h>
#include <time.h>
#include <stdint.h>


static volatile int stop;

#define DEFAULT_DURATION                10000
#define DEFAULT_INITIAL                 256
#define DEFAULT_NB_THREADS              1
#define DEFAULT_RANGE                   1000
#define DEFAULT_SEED                    0
#define DEFAULT_UPDATE                  20
#define DEFAULT_TX	    	true
#define DEFAULT_ALTERNATE	        0
#define DEFAULT_EFFECTIVE	 	1

#define XSTR(s)                         STR(s)
#define STR(s)                          #s

#define TRANSACTIONAL                   d->unit_tx

typedef void node_l_t;
typedef void intset_l_t;
typedef void main_usage_t;
typedef void memory_manager_t;
typedef void thread_usage_t;
typedef int val_t;

#ifdef __cplusplus
extern "C"
{
#endif

intset_l_t *set_new_l();
void set_delete_l(intset_l_t *set);
int set_size_l(intset_l_t *set);

#ifdef __cplusplus
}
#endif