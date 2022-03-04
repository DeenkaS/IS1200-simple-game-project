/* mipslabwork.c

   This file written 2015 by F Lundevall
   Updated 2017-04-21 by F Lundevall

   This file should be changed by YOU! So you must
   add comment(s) here with your name(s) and date(s):

   This file modified 2017-04-31 by Ture Teknolog

   For copyright and licensing, see file COPYING */
//This file was last modified 2022-03-04 by Jonas Dåderman & Deniel Saskin
//
#include <stdint.h>  /* Declarations of uint_32 and the like */
#include <pic32mx.h> /* Declarations of system-specific addresses etc */
#include "mipslab.h" /* Declatations for these labs */
#include <stdio.h>
#include <string.h>

int8_t field[128 * 4] = {255};
int8_t xyarray[4][128] = {255};

int8_t basket[8] = {0xC3, 0xC3, 0xC3, 0xC3, 0xFF, 0x7e, 0x3d, 0x00};

int x = 1;
int points;

int rad;
int kolumn;
int collision;


int mytime = 0x1337;

char textstring[] = "text, more text, and even more text!";


/* Interrupt Service Routine */
void user_isr(void)
{
  IFSCLR(0) = 0x100;
  InterruptFlag = 1;
  return;
}

/* Lab-specific initialization goes here */
void labinit(void)
{
  // Intialize TRISE
  volatile int *set_e = (volatile int *)0xbf886100; // make LED
  *set_e & 0xffffff00;

  // Initialize TRISD 5-11 (buttons 4,3 & 2)
  // make button go input
  TRISD = TRISD & 0x0fe0;

  // intialize timer2
  T2CON = 0x0; // Stop the cock
  T3CON = 0x0; // Stop the clock

  TMR2 = 0x0;
  PR2 = (80000000 / 256) / 25; // 100ms
  T2CON = 0x8070;              // Set prescaling to 1:256
  //TMR3 used for random function
  TMR3 = 0x0;
  PR3 = (80000000 / 256) / 10;
  T3CON = 0x8070;              // Set prescaling to 1:256

  // initialize interrupts for timers 2 and 3 & inbterrupt priority
  IEC(0) = 0x1100;
  IPC(2) += 0x10;
  enable_interrupt();


  return;
}



void moveleft(void) //moves the basket to the left and clears it's position
{
  int i = 0;
  if ((x < 3) && (IFScounter >= 2))   //Separate timer for 
  {
    IFScounter = 0;
    x += 1;
  for (i; i < 8; i++)                 //Redraw basket at new location
  {
    xyarray[x - 1][119 + i] = 0;
  }
  }
}

void moveright(void)  //moves the basket to the right and clears it's previous position
{
  int i = 0;
  if ((x > 0) && (IFScounter >= 2))
  {
    IFScounter = 0;
    x -= 1;
  for (i; i < 8; i++)           //Redraw basket at new location
  {

    xyarray[x + 1][119 + i] = 0;
  }
  }
}

//Display gameover (relative to checkiflost)
void duesemst(void)
{
  int j = 0;
  int k = 0;
  for (j; j < 4; j++)
  {
    for (k; k < 128; k++)
    {
      xyarray[j][k] = 0;
    }

    display_update();
    display_string(0,"   Game Over");
    display_string(1,"   points:");
    display_string(2,   itoaconv(points));  //How to move point display to the middle??
    display_string(3, "       ");
    display_update();
    delay(10000);
    gamestate = 0;
  }
}
//Checks if the apple reaches the bottom 4 pixelrows. If yes, duesemst is called
void checkiflost(void){
  int i = 0;
  for (i; i < 4; i++)
  {
  if (xyarray[i][127] == 0x3c || xyarray[i][126] == 0x3c || xyarray[i][125] == 0x3c ||xyarray[i][124] == 0x3c)
    {
      duesemst();
    }
  
  }
}
  void get_random(void){
  randomout = *randomin%4;
}
  //Apple falling from the sky & manage game speed with switches
void drawApple(void)
{
  int i;
  int pos = randomout;
  
  for (i = 0; i < 9; i++)
  {
    xyarray[pos][apple1[10] - i] = apple1[i]; //int8_t apple1[9] = {0x00, 0x00, 0x3C, 0x3C, 0x3C, 0x3C, 0x0, 0x00, 0};
  }
  //Medium
  if(getsw() == 1){
  apple1[10] += 2;
  }
  //Hard
  if(getsw() == 3){
  apple1[10] += 3;
  }
  //Extreme
  if(getsw() > 3){
    apple1[10] += 4;
  }
  //Easy
  else{
    apple1[10]++;
  }
  
  if (apple1[10] == 128)
  {
    apple1[10] = 4;
    // gameover();
  }
}

//Checks if apple touches the basket, if yes: redraw apple at new top location & increment points.
void checkCollision(void)
{
  int i;
  collision = xyarray[x][118]+xyarray[x][119];

  if (collision != ~0x3c) //Evil bithack
  {

    points += 1;
    for(i = 114; i < 119; i++){
    xyarray[x][i] = 0;
    }
    apple1[10] = 4;

    get_random();
    
  }
  

}

/* This function is called repetitively from the main program */
//Labwork is used to call functions above, draw the basket and display the image.
void labwork(void)
{
  int i;
  int k = 0;
  if(IFS(0)&0x1000){
    IFScounter++;
    IFSCLR(0) = 0x1000;
  } 

  checkiflost();
  int buttons = getbtns();  //Get value of buttons
  drawApple();
  
  if (buttons & 1)
  {
    moveleft();    
  }

  if (buttons & 2)
  {
    moveright();
  }

//Draw basket
for(i = 0; i < 8; i++){
  xyarray[x][119+i] = basket[i];
}

checkCollision();
//---------------------------------------------------------------------------------------
//VERY IMPORTANT: MAPS XYARRAY TO FIELD 
for(kolumn = 0; kolumn < 4; kolumn++)
{
  for(rad = 0; rad < 128; rad++)
  {
    field[k]=xyarray[kolumn][rad];
    k++;
  }
}
display_image(0, field);

}