#include <string>

void* receive_input_thread(void* args);
void* print_thread(void* args);
void* solve_thread(void* args);
void* send_thread(void *args);
void* solve_decomp_thread_M(void* args);
void* solve_decomp_thread(void* args);