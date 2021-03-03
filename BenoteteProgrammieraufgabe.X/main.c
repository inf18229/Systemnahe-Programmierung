/*
 * Benotete Programmieraufgabe
 * Author: Stefan Maier
 */

#include <stdint.h>
#include <xc.h>
#include <sys/attribs.h>
#include "sinus.h"

void SYSTEM_Initialize(void);
unsigned int currenCounter = 0;

char hours = 0;
char minutes = 0;
char seconds = 0;
char mseconds = 0;

char stoppwatch_enable=0;
char* digitsChars[10] = {"0","1","2","3","4","5","6","7","8","9"};
char roundStopFlag=0;
char menueEntryFlag=0;
char stopEnable=0;
char timerEnable=0;
char timerSelectionFlag=0;
char pottiInputFlag = 0;

void displayMenu(){
    switch(menueEntryFlag){
        case 0:
            setCursor(0,0);             // set cursor position(row, col) 
            writeLCD("> Stoppuhr",10);        
            setCursor(1,0);             
            writeLCD("  Timer",7);   
            break;
        case 1:
            setCursor(0,0);             // set cursor position(row, col) 
            writeLCD("  Stoppuhr",10);        
            setCursor(1,0);             
            writeLCD("> Timer",7);   
            break;
    } 
}
void displayStopwatch(){
    setCursor(1,0);             // set cursor position(row, col) 
    writeLCD("  Stoppuhr",10);
}
void displayTimer(){
    setCursor(1,0);             // set cursor position(row, col) 
    writeLCD("  Timer",7);
}
//void roundStopp(int h, int m, int s, int ms){
//    /*
//     * Create current Value of timer in Round
//     */
//    setCursor(0, 0);
//    writeLCD(" ", 1);
//    setCursor(0, 1);
//    writeLCD(" ", 1);
//    setCursor(0, 2);
//    writeLCD(digitsChars[h/10], 1);
//    setCursor(0, 3);
//    writeLCD(digitsChars[h%10], 1);
//    setCursor(0, 4);
//    writeLCD(":", 1);
//    setCursor(0, 5);
//    writeLCD(digitsChars[m/10], 1);
//    setCursor(0, 6);
//    writeLCD(digitsChars[m%10], 1);
//    setCursor(0, 7);
//    writeLCD(":", 1);
//    setCursor(0, 8);
//    writeLCD(digitsChars[s/10], 1);
//    setCursor(0, 9);
//    writeLCD(digitsChars[s%10], 1);
//    setCursor(0, 10);
//    writeLCD(",", 1);
//    setCursor(0, 11);
//    writeLCD(digitsChars[ms / 10], 1);
//    setCursor(0, 12);
//    writeLCD(digitsChars[ms % 10], 1);
//     
//}
void displayLCD(){
    
    /*
     * Create centered Output of current values from hours,minutes, seconds, mseconds
     * Format hh:mm:ss,msms
     * To create to decimals I use modulo operation to calculate second ditit
     */
    if(roundStopFlag==0){
    setCursor(0, 0);
    writeLCD(" ", 1);
    setCursor(0, 1);
    writeLCD(" ", 1);
    setCursor(0, 2);
    writeLCD(digitsChars[hours/10], 1);
    setCursor(0, 3);
    writeLCD(digitsChars[hours%10], 1);
    setCursor(0, 4);
    writeLCD(":", 1);
    setCursor(0, 5);
    writeLCD(digitsChars[minutes/10], 1);
    setCursor(0, 6);
    writeLCD(digitsChars[minutes%10], 1);
    setCursor(0, 7);
    writeLCD(":", 1);
    setCursor(0, 8);
    writeLCD(digitsChars[seconds/10], 1);
    setCursor(0, 9);
    writeLCD(digitsChars[seconds%10], 1);
    setCursor(0, 10);
    writeLCD(",", 1);
    setCursor(0, 11);
    writeLCD(digitsChars[mseconds / 10], 1);
    setCursor(0, 12);
    writeLCD(digitsChars[mseconds % 10], 1);
    }   
}

