/* mipslabmain.c

  This file written 2015 by Axel Isaksson,
   modified 2015, 2017 by F Lundevall

   Latest update 2017-04-21 by F Lundevall

   For copyright and licensing, see file COPYING */

#include <stdint.h>	 /* Declarations of uint_32 and the like */
#include <pic32mx.h> /* Declarations of system-specific addresses etc */
#include "mipslab.h" /* Declatations for these labs */

int main(void)
{
	/*
  This will set the peripheral bus clock to the same frequency
  as the sysclock. That means 80 MHz, when the microcontroller
  is running at 80 MHz. Changed 2017, as recommended by Axel.
*/
	SYSKEY = 0xAA996655; /* Unlock OSCCON, step 1 */
	SYSKEY = 0x556699AA; /* Unlock OSCCON, step 2 */
	while (OSCCON & (1 << 21))
		;				  /* Wait until PBDIV ready */
	OSCCONCLR = 0x180000; /* clear PBDIV bit <0,1> */
	while (OSCCON & (1 << 21))
		;		  /* Wait until PBDIV ready */
	SYSKEY = 0x0; /* Lock OSCCON */

	/* Set up output pins */
	AD1PCFG = 0xFFFF;
	ODCE = 0x0;
	TRISECLR = 0xFF;
	PORTE = 0x0;

	/* Output pins for display signals */
	PORTF = 0xFFFF;
	PORTG = (1 << 9);
	ODCF = 0x0;
	ODCG = 0x0;
	TRISFCLR = 0x70;
	TRISGCLR = 0x200;

	/* Set up input pins */
	TRISDSET = (1 << 8);
	TRISFSET = (1 << 1);

	/* Set up SPI as master */
	SPI2CON = 0;
	SPI2BRG = 4;
	/* SPI2STAT bit SPIROV = 0; */
	SPI2STATCLR = 0x40;
	/* SPI2CON bit CKP = 1; */
	SPI2CONSET = 0x40;
	/* SPI2CON bit MSTEN = 1; */
	SPI2CONSET = 0x20;
	/* SPI2CON bit ON = 1; */
	SPI2CONSET = 0x8000;

	display_init();

	display_update();

labinit(); /* Do any lab-specific initialization */
IFScounter = 0;
//main menu
randomin = 0xBF800A10;
randomout = 0;
//Psudorandom function based on gamestart (button 4)
void get_random(void){
  randomout = *randomin%4;  
}

gamestate = 0;
//Game main menu
while(gamestate == 0){
int btn = getbtns();
	display_string(0, "   Welcome to");
	display_string(1, "   Apple Game");
	display_string(2, "  Press button 4");
	display_string(3, "    to start");
	display_update();
	if(btn&4){
	get_random();
	gamestate = 1;
	// T3CON = 0x8000;
	}
}

while (gamestate == 1)
{
	//Run the program every flag called by TMR2.
	if(InterruptFlag == 1){
	labwork();
		IFSCLR(0) = 0x100;
		InterruptFlag = 0;
	}

}

return 0;
}
