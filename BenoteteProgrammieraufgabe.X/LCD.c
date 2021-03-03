
/* some useful functions for controlling the LCD display */

#define I2C_ADDRESS             0x7C    // represents address 0x3E << 1

#include <xc.h>

typedef unsigned char u8;
typedef unsigned int u32;
void initI2C(){
    TRISBbits.TRISB7 = 0;
    TRISBbits.TRISB13 = 0;
    ANSELBbits.ANSB13 = 0;
    I2C3CON = 0x0; //clear I2C 3 Config Bits
    I2C3BRG = 27; //(24×10^6)÷(2×400×10^3)?1?(24×10^6×130×10^?9)÷2
    IFS2bits.I2C3MIF = 0;       //for safety clear Master Interupt Flag
    I2C3CONbits.ON = 1;         //Enable I2C 3 Module
}
void startI2C(){
    I2C3CONbits.SEN = 1; //create start condition
    while(!IFS2bits.I2C3MIF); //Wait till Master InterruptFlag ==1
    IFS2bits.I2C3MIF = 0; //Execute next I2C Operation
}
void stopI2C(){
    I2C3CONbits.PEN = 1; //create stop condition
    while(!IFS2bits.I2C3MIF); //Wait till Master InterruptFlag ==1
    IFS2bits.I2C3MIF = 0; //Execute next I2C Operation
}
int writeI2C(u8 I2Cmessage){
    I2C3TRN = I2Cmessage;
    while(!IFS2bits.I2C3MIF); //Wait till Master InterruptFlag ==1
    IFS2bits.I2C3MIF = 0; //Execute next I2C Operation
    return !I2C3STATbits.ACKSTAT; //1 NACK, 0ACK here 0 NACK and 1 ACK 
}
u8 sendI2C(u8 instr, u8 value) {
	u8 ret;
	
	startI2C();
	if ((ret = writeI2C(I2C_ADDRESS))) {
        ret = writeI2C(instr);
		ret = writeI2C(value);
    }
	stopI2C();
    delay_us(30);        // delay required according to data sheet
	return ret;
}

void clearLCD() {
    u8 mode = 0x00;
	sendI2C(mode,0x01);  // clear display
	delay_us(1200);      // required for clearing display
}

void homeLCD() {
	u8 mode = 0x00;      // writing single instruction
	sendI2C(mode,0x02);  // set cursor to home position
    delay_us(1200);      // required for home instruction
}

void initLCD() {
	u8 mode = 0x00;      // writing single instruction
    delay_us(50000);     // additional delay after power-on required
    sendI2C(mode,0x38);  // 8 bit interface, 2 lines, set basic table
    sendI2C(mode,0x39);  // set extended table
    sendI2C(mode,0x14);  // adjust internal oscillator frequency
    sendI2C(mode,0x74);  // set contrast (low part)
    sendI2C(mode,0x54);  // set contrast (high part), set power = 3V (booster)
    sendI2C(mode,0x6F);  // follower control 
    sendI2C(mode,0x0E);  // display on, show cursor
    clearLCD();
}

void writeLCD (char* str, u32 len) {
	u8 mode = 0x40;
	u32 i;
	if (len > 0) {
        for (i=0; i<len; i++)
            sendI2C(mode,str[i]);
    }
}

void setCursor(u8 row, u8 col) {
	u8 p;
	u8 mode = 0x80;
	
	if (row > 1) row = 0;
	if (col > 15) col = 0;
	if (row == 0) 
		p = 0x80 + col;
	else 
		p = 0xC0 + col;
	sendI2C(mode, (0x00 | p));
}
 
void testLCD() {
   	// first LCD test: print 1 line
    setCursor(0,0);             // set cursor position(row,col) 
    writeLCD("Hello World!",12);   // print string "Hello World" at set location
    delay_us(2000000);
	
    // second LCD test: print 2 lines  
    clearLCD();	
    setCursor(0,2);             // set cursor position(row, col) 
    writeLCD("First Line",10);        
    setCursor(1,2);             
    writeLCD("Second Line",11);   
    delay_us(2000000);
}