#include <cstdio>
#include <cstdlib>
#include <string>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <ctime>
#include <unistd.h>
#include <vector>
#include <cstring>

#include "utils.hpp"
#include "int128.hpp"

using namespace std;
extern Queue q;

int send_result_count = 0;
void send_result(string s){
    int sock, ret;
    struct sockaddr_in serv_addr;
    char post_final[4096], post_content[4096], str[128], buf[4096];
    socklen_t len;
    fd_set t_set1;
    struct timeval tv;

    sock = socket(AF_INET, SOCK_STREAM, 0);
    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(10002);
    serv_addr.sin_addr.s_addr = inet_addr("47.95.111.217");
    while (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) == -1){
        printf("Connect to server error.\n");
        sleep(1);
    }

    memset(post_content, 0, sizeof(post_content));
    strcat(post_content, s.c_str());
    len = strlen(post_content);
    sprintf(str, "%d", len);

    memset(post_final, 0, sizeof(post_final));
    strcat(post_final, "POST /submit?user=psi&passwd=CyQPv4tr HTTP/1.1\r\n");
    strcat(post_final, "Accept: */*\r\n");
    strcat(post_final, "Connection: close\r\n");
    strcat(post_final, "Content-Type: raw\r\n");
    strcat(post_final, "Content-Length: ");
    strcat(post_final, str);
    strcat(post_final, "\r\n");
    strcat(post_final, "Host: 47.95.111.217:10002\r\n\r\n");
    strcat(post_final, post_content);

    ret = write(sock, post_final, strlen(post_final));
    if (ret > 0){
        printf("%s\tAnswer submitted. \nAnswer: \t%s\n", now_time().c_str(), s.c_str());
        ++send_result_count;
    }
    close(sock);
}

void* receive_input_thread(void* args){
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

    int str_len;
    char message[BUF_SIZE];
    while (1){
        str_len = recv(sock, message, BUF_SIZE, 0);
        message[str_len] = 0;
        for (int i = 0; i < str_len; ++i)
            if (message[i] >= '0' && message[i] <= '9')
                q.push(message[i] - '0');
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
        if (q.size() > 0){
            int d = q.pop();
            mm->push(d);
            if (mm->check_valid(&answer))
                send_result(answer);
        }
    }
    return NULL;
}

void* solve_decomp_thread(void* args){
    ModMatrixDecomp *mmd;
    mmd = new ModMatrixDecomp(512);
    std::vector <int128> tmp;
    FILE *f = fopen("factors.txt", "r");
    // 每个m的质因子都存在文件factors.txt中，以0划分每个m的范围
    printf("Factors: \n");
    while (!feof(f)){
        int128 factor;
        tmp.clear();
        while (1){
            factor = read_from_file(f);
            if (factor == 0)
                break;
            tmp.push_back(factor);
        }
        mmd->add_m(tmp);
        for (auto i: tmp){
            print(i);
            printf(" ");
        }
        printf("\n");
    }
    string answer;
    while (1){
        if (q.size() > 0){
            int d = q.pop();
            mmd->push(d);
            if (mmd->check_valid(&answer))
                send_result(answer);
        }
    }
    return NULL;
}