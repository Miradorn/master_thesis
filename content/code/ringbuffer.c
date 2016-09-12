#include <stdio.h>
#include <stdatomic.h>
#include <pthread.h>
#include <stdlib.h>
#include <sys/time.h>

int BUF_SIZE = 5;

char* ring_buffer;
char* _Atomic read_ptr;
char* write_ptr;

void init_ring_buffer() {
  ring_buffer = malloc(BUF_SIZE);
  read_ptr = ring_buffer;
  write_ptr = ring_buffer;

}

char* buffer_next(char* ptr) {
  return (char*) ring_buffer + (((ptr - ring_buffer) + 1) % BUF_SIZE);
}

void process(char* data){
  struct timespec ts;
  ts.tv_sec = 0;
  ts.tv_nsec = 1000000;
  nanosleep(&ts, NULL);
}

void producer_main() {
  char* next_write_ptr;
  while (!feof(stdin)){
    next_write_ptr = buffer_next(write_ptr);
    if (next_write_ptr != read_ptr) {
      *write_ptr = getc(stdin);
      write_ptr = next_write_ptr;

      struct timespec ts;
      ts.tv_sec = 0;
      ts.tv_nsec = 100000000ULL * rand() / RAND_MAX ;
      nanosleep(&ts, NULL);
    }
  }
}

void* consumer_main(void* thread_id) {
  int tid = (int) thread_id;
  char* current_rptr;
  char* current_wptr;
  char data = 0;
  char* next_read_ptr;
  char* local_read_ptr;

  while (1) {
    local_read_ptr = read_ptr;

    if (local_read_ptr != write_ptr) {
      data = *local_read_ptr;
      next_read_ptr = buffer_next(local_read_ptr);
      if (atomic_compare_exchange_weak(&read_ptr, &local_read_ptr, next_read_ptr)) {
        process(&data);
      }
      struct timespec ts;
      ts.tv_sec = 0;
      ts.tv_nsec = 100000000ULL * rand() / RAND_MAX ;
      nanosleep(&ts, NULL);
    }
  }
}


void create_consumers(int num_consumers) {
  pthread_t threads[num_consumers];
  int thread_ids[num_consumers];
  int rc;
  for(int t = 0; t < num_consumers; t++){
    thread_ids[t] = t;
    rc = pthread_create(&threads[t], NULL, consumer_main, (void* ) t);
    if (rc){
      pthread_exit(NULL);
    }
  }
}

int main() {
  init_ring_buffer();

  create_consumers(2);

  producer_main();

  return 0;
}
