#include <string>
#include <queue>
#include <vector>
#include <cstring>
#include <cstdio>

#include "utils.hpp"
#include "int128.hpp"

std::string now_time(){
    time_t now = time(0);
    char *dt = ctime(&now);
    dt[strlen(dt) - 1] = 0;
    return std::string(dt);
}

Counter::Counter(int size_limit): size_limit(size_limit){
    sum = 0;
    tail = count = 0;
    arr = new long long[size_limit];
    for (int i = 0; i < size_limit; ++i)
        arr[i] = 0;
}
void Counter::push(long long t){
    sum -= arr[tail];
    sum += t;
    count = std::min(count + 1, size_limit);
    arr[tail] = t;
    tail = (tail + 1) % size_limit;
}
int Counter::average(){
    if (count == 0) return 0;
    return int(sum / count);
}

Queue::Queue(){
    while (!unprocessed_data.empty())
        unprocessed_data.pop();
}
void Queue::push(int d){
    // pthread_mutex_lock(&mutex);
    unprocessed_data.push(d);
    // pthread_mutex_unlock(&mutex);
}
int Queue::pop(){
    int res = 0;
    // pthread_mutex_lock(&mutex);
    res = unprocessed_data.front();
    unprocessed_data.pop();
    // pthread_mutex_unlock(&mutex);
    return res;
}
int Queue::size(){
    return unprocessed_data.size();
}

SendQueue::SendQueue(){
    while (!results.empty())
        results.pop();
}
void SendQueue::push(std::string s){
    // if (pthread_mutex_lock(&mutex) != 0) exit(1);
    results.push(s);
    // if (pthread_mutex_unlock(&mutex) != 0) exit(1);
}
std::string SendQueue::pop(){
    // if (pthread_mutex_lock(&mutex) != 0) exit(1);
    std::string res = results.front();
    results.pop();
    // if (pthread_mutex_unlock(&mutex) != 0) exit(1);
    return res;
}
int SendQueue::size(){
    return results.size();
}

BigInteger::BigInteger(){
    len = 1;
    digits[0] = 0;
}
BigInteger::BigInteger(std::string s){
    len = s.size();
    for (int i = 0; i < s.size(); ++i)
        digits[len - i - 1] = s[i] - '0';
    while (digits[len - 1] == 0 && len > 1)
        --len;
}
BigInteger::BigInteger(int t){
    len = 0;
    do
    {
        digits[len++] = t % 10;
        t /= 10;
    } while (t > 0);
}

bool BigInteger::is_zero(){
    return len == 1 && digits[0] == 0;
}
std::string BigInteger::to_string(){
    std::string s;
    for (int i = 0; i < len; ++i)
        s = s + char(digits[len - i - 1] + '0');
    return s;
}
int BigInteger::compare(const BigInteger &a) const{
    // 大小比较，小于返回-1，大于返回1，等于返回0
    if (len < a.len)
        return -1;
    if (len > a.len)
        return 1;
    for (int i = len - 1; i >= 0; --i)
    {
        if (digits[i] < a.digits[i])
            return -1;
        if (digits[i] > a.digits[i])
            return 1;
    }
    return 0;
}
bool BigInteger::operator<(const BigInteger &a) const{
    return compare(a) == -1;
}

void BigInteger::shift_left(){
    // 乘以10
    if (is_zero())
        return;
    for (int i = len; i > 0; --i)
        digits[i] = digits[i - 1];
    digits[0] = 0;
    len++;
}
void BigInteger::mod(const BigInteger &mod){
    BigInteger res(*this);
    while (res.compare(mod) >= 0)
        res = res - mod;
    len = res.len;
    memcpy(digits, res.digits, sizeof(digits));
}
BigInteger BigInteger::operator+(const BigInteger &a) const{
    BigInteger res;
    res.len = std::max(len, a.len);
    int add = 0;
    for (int i = 0; i < res.len; ++i)
    {
        res.digits[i] = (i < len ? digits[i] : 0) + (i < a.len ? a.digits[i] : 0) + add;
        if (res.digits[i] >= 10)
        {
            add = 1;
            res.digits[i] -= 10;
        }
        else
            add = 0;
    }
    if (add)
        res.digits[res.len++] = add;
    return res;
}
BigInteger BigInteger::operator-(const BigInteger &a) const{
    // 减法，保证this >= a，即不会出现负数结果
    BigInteger res;
    res.len = len;
    int add = 0;
    for (int i = 0; i < res.len; ++i)
    {
        res.digits[i] = digits[i] - (i < a.len ? a.digits[i] : 0) + add;
        if (res.digits[i] < 0)
        {
            add = -1;
            res.digits[i] += 10;
        }
        else
            add = 0;
    }
    while (res.digits[res.len - 1] == 0 && res.len > 1)
        --res.len;
    return res;
}
BigInteger BigInteger::add_mod(const BigInteger &a, const BigInteger &mod) const{
    // (this+a) % mod, 确保a < mod
    BigInteger res;
    res = this->operator+(a);
    while (res.compare(mod) >= 0)
        res = res - mod;
    return res;
}
BigInteger BigInteger::sub_mod(const BigInteger &a, const BigInteger &mod) const{
    // (this-a) % mod, 确保 a < mod
    BigInteger res;
    if (compare(a) >= 0)
        res = this->operator-(a);
    else
        res = this->operator+(mod) - a;
    return res;
}

