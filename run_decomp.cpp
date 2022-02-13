#include <pthread.h>

#include "utils.hpp"
#include "threads.hpp"

Queue queues[MAX_M];

int main(){
    pthread_t recv_tid, print_tid, solve_tid;
    pthread_create(&recv_tid, NULL, receive_input_thread, NULL);
    pthread_create(&print_tid, NULL, print_thread, NULL);
    pthread_create(&solve_tid, NULL, solve_decomp_thread, NULL);
    pthread_exit(NULL);
    return 0;
}