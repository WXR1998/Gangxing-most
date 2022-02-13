#include <cstdio>
#include <cstring>
#include <string>

#include "utils.hpp"

using namespace std;

int main(){
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
            print_int128(i);
            printf(" ");
        }
        printf("\n");
    }
    string in = "2647654844351325008239048407883744826103779539505875833058544037772137424382952", answer;
    for (auto ch: in){
        mmd->push(ch - '0');
        printf("%c\n", in.c_str());
        mmd->print();
        printf("valid = %d\n", mmd->check_valid(&answer));
    }
    return NULL;
}