ModMatrix::ModMatrix(int M, int N, std::vector<BigInteger> m) : M(M), N(N){
    for (int i = 0; i < M; ++i)
        mods[i] = m[i];
    count = 0;
}

bool ModMatrix::check_valid(std::string *answer){
    // 检查当前矩阵里有没有模值为0的合法结果(子串不能纯为0)
    // 如果有，则返回true，并在answer中填写结果
    bool all_zero = true;
    for (int i = N - 1; i >= std::max(0, N - count); --i)
    {
        if (history[i] != 0)
            all_zero = false;
        for (int j = 0; j < M; ++j)
            if (!all_zero && mat[j][i].is_zero())
            {
                *answer = "";
                for (int k = i; k < N; ++k)
                    *answer = *answer + char(history[k] + '0');
                return true;
            }
    }
    return false;
}

void ModMatrix::push(int t){
    // 新进一个数，需要把矩阵整体左移一格，然后每个数乘10，再加上当前的这个数
    for (int i = std::max(0, N - 1 - count); i < N - 1; ++i)
    {
        history[i] = history[i + 1];
        for (int j = 0; j < M; ++j)
        {
            mat[j][i] = mat[j][i + 1];
            mat[j][i].shift_left();
            mat[j][i] = mat[j][i] + BigInteger(t);
            mat[j][i].mod(mods[j]);
        }
    }
    history[N - 1] = t;
    for (int j = 0; j < M; ++j)
    {
        mat[j][N - 1] = BigInteger(t);
        mat[j][N - 1].mod(mods[j]);
    }
    count++;
}

void ModMatrix::print(){
    printf("count = %d\nhistory:\t", count);
    for (int i = std::max(0, N - count); i < N; ++i)
        printf("%d", history[i]);
    printf("\n");
    for (int j = 0; j < M; ++j)
    {
        for (int i = std::max(0, N - count); i < N; ++i)
            printf("%s\t", mat[j][i].to_string().c_str());
        printf("\n");
    }
    printf("\n");
}

ModMatrixDecomp::ModMatrixDecomp(int N): N(N){
    M_factors = M = count = 0;
    current = N - 1;
}
int ModMatrixDecomp::indexof(int d){
    int res = d + 1 + current;
    return res >= N ? res - N : res;
}
void ModMatrixDecomp::add_m(std::vector <int128> factors){
    // 每个线程都有一个单独的ModMatrix，所以M=1，M_factors可能最大有3
    for (auto i: factors)
        mods[M_factors++] = i;
    ++M;
}
bool ModMatrixDecomp::check_valid(std::string *answer){
    // 检查当前矩阵里有没有模值为0的合法结果(子串不能纯为0)
    // 如果有，则返回true，并在answer中填写结果
    bool suffix_all_zero = true;
    for (int i = N - 1; i >= std::max(0, N - count); --i){
        int idx = indexof(i);
        if (history[idx] != 0) suffix_all_zero = false;
        if (suffix_all_zero) continue;
        
        // bool factors_all_zero = true;
        // for (int j = 0; j < M_factors; ++j)
        //     if (mat[j][idx])
        //         factors_all_zero = false;
        // if (factors_all_zero){
        if (zero_count[idx] == M_factors){
            *answer = "";
            for (int k = i; k < N; ++k)
                *answer = *answer + char(history[indexof(k)] + '0');
            return true;
        }
    }
    return false;
}
void ModMatrixDecomp::push(int t){
    // 新进一个数，需要把矩阵整体左移一格，然后每个数乘10，再加上当前的这个数
    current = (current + 1) % N;
    for (int i = std::max(0, N - 1 - count); i < N - 1; ++i){
        int idx = indexof(i);
        zero_count[idx] = 0;
        for (int j = 0; j < M_factors; ++j){
            mat[j][idx] = (mat[j][idx] << 3ll) + (mat[j][idx] << 1ll) + t;
            while (mat[j][idx] >= mods[j]) mat[j][idx] -= mods[j];
            if (!mat[j][idx])
                zero_count[idx]++;
        }
    }
    int idx = indexof(N - 1);
    history[idx] = t;
    zero_count[idx] = 0;
    for (int j = 0; j < M_factors; ++j){
        mat[j][idx] = t;
        while (mat[j][idx] >= mods[j]) mat[j][idx] -= mods[j];
        if (!mat[j][idx])
            zero_count[idx]++;
    }
    count++;
}

void ModMatrixDecomp::print(){
    printf("count = %d\nhistory:\t", count);
    for (int i = std::max(0, N - count); i < N; ++i)
        printf("%d", history[indexof(i)]);
    printf("\n");
    for (int j = 0; j < M_factors; ++j)
    {
        for (int i = std::max(0, N - count); i < N; ++i){
            // print_int128(mat[j][i]);
            if (mat[j][indexof(i)])
                printf("1");
            else
                printf("0");
        }
        printf("\n");
    }
}