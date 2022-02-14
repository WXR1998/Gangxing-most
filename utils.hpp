#include <string>
#include <queue>
#include <vector>

#include "int128.hpp"

const int BUF_SIZE = 1;
const int MAX_N = 512;
const int MAX_M = 4;

std::string now_time();

class Queue{
private:
    pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
    std::queue <int> unprocessed_data;
public:
    Queue();
    void push(int d);
    int pop();
    int size();
};

class SendQueue{
private:
    pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
    std::queue <std::string> results;
public:
    SendQueue();
    void push(std::string);
    std::string pop();
    int size();
};

// 只有自然数的高精度运算，支持加减法和大小比较
class BigInteger{
public:
    int digits[MAX_N];
    int len;

    BigInteger();
    BigInteger(std::string s);
    BigInteger(int t);

    bool is_zero();
    std::string to_string();
    int compare(const BigInteger &a) const;
    bool operator <(const BigInteger &a) const;

    void shift_left();
    void mod(const BigInteger &mod);
    BigInteger operator +(const BigInteger &a) const;
    BigInteger operator -(const BigInteger &a) const;
    BigInteger add_mod(const BigInteger &a, const BigInteger &mod) const;
    BigInteger sub_mod(const BigInteger &a, const BigInteger &mod) const;
};

/*
    矩阵大小为M行N列，M行表示M个除数，N列
    第i列表示：当前这个数字流之中，最新的i位数模这个m得到的余数
    每次进来一个数，就先把矩阵左移一格，然后乘10，再加上当前的这个数
*/
class ModMatrix{
private:
    BigInteger mat[MAX_M][MAX_N];
    int M, N, count;
    // count: 已经进了多少个数
    BigInteger mods[MAX_M];
    int history[MAX_N]; // 历史进来的数之列表
public:
    ModMatrix(int M, int N, std::vector<BigInteger> m);

    bool check_valid(std::string *answer);
    void push(int t);
    void print();
};

class ModMatrixDecomp{
private:
    int128 mat[MAX_M][MAX_N];
    // M: 原始的M的数量     M_factor: 所有M的质因子的数量   current: 采用滚动数组的方式，记录当前最后一列的下标
    int M, M_factors, N, count, current;
    // mods[0..3]分别表示mod*1, mod*2, mod*4, mod*8
    int128 mods[MAX_M][4];
    int history[MAX_N];
    // 一个原始的M可以被分解成多个质因子，mods_mapping[i]是原始m[i]对应的质因子的下标列表
    std::vector <int> mods_mapping[MAX_M];
public:
    ModMatrixDecomp(int N);

    // 因为滚动数组的原因，给定需求的下标位置，求真实数组物理地址
    int indexof(int d);
    void add_m(std::vector <int128> factors);
    bool check_valid(std::string *answer);
    void push(int t);
    void print();
};