#include <cstdio>
#include <cstring>
#include <string>

#include "utils.hpp"

using namespace std;

int main(){
    ModMatrixDecomp *mmd[4];
    for (int i = 0; i < 4; ++i)
        mmd[i] = new ModMatrixDecomp(MAX_N, D);
    int M = 0;
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
        mmd[M++]->add_m(tmp);
        for (auto i: tmp){
            print_int128(i);
            printf(" ");
        }
        printf("\n");
    }

    string in = "84028730551471784642070778470748611801571785766961335042180378032128915373293568", answer;
    for (int i = 0; i < in.size(); ++i){
        char ch = in[i];
        mmd[3]->push(ch - '0');

        if (i + 8 > in.size()){
            mmd[3]->print();
            printf("valid = %d\n", mmd[3]->check_valid(&answer));
        }
    }
    return NULL;
}