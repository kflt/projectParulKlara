/* Code from Lab3
   By Parul Khattar & Klara Fält
   2021
*/

#include <stdint.h>
#include <pic32mx.h>
#include "mipslab.h"

/* Timer period constant */
#define TMR2PERIOD  ((80000000 / 256) / 10)

volatile int* portD = (volatile int*) 0xbf8860D0;
volatile int* portF = (volatile int*) 0xbf886150;

/* Code originally from Lab 3 */
void io_init(void){
    /*
    This will set the peripheral bus clock to the same frequency
    as the sysclock. That means 80 MHz, when the microcontroller
    is running at 80 MHz. Changed 2017, as recommended by Axel.
    */
    SYSKEY = 0xAA996655;        /* Unlock OSCCON, step 1  */
    SYSKEY = 0x556699AA;        /* Unlock OSCCON, step 2  */
    while(OSCCON & (1 << 21));  /* Wait until PBDIV ready */
    OSCCONCLR = 0x180000;       /* clear PBDIV bit <0,1>  */
    while(OSCCON & (1 << 21));  /* Wait until PBDIV ready */
    SYSKEY = 0x0;               /* Lock OSCCON */

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
}

/* Lab-specific initialization goes here */
void labinit(void)
{
  /* Set up PortE LEDs */
  volatile int* trisE = (volatile int*) 0xbf886100;     /* Pointer trisE points to the adress of TRISE              */
  *trisE &= ~0xff;                                      /* Bits 7 through 0 are set to 0 (i.e. output)              */

  volatile int* portE = (volatile int*) 0xbf886110;     /* pointer portE points to the adress of PORTE              */
  *portE = 0x0;

  TRISD |= 0xfe0;                                       /* bits 11 through 5 are set to value 1 (i.e. input)        */

  TRISF |= 0x2;                                         /* bit 1 are set to value 1 (i.e. input, RF1)               */

  T2CON = 0x0070;                                       /* get prescaler 256 and set bit 15 to value 0 (stop timer) */
  TMR2 = 0;                                             /* reset timer to value 0                                   */
  PR2 = TMR2PERIOD;                                     /* set timer period to a defined constant                   */
  IPC(2) = 0x1C | 0x3;                                  /* T2IP priority 7 (bit 4-2), T2IS subpriority 3 (bit 1-0)  */
  IFS(0) = 0 << 8;                                      /* clear Timer 2 interrupt flag, T2IF bit 8                 */
  IEC(0) = 1 << 8;                                      /* enable Timer 2 interrupt, T2IE bit 8                     */
  T2CONSET = 0x8000;                                    /* set bit 15 to value 1 (start timer)                      */
  enable_interrupts();

  return;
}

int timeout = 0;

/* Interrupt Service Routine */
void user_isr(void)
{
  if(IFS(0) & 0x100)                                   /* in case of an interrupt, Timer 2 interrupt flag has value 1 */
  {
    /* Same as IFS(0) = 0 */                           /* clear Timer 2 interrupt flag                                */
    IFSCLR(0) = 0x100;

    timeout++;                                         /* increase timeout by 1                                       */
  }
  return;
}

/*   Data from SW4-1 on bit ind. 11-8 of PORTD
     Result shifted to bit ind. 3-0
     SW4: 1000, SW3: 0100, SW2: 0010, SW1: 0001
     Only SW4 is used in the game. Walls On/ Off. */
int getsw(void)
{
	return ((*portD & 0xf00)>>8);
}

/* Data from BTN4-2 on bit ind. 7-5 of PORTD. Result shifted to bit ind. 3-1
   Data from BTN1 on bit ind. 1 of PORTF. Result shifted to bit ind. 0
   BTN4: 1000, BTN3: 0100, BTN2: 0010 resp. BTN1: 0001 */
int getbtns(void)
{
	return ((*portD & 0xe0)>>4) | ((*portF & 0x2)>>1);
}
