/*
 * File:   main.c
 * Author: Bruno Vilela Batista
 *
 * Created on 25 de Julho de 2021, 16:08
 
 * 2020003845 - Bruno Vilela Batista

Tema: Aparelho Micro-ondas

Funcionalidades a serem utilizadas LEDs, 
 * display LCD, displays de 7-seg, teclas, potenciômetros, 
 * Relés, Ventoinha (cooler) 

O PICSimLab funcionará como um aparelho micro-ondas,
 * com opções para o preparo de alimentos, escolhidas pelo usuário, 
 * e realizará a simulação de execução dos mesmos.
 */

#define CLR 0x01
#define ON 0x0F


#include <pic18f4520.h>
#include <stdlib.h>
#include "lcd.h"
#include "lcdPosition.h"  
#include "stdio.h"
#include "keypad.h"
#include "bits.h"
#include "ssd.h" 
#include "delay.h"
#include "io.h"
#include "itoa.h"
#include "adc.h"



void ligarele1(){
    TRISCbits.TRISC0=0;
    PORTCbits.RC0=1;
}

void ligarele2(){
    TRISEbits.TRISE0=0;
    PORTEbits.RE0=1;
}

void desligarele1(){
    TRISCbits.TRISC0=0;
    PORTCbits.RC0=0;
}

void desligarele2(){
    TRISEbits.TRISE0=0;
    PORTEbits.RE0=0;
}

void desligaDisplays (){
    digitalWrite(PIN_A2, LOW);
    digitalWrite(PIN_A3, LOW);
    digitalWrite(PIN_A4, LOW);
    digitalWrite(PIN_A5, LOW);
}

void Inicia(){
    unsigned int i,j,k,m = 0;
    unsigned char oldtrisd,tecla = 16,ref2 = 0b00000001;//primeiro led


    oldtrisd = TRISD;
    lcdCommand(CLR);
    TRISD = 0x00;  
    PORTD = 0x00;
    imprime("ligando.");
    for(k=0;k<8;k++){
        
        PORTD = ref2;
        for (i = 0; i < 200; i++) {
            for (j = 0; j < 200; j++);
            for (j = 0; j < 200; j++);
        }
        ref2 = ref2 << 1;
        ref2 += 1;
        if(m++ == 2) {
            m=0;
            lcdCommand(CLR); 
        }
        if(m==0)imprime("ligando.");
        lcdPosition(0,0);
        if(m==1)imprime("ligando..");
        lcdPosition(0,0);
        if(m==2)imprime("ligando..."); 
    }
    tempo(760);
   
}

float potencia(){ 
    unsigned char potchar[3],tmp,tecla = 16,oldtrisd;
    float faixa;
    char str[6];
    double potdob;
    
    lcdCommand(CLR);
    imprime("Potencia");
    lcdPosition(3,0);
    imprime("Confirmar (*)");
    adcInit();
    for (;;) {
        kpDebounce();
        tmp = (adcRead(0)*10) / 204;
        lcdCommand(0xC0 + 6);
        itoa(tmp, str);
        potchar[0] = str[3];
        potchar[0] = '.';
        potchar[0] = str[4];
        potdob = atof(potchar);
        if(potdob >= 0 && potdob <2){
            lcdPosition(1,7);
            imprime("BAIXA"); 
            faixa = 1;
        }else if(potdob >= 2 && potdob <4){
            lcdPosition(1,7);
            imprime("MEDIA");
            faixa = 0.5;
        }else if(potdob >= 4 && potdob <5){
            lcdPosition(1,7);
            imprime("ALTA ");
            faixa = 0.25;
        }       
        tempo(1);       
        if (kpRead() != tecla) {
            tecla = kpRead();
            if (bitTst(tecla, 0)) {
                ligarele1();
                PORTD = 0x00;
                TRISD = oldtrisd;
                return faixa;
            }
        }
    }
}
void temporizador(unsigned long int valor){
    volatile int tempo,i;
    unsigned long int cont = valor; //mudança no tipo para que cont suporte valores               //mais altos
    for (i = valor;i>0;i--) {
        cont--;
        ssdDigit(((cont / 100) % 10), 3); //1s
        ssdDigit(((cont / 1000) % 6), 2); //10s //digitos de 0 a 6 apenas(%6))
        ssdDigit(((cont / 6000) % 10), 1);//60s
        ssdDigit(((cont / 60000) % 10) , 0);//600s
        ssdUpdate();
        for (tempo = 0; tempo < 575; tempo++); // 575(sem flicker/10s +_= 9,7s  ))   
    }
}

int verfligamento(){
    unsigned char tecla;
    kpDebounce();
    if(kpRead() != tecla){
        tecla = kpRead();
        if(bitTst(tecla,0)){
            return tecla;
        }else return 0;   
    }
}

void menuPrincipal(){
    unsigned char tecla,cont;
    if(!cont){
        imprime("Ligar");
        imprime(" PRESS *");        
    }else {
        imprime("Voltar ao menu");
        lcdPosition(1,3);
        imprime("PRESS *");
    }
    for(;;){
       kpDebounce();  
        if (kpRead() != tecla){
            tecla = kpRead();
            if(bitTst(tecla, 0)){
                if(!cont){
                    Inicia(); 
                    cont = 1;
                }
                ligarele1();
                lcdCommand(CLR);
                lcdCommand(0x80);
                imprime("Predefinidos");
                lcdCommand(0xC1);
                imprime("1)Arroz");
                lcdCommand(0x91);
                imprime("4)Congelados");
                lcdCommand(0xD1);
                imprime("7)Pipoca");//
                tempo(5000);
                lcdCommand(CLR);
                lcdCommand(0x81);
                imprime("2)Carnes");
                lcdCommand(0xC1);
                imprime("5)Legumes");
                lcdCommand(0x91);
                imprime("8)Massas");
                lcdCommand(0xD1);
                imprime("0)Desligar");//
                break;
            }
        }
    }
}

