#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "typedefs.h"

const i32 pass_time = 1;

typedef pthread_mutex_t mutex_t;

typedef struct {
    mutex_t* mutex;
} thread_ctx_t;

void* pass_northeners(void* ctx) {
    thread_ctx_t* thread_ctx = ctx;
    i32 farmers_n = 0;

    for (;;) {
        pthread_mutex_lock(thread_ctx->mutex);
        printf("Northern Farmer #%d is passing the bridge\n", farmers_n);
        farmers_n++;
        sleep(pass_time);
        pthread_mutex_unlock(thread_ctx->mutex);
    }

    return null;
}

void* pass_southerners(void* ctx) {
    thread_ctx_t* thread_ctx = ctx;
    i32 farmers_n = 0;

    for (;;) {
        pthread_mutex_lock(thread_ctx->mutex);
        printf("Southern Farmer #%d is passing the bridge\n", farmers_n);
        farmers_n++;
        sleep(pass_time);
        pthread_mutex_unlock(thread_ctx->mutex);
    }

    return null;
}

i32 main() {
    thread_ctx_t* thread_ctx = malloc(sizeof(thread_ctx_t));
    thread_ctx->mutex = malloc(sizeof(mutex_t));
    pthread_mutex_init(thread_ctx->mutex, null);

    pthread_t north_thread, south_thread;

    if (pthread_create(&north_thread, null, pass_northeners, thread_ctx) ||
        pthread_create(&south_thread, null, pass_southerners, thread_ctx)) {
        printf("Failed creating threads!\n");
        exit(-1);
    }

    pthread_join(north_thread, null);
    pthread_join(south_thread, null);

    pthread_mutex_destroy(thread_ctx->mutex);
    free(thread_ctx->mutex);
    free(thread_ctx);

    return 0;
}