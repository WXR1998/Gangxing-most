#include <string>

void send_result(std::string s);
void* receive_input_thread(void* args);
void* print_thread(void* args);
void* solve_thread(void* args);