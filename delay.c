#include "delay.h"

void tempo(unsigned int x) {
    volatile unsigned char j, k;
    int a = x;
    while(a > 0){
        for (j = 0; j < 35; j++) {
            for (k = 0; k < 3; k++);
        }
        a--;
    }
}