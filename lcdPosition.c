#include "lcdPosition.h"
#include "lcd.h"

#define L0 0x80
#define L1 0xC0
#define L2 0x90
#define L3 0xD0
#define CLR 0x01
#define ON 0x0F

void lcdPosition(char linha, char coluna){
    int i;
    for(i = 0 ; i < 16; i++){
        if(coluna == i){
            switch (linha){
                case 0:
                    lcdCommand(L0+i);
                    break;
                case 1:
                    lcdCommand(L1+i);
                    break;
                case 2:
                    lcdCommand(L2+i);
                    break;
                case 3:
                    lcdCommand(L3+i);
                    break;
                default:
                    lcdCommand(L0+i);
                    break;
            }
        }        
    }
}