void increment(){
    /*
     * Increment Routine ist Zeitkritisch und wird in einer Interruptroutine aufgerufen
     */
    asm volatile("li $t0,100 \n\t"
                 "li $t1, 60 \n\t"
                 "addi %0,%0,1 \n\t"
                 "bne %0,$t0,1f \n\t"
                 "nop \n\t"
                 "move %0,$zero \n\t"
                 "addi %1,%1,1 \n\t"
                 "bne %1,$t1,2f \n\t"
                 "nop \n\t"
                 "move %1,$zero \n\t"
                 "addi %2,%2,1 \n\t"
                 "bne %2,$t1,3f \n\t"
                 "nop \n\t"
                 "move %2,$zero \n\t"
                 "addi %3,%3,1 \n\t"
                 "bne %3,$t0,4f \n\t"
                 "nop \n\t"
                 "move %3,$zero \n\t"
                 "4: "
                 "3: "
                 "2: "
                 "1: "
    :"+r"(mseconds),"+r"(seconds),"+r"(minutes),"+r"(hours)
    :
    :"t0","t1");
//    mseconds++;
//    if(mseconds >=100){
//        mseconds = 0;
//        seconds++;
//        if(seconds>=60){
//            seconds = 0;
//            minutes++;
//            if(minutes>=60){
//                minutes = 0;
//                hours++;
//                if(hours>=100){
//                    hours = 0;
//                }
//            }
//        }
//    }
}
void Timer(){
    minutes++;
    displayLCD();
}
void resetTimer(){
    hours = 0;
    minutes = 0;
    seconds = 0;
    mseconds = 0; 
    displayLCD();
}
void notifyBeep(){
    T1CONbits.ON = 0;   // Stop timer 1
    T2CONbits.ON=1;
    LATAbits.LATA7=1;
    delay_us(1000000);
    LATAbits.LATA7=0;
    T2CONbits.ON = 0;
    DAC1CONbits.DACDAT = 0; // Explicit low the output otherwise the speaker makes noise
    resetTimer();

}
void decrement(){
    mseconds--;
    if(mseconds<0){
        mseconds=99;
        seconds--;
        if(seconds<0){
            seconds=59;
            minutes--;
            if(minutes<0){
                notifyBeep();
            }
            
        }
    }
}
void nextOutput(){ //von Aufgabe 3
    asm volatile (  "li $t0, 100 \n\t"  //load constant 100 for  <99
                    "la $s1, sinus \n\t"       //load sinus
                    "bne %0, $t0, 1f \n\t"
                    "nop \n\t"
                    "li %0, 0 \n\t"
                    "1: "
                    "add $s1, $s1, %0 \n\t"
                    "lb $s2, 0($s1) \n\t"
                    "move %3, $s2 \n\t"
                    "addi %0, %0, 1"
                    :"+r"(currenCounter),"+r"(DAC1CONbits.DACDAT)
                    :
                    :"s1","s2","t0");
}
void __ISR(_TIMER_1_VECTOR,IPL3SRS)Timer1Handler(void)  //For Perfomance use shadow register set for interrupt handling
{
    if(stoppwatch_enable == 1){    //if stopwatch is not enabled then start decrementing the timer value
        decrement();
    }
    else if(stoppwatch_enable == 0){   // stopwatch is enabled start incrementing the timer
        increment();
    }
    displayLCD();
    IFS0bits.T1IF = 0;  // clear the Timer 2 interrupt status
}
void __ISR(_TIMER_3_VECTOR,IPL4SOFT)Timer2Handler(void) //Use Stack here Performance not needed
{
    IFS0bits.T3IF=0; // Be sure to clear the Timer 3 interrupt status
    nextOutput();
}

void readADC(){
    AD1CON1SET = 0x0002;
    delay_us(100);
    AD1CON1CLR = 0x0002;
    while(AD1CON1bits.DONE==0);
    switch(timerSelectionFlag){
        case 0:
            setCursor(1, 9);
            writeLCD("[hh]",4);
            hours = (char)((float)ADC1BUF0*0.097);
            break;
        case 1:
            setCursor(1, 9);
            writeLCD("[mm]",4);
            minutes = (char)((float)ADC1BUF0*0.058);
            break;
        case 2:
            setCursor(1, 9);
            writeLCD("[ss]",4);
            seconds = (char)((float)ADC1BUF0*0.058);
            break;
        case 3:
            setCursor(1, 9);
            writeLCD("[xx]",6);
            mseconds = (char)((float)ADC1BUF0*0.097);
            break;
    }
    displayLCD();
}


void setup() { 
    
    SYSTEM_Initialize();
	//Timer 1 Configuration
    T1CON = 0x0;        // Clear all Configration bits
    TMR1 = 0x0;         
    PR1 = 30000;           // period Register is set to 41,667ns * 8 * 30000 = 0,01s
    T1CONbits.TCS = 0;   // Clock source PBCLK (24 MHZ)
    T1CONbits.TCKPS = 0b01;    //prescaler 8
    IFS0bits.T1IF = 0;  // clear timer 1
    IPC4bits.T1IP = 3;  // interrupt priority 3
    IPC4bits.T1IS = 1;  // Subpriority to 1
    IEC0bits.T1IE = 1;  // enable interrupt for timer 1
    PRISSbits.PRI3SS = 1; // enable shadow register set for timer 1
    T1CONbits.ON = 0;   // Let timer 1 be disabled
    
    //Button S1 Configuration
    TRISBbits.TRISB9 = 1; //enable S1 for input
    TRISCbits.TRISC10 = 1; //enable S2 for input
    TRISCbits.TRISC4 = 1; //enable S3 for input
    TRISAbits.TRISA7 = 0;   //Enable Debug LED on A7 Pin
    
    //DAC Configuration
    TRISBbits.TRISB14=0;
    DAC1CONbits.DACOE=0b1;
    DAC1CONbits.REFSEL=0b11;
    DAC1CONbits.ON=0b1;
    
    //Timer COnfiguration
    T2CON=0x0;
    TMR2=0;
    T2CONbits.T32=1;
    PR2=190;
    T2CONbits.TCKPS=0b00;
    IFS0bits.T3IF = 0;  // Clear interrupt flag for timer 2
    IPC4bits.T3IP = 4;  // Interrupt priority 4
    IPC4bits.T3IS = 1;  // Sub-priority 1
    IEC0bits.T3IE = 1;  // Enable Timer 2 Interrupt
    
    T2CONbits.ON=0;
    
    //ADC Poti Configration
    TRISCbits.TRISC8=1;
    ANSELCbits.ANSC8=1;
    AD1CHSbits.CH0SA=14;
    AD1CHSbits.CH0NA=0;
    AD1CON3bits.ADRC=0;
    AD1CON3bits.ADCS=0b11111111;
    
    AD1CON1bits.ON=1;
    
    //LCD Setup
    initI2C();
    initLCD();
    displayMenu();
}

void loop() {

    while(1){
        if(stopEnable==0 && timerEnable ==0){ //If Nothing is selected show menu
            displayMenu();
        }
        if(timerEnable==1 && menueEntryFlag == 1 && pottiInputFlag == 1){
            if(T1CONbits.ON == 0){
                readADC();      
            }
        }
        if(PORTBbits.RB9 == 0){
            if(stopEnable==0 && timerEnable ==0){ //User selects something if nothing is already selected
                menueEntryFlag=!menueEntryFlag; //User selects something if nothing is already selected
            }
            if((stopEnable==1 || timerEnable==1) && T1CONbits.ON == 0){ //If Stoptimer is selected start it
                T1CONbits.ON = 1;   // Start timer 1
            }
            else if ((stopEnable==1 || timerEnable==1) && T1CONbits.ON == 1){   //If Stoptimer is selected 
                T1CONbits.ON = 0;   // Stop timer 1
            }
        }
        delay_us(100);
        while(!PORTBbits.RB9);      //entprellen von S1
        
        if(PORTCbits.RC10 == 0 ){ 
            pottiInputFlag = 0;
            if(menueEntryFlag==1 && timerEnable==1 && pottiInputFlag == 0){
                Timer();
            }
            
        }
        delay_us(100);
        while(!PORTCbits.RC10);     //entprellen von S2
        
        if(PORTCbits.RC4 == 0){
            if(stopEnable==0 && timerEnable ==0){
                switch(menueEntryFlag){
                    case 0:
                        stopEnable=1;
                        timerEnable=0;
                        clearLCD();
                        displayStopwatch();
                        displayLCD();
                        break;
                    case 1:
                        stopEnable=0;
                        timerEnable=1;
                        stoppwatch_enable = 1;
                        clearLCD();
                        displayTimer();
                        displayLCD();
                        break;
                }
            }
            if(menueEntryFlag==0 && stopEnable==1 && T1CONbits.ON == 0){
                resetTimer();
            }
            else if(menueEntryFlag==0 && stopEnable==1){
                if(roundStopFlag==0){
                    //roundStopp(hours,minutes,seconds,mseconds);
                    roundStopFlag = 1;      //DisplayLCD function will not be executed
                }
                else{
                    roundStopFlag = 0;
                }
            }
            if(menueEntryFlag == 1 && timerEnable == 1){
                pottiInputFlag = 1;
                timerSelectionFlag++;
                if(timerSelectionFlag>3){
                    timerSelectionFlag=0;
                }
            }
        }
        delay_us(100);
        while(!PORTCbits.RC4);  //entprellen von S3
    }
}

int main(void) {
    setup();
    loop();
}