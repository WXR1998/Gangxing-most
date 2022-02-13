#include <string>
#include <deque>
#include <vector>

const int BUF_SIZE = 8;
const int MAX_N = 512;
const int MAX_M = 3;

std::string now_time();

class Queue{
private:
    pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
    std::deque <int> unprocessed_data;
public:
    Queue();
    void push(int d);
    int pop();
    void print();
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
    long long mat[MAX_M * 5][MAX_N];
    // M: 原始的M的数量     M_factor: 所有M的质因子的数量
    int M, M_factors, N, count;
    long long mods[MAX_M * 5];
    int history[MAX_N];
    // 一个原始的M可以被分解成多个质因子，mods_mapping[i]是原始m[i]对应的质因子的下标列表
    std::vector <int> mods_mapping[MAX_M];
public:
    ModMatrixDecomp(int N);

    void add_m(std::vector <long long> factors);
    bool check_valid(std::string *answer);
    void push(int t);
};