void ligarcooler(){

    TRISC = 0x01;
    bitSet(PORTC, 2);

}

void desligarcooler(){

    TRISC = 0x01;
    bitClr(PORTC, 2); 

}

void main(void) {
    unsigned char i, tecla;  
    float potescolhida;
    lcdInit();    
    ssdInit();
    kpInit();  
    menuPrincipal();
    for(;;){    
        kpDebounce();
        if (kpRead() != tecla){
            tecla = kpRead();
            if(bitTst(tecla, 3)){ 
                potescolhida = potencia();
                lcdCommand(CLR);
               
                desligarele1();
                ligarele2();
                ligarcooler();  
                
                
                imprime("Esquentando");
                lcdPosition(1,5);
                imprime("arroz...");

                lcdPosition(3,0);
                imprime("PORTAS FECHADAS");
     
                temporizador(6000*potescolhida);
                
                desligarcooler();
                desligarele2();
                ligarele1();
               
                
                lcdCommand(CLR);
                imprime("Alimento pronto");
                lcdPosition(3,0);
                imprime("PORTAS ABERTAS");
                
                desligaDisplays();
                
                tempo(8000);
                break;
            }
            if(bitTst(tecla, 2)){ 
                potescolhida = potencia();
                lcdCommand(CLR); 
                
                desligarele1();
                ligarele2();
                ligarcooler();  
                
                imprime("Descongelando");

                lcdPosition(3,0);
                imprime("PORTAS FECHADAS");
     
                temporizador(30000*potescolhida);
                
                desligarcooler();
                desligarele2();
                ligarele1();
               
                
                lcdCommand(CLR);
                imprime("Alimento pronto");
                lcdPosition(3,0);
                imprime("PORTAS ABERTAS");
                
                desligaDisplays();
                
                tempo(8000);
                break;
            }
            if(bitTst(tecla, 1)){   
                potescolhida = potencia();
                lcdCommand(CLR);
               
                desligarele1();
                ligarele2();
                ligarcooler();  
                
                
                imprime("Estourando");
                lcdPosition(1,5);
                imprime("pipoca...");

                lcdPosition(3,0);
                imprime("PORTAS FECHADAS");
     
                temporizador(6250*potescolhida);
                
                desligarcooler();
                desligarele2();
                ligarele1();
               
                
                lcdCommand(CLR);
                imprime("Alimento pronto");
                lcdPosition(3,0);
                imprime("PORTAS ABERTAS");
                
                desligaDisplays();
                
                tempo(8000);
                break;
            }
            if(bitTst(tecla, 7)){   
                potescolhida = potencia();
                lcdCommand(CLR);
               
                desligarele1();
                ligarele2();
                ligarcooler();  
                
                
                imprime("Esquentando");
                lcdPosition(1,5);
                imprime("carne...");

                lcdPosition(3,0);
                imprime("PORTAS FECHADAS");
     
                temporizador(15000*potescolhida);
                
                desligarcooler();
                desligarele2();
                ligarele1();
               
                
                lcdCommand(CLR);
                imprime("Alimento pronto");
                lcdPosition(3,0);
                imprime("PORTAS ABERTAS");
                
                desligaDisplays();
                
                tempo(8000);
                break;
            }
            if(bitTst(tecla, 6)){ 
                potescolhida = potencia();
                lcdCommand(CLR);
               
                desligarele1();
                ligarele2();
                ligarcooler();  
                
                
                imprime("Esquentando");
                lcdPosition(1,5);
                imprime("legumes...");

                lcdPosition(3,0);
                imprime("PORTAS FECHADAS");
     
                temporizador(3000*potescolhida);
                
                desligarcooler();
                desligarele2();
                ligarele1();
               
                
                lcdCommand(CLR);
                imprime("Alimento pronto");
                lcdPosition(3,0);
                imprime("PORTAS ABERTAS");
                
                desligaDisplays();
                
                tempo(8000);
                break;
            }
            if(bitTst(tecla, 5)){   
                potescolhida = potencia();
                lcdCommand(CLR);
               
                desligarele1();
                ligarele2();
                ligarcooler();  
                
                
                imprime("Esquentando");
                lcdPosition(1,5);
                imprime("massa...");

                lcdPosition(3,0);
                imprime("PORTAS FECHADAS");
     
                temporizador(1000*potescolhida);
                
                desligarcooler();
                desligarele2();
                ligarele1();
               
                
                lcdCommand(CLR);
                imprime("Alimento pronto");
                lcdPosition(3,0);
                imprime("PORTAS ABERTAS");
                
                desligaDisplays();
                
                tempo(8000);
                break;
            }
            if(bitTst(tecla,4)){
                desligarele1();
                desligarele2(); 
                desligaDisplays();
                PORTD = 0x00;
                lcdCommand(CLR);
               
                imprime("**Desligou**");
                tempo(3000);
                lcdCommand(CLR);   
                imprime("Ligar");
                imprime(" PRESS *");                 
                while(verfligamento() == 0);
                break;
            }
        }
    }
}