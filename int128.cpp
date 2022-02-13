#include <cstdio>

#include "int128.hpp"

int128 read_from_file(FILE *f){
    int128 ans = 0;
    char ch = fgetc(f);
    while (ch < '0' || ch > '9')
        ch = fgetc(f);
    while (ch >= '0' && ch <= '9'){
        ans = ans * 10 + ch - '0';
        ch = fgetc(f);
    }
    return ans;
}
void print(int128 d){
    if (d > 9)
        print(d / 10);
    putchar(d % 10 + '0');
}