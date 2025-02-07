#include "buffer.h"

void buffer_init(shared_buffer_t *buf) {
    buf->in = buf->out = 0;
    sem_init(&buf->sem_spazi, 0, BUFFER_SIZE); // BUFFER_SIZE spazi inizialmente liberi
    sem_init(&buf->sem_elementi, 0, 0);       // 0 elementi inizialmente presenti
}

// Inserisce un elemento nel buffer
void buffer_push(shared_buffer_t *buf, Item data) {
    sem_wait(&buf->sem_spazi);               // Aspetta uno spazio libero
    buf->buffer[buf->in] = data;             // Inserisce l'elemento
    buf->in = (buf->in + 1) % BUFFER_SIZE;   // Incrementa il puntatore circolare
    sem_post(&buf->sem_elementi);            // Segnala un nuovo elemento disponibile
}

// Rimuove un elemento dal buffer
Item buffer_pop(shared_buffer_t *buf) {
    sem_wait(&buf->sem_elementi);            // Aspetta un elemento disponibile
    Item data = buf->buffer[buf->out];       // Legge l'elemento
    buf->out = (buf->out + 1) % BUFFER_SIZE; // Incrementa il puntatore circolare
    sem_post(&buf->sem_spazi);               // Segnala uno spazio libero
    return data;
}

void buffer_destroy(shared_buffer_t *buf) {
    sem_destroy(&buf->sem_spazi);
    sem_destroy(&buf->sem_elementi);
}