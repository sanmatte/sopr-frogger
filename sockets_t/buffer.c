#include "buffer.h"

/**
 * @brief  Initialize the buffer
 * @param  buf buffer to initialize
 */
void buffer_init(shared_buffer_t *buf) {
    buf->in = buf->out = 0;
    sem_init(&buf->sem_spazi, 0, BUFFER_SIZE);             // semaphore for free buffer spaces
    sem_init(&buf->sem_elementi, 0, 0);                    // semaphore for available elements
    pthread_mutex_init(&buf->mutex, NULL);
}


/**
 * @brief  Push an element into the buffer
 * @param  buf buffer to push the element into
 * @param  data element to push
 */
void buffer_push(shared_buffer_t *buf, Item data) {
    sem_wait(&buf->sem_spazi);                             // wait for a free space
    pthread_mutex_lock(&buf->mutex);
    pthread_setcancelstate(PTHREAD_CANCEL_DISABLE, NULL);  // disables thread cancellation
    buf->buffer[buf->in] = data;                           // insert the element
    buf->in = (buf->in + 1) % BUFFER_SIZE;                 // increment the circular pointer
    sem_post(&buf->sem_elementi);                          // signal an available element
    pthread_mutex_unlock(&buf->mutex);
    pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);   // enables thread cancellation
}


/**
 * @brief  Pop an element from the buffer
 * @param  buf buffer to pop the element from
 * @return the popped element
 */
Item buffer_pop(shared_buffer_t *buf) {
    sem_wait(&buf->sem_elementi);                          // wait for an available element
    Item data = buf->buffer[buf->out];                     // get the element
    buf->out = (buf->out + 1) % BUFFER_SIZE;               // increment the circular pointer
    sem_post(&buf->sem_spazi);                             // signal a free space
    return data;
}


/**
 * @brief  Destroy the buffer
 * @param  buf buffer to destroy
 */
void buffer_destroy(shared_buffer_t *buf) {
    sem_destroy(&buf->sem_spazi);
    sem_destroy(&buf->sem_elementi);
    pthread_mutex_destroy(&buf->mutex);
}