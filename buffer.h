#ifndef BUFFER_H
#define BUFFER_H

#include <pthread.h>
#include <semaphore.h>
#include "struct.h"

#define BUFFER_SIZE 500

void buffer_init(shared_buffer_t *buf);
void buffer_push(shared_buffer_t *buf, Item data);
Item buffer_pop(shared_buffer_t *buf);
void buffer_destroy(shared_buffer_t *buf);

#endif