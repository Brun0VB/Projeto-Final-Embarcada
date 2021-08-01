#include "stdio.h"
#include "lcd.h"

void imprime(char mensagem[48]){
    char i = 0;
    char fimmsg = 0;
    while(mensagem[i]!= '\0'){
        fimmsg++;
        i++;
    }
    for (i = 0; i < fimmsg; i++) {
        lcdData(mensagem[i]);
    }
}