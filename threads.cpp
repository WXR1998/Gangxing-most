#include <cstdio>
#include <cstdlib>
#include <string>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <ctime>
#include <unistd.h>
#include <vector>
#include <cstring>
#include <iostream>
#include <csignal>
using namespace std;

#include "utils.hpp"
#include "int128.hpp"

using namespace std;
extern Queue queues[MAX_M];
extern SendQueue sendqueue;

int send_result_count = 0;
char post_final[4096], str[4096];
int send_socket;

void handle_pipe(int sig){
    struct sockaddr_in serv_addr;
    send_socket = socket(AF_INET, SOCK_STREAM, 0);
    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(10002);
    serv_addr.sin_addr.s_addr = inet_addr("47.95.111.217");
    printf("Connection lost, reconnecting...\n");
    while (connect(send_socket, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) == -1){
        printf("Connect to server error.\n");
        sleep(1);
    }
}

void* send_thread(void *args){
    struct sigaction action;
    action.sa_handler = handle_pipe;
    sigemptyset(&action.sa_mask);
    action.sa_flags = 0;
    sigaction(SIGPIPE, &action, NULL);

    while (1){
        try{
            int ret;
            struct sockaddr_in serv_addr;
            socklen_t len;
            fd_set t_set1;
            struct timeval tv;

            send_socket = socket(AF_INET, SOCK_STREAM, 0);
            memset(&serv_addr, 0, sizeof(serv_addr));
            serv_addr.sin_family = AF_INET;
            serv_addr.sin_port = htons(10002);
            serv_addr.sin_addr.s_addr = inet_addr("47.95.111.217");
            while (connect(send_socket, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) == -1){
                printf("Connect to server error.\n");
                sleep(1);
            }

            while (1){
                if (sendqueue.size() > 0){
                    string s = sendqueue.pop();

                    post_final[0] = 0;
                    strcat(post_final, "POST /submit?user=psi&passwd=CyQPv4tr HTTP/1.1\r\nAccept: */*\r\nConnection: close\r\nContent-Type: raw\r\nContent-Length: ");
                    sprintf(str, "%d\r\nHost: 47.95.111.217:10002\r\n\r\n%s", s.size(), s.c_str());
                    strcat(post_final, str);

                    ret = write(send_socket, post_final, strlen(post_final));
                    if (ret > 0){
                        string answer;
                        if (s.size() > 40)
                            answer = s.substr(0, 10) + "..." + s.substr(s.size() - 10, 10);
                        else
                            answer = s;
                        printf("%s\tAnswer submitted. \nAnswer: \t%s\n", now_time().c_str(), answer.c_str());
                        ++send_result_count;
                    }
                }
            }
            close(send_socket);
        }
        catch(const std::exception& e) {
            std::cerr << e.what() << '\n';
        }
    }
    return NULL;
}

void* receive_input_thread(void* args){
    char message[BUF_SIZE];
    int str_len;
    while (1){
        try{
            int sock;
            struct sockaddr_in serv_addr;
            sock = socket(PF_INET, SOCK_STREAM, 0);

            memset(&serv_addr, 0, sizeof(serv_addr));
            serv_addr.sin_family = AF_INET;
            serv_addr.sin_addr.s_addr = inet_addr("47.95.111.217");
            serv_addr.sin_port = htons(10001);

            printf("Connecting...\n");
            while (connect(sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) == -1){
                printf("Connect to server error.\n");
                sleep(1);
            }
            printf("Start listening from the server...\n");

            while (1){
                str_len = recv(sock, message, BUF_SIZE, 0);
                for (int i = 0; i < str_len; ++i)
                    if (message[i] >= '0' && message[i] <= '9'){
                        for (int j = 0; j < MAX_M; ++j)
                            queues[j].push(message[i] - '0');
                    }
            }
        }
        catch(const std::exception& e){
            std::cerr << e.what() << '\n';
        }
    }
    return NULL;
}

void* print_thread(void* args){
    while (1){
        printf("%s\tSend result count: %d\n", now_time().c_str(), send_result_count);
        sleep(5);
    }
    return NULL;
}

void* solve_thread(void* args){
    std::vector <BigInteger> m;
    // m.push_back(BigInteger("20220209192254"));
    m.push_back(BigInteger("104648257118348370704723099"));
    m.push_back(BigInteger("72743368316654380304300807600573288589391843379298689492067180085248"));
    // m.push_back(BigInteger("125000000000000064750000000000009507500000000000294357"));
    ModMatrix *mm;
    mm = new ModMatrix(m.size(), 512, m);
    string answer;
    while (1){
        if (queues[0].size() > 0){
            int d = queues[0].pop();
            mm->push(d);
            if (mm->check_valid(&answer))
                sendqueue.push(answer);
        }
    }
    return NULL;
}

void* solve_decomp_thread_M(void* args){
    void **arg = (void**)args;
    Queue *q = (Queue*)arg[0];
    ModMatrixDecomp *mmd = (ModMatrixDecomp*)arg[1];
    string answer;

    double time_sum = 0.0;
    int sample_count = 0;

    while (1){
        try{
            if (q->size() > 0){
                clock_t start_time = clock(), end_time;
                int d = q->pop();
                mmd->push(d);
                if (mmd->check_valid(&answer)){
                    end_time = clock();
                    sendqueue.push(answer);
                    double time_cost = (double)(end_time - start_time) / CLOCKS_PER_SEC * 1000;
                    time_sum += time_cost;
                    sample_count++;
                    printf("Average time cost: %lf ms\n", time_sum / sample_count);
                }
            }
        }
        catch(const std::exception& e){
            std::cerr << e.what() << '\n';
        }
        
    }
    return NULL;
}

void* solve_decomp_thread(void* args){
    // 每个M都有一个单独的线程进行处理
    ModMatrixDecomp *mmd[MAX_M];
    for (int i = 0; i < MAX_M; ++i)
        mmd[i] = new ModMatrixDecomp(512);
    std::vector <int128> tmp;
    FILE *f = fopen("factors.txt", "r");
    // 每个m的质因子都存在文件factors.txt中，以0划分每个m的范围
    printf("Factors: \n");
    int cnt = 0;
    while (!feof(f)){
        int128 factor;
        tmp.clear();
        while (1){
            factor = read_from_file(f);
            if (factor == 0)
                break;
            tmp.push_back(factor);
        }
        mmd[cnt++]->add_m(tmp);
        for (auto i: tmp){
            print_int128(i);
            printf(" ");
        }
        printf("\n");
    }

    void* pnts[MAX_M][2];
    pthread_t tids[MAX_M];
    for (int i = 0; i < MAX_M; ++i){
        pnts[i][0] = &queues[i];
        pnts[i][1] = mmd[i];
        pthread_create(&tids[i], NULL, solve_decomp_thread_M, pnts[i]);
    }
    while (1);

    return NULL;
}