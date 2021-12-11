/* mipslabfunc.c
   This file written 2015 by F Lundevall
   Some parts are original code from mipslabwork.c

   For copyright and licensing, see file COPYING

   This file modified 2021 by P.Khattar & K.Fält */

#include <stdint.h>   /* Declarations of uint_32 and the like */
#include <pic32mx.h>  /* Declarations of system-specific addresses etc */
#include "mipslab.h"  /* Declarations for these labs */

char textbuffer[4][16];
uint8_t displaybuffer[SCREEN_SIZE];
uint16_t directionbuffer[SCREEN_SIZE];
int snakesize;
long random;
int score;
int button;
int head_x;
int head_y;
int tail_x;
int tail_y;
int apple_x;
int apple_y;
int invalid_direction;

/****     Code originally from Lab 3     ****/

/* A simple function to create a small delay.
   Very inefficient use of computing resources,
   but very handy in some special cases. */
void quicksleep(int cyc){
	
	int i;
	for(i = cyc; i > 0; i--);
}


uint8_t spi_send_recv(uint8_t data){
	
	while(!(SPI2STAT & 0x08));
	SPI2BUF = data;
	while(!(SPI2STAT & 1));
	return SPI2BUF;
}


void display_init(void){
        
	DISPLAY_CHANGE_TO_COMMAND_MODE;
	quicksleep(10);
	DISPLAY_ACTIVATE_VDD;
	quicksleep(1000000);

	spi_send_recv(0xAE);
	DISPLAY_ACTIVATE_RESET;
	quicksleep(10);
	DISPLAY_DO_NOT_RESET;
	quicksleep(10);

	spi_send_recv(0x8D);
	spi_send_recv(0x14);

	spi_send_recv(0xD9);
	spi_send_recv(0xF1);

	DISPLAY_ACTIVATE_VBAT;
	quicksleep(10000000);

	spi_send_recv(0xA1);
	spi_send_recv(0xC8);

	spi_send_recv(0xDA);
	spi_send_recv(0x20);

	/* Horizontal address mode */
	spi_send_recv(0x20);
	spi_send_recv(0x00);

	spi_send_recv(0xAF);
}


void display_string(int line, char *s){
	
	int i;
	if(line < 0 || line >= 4)
	   return;
	
	if(!s)
	   return;

	for(i = 0; i < 16; i++)
	    
            if(*s) {
	             textbuffer[line][i] = *s;
	             s++;
		    
	    } else
	             textbuffer[line][i] = ' ';
}


void display_update(void){
	
	int i, j, k;
	int c;
	for(i = 0; i < 4; i++) {
		DISPLAY_CHANGE_TO_COMMAND_MODE;
		spi_send_recv(0x22);
		spi_send_recv(i);

		spi_send_recv(0x0);
		spi_send_recv(0x10);

		DISPLAY_CHANGE_TO_DATA_MODE;

		for(j = 0; j < 16; j++) {
			c = textbuffer[i][j];
			if(c & 0x80)
			   continue;

			for(k = 0; k < 8; k++)
			    spi_send_recv(font[c*8 + k]);
		}
	}
}


char * itoaconv(int num){
  
  register int i, sign;
  static char itoa_buffer[ ITOA_BUFSIZ ];
  static const char maxneg[] = "-2147483648";

  itoa_buffer[ ITOA_BUFSIZ - 1 ] = 0;   /* Insert the end-of-string marker.			 */
  sign = num;                           /* Save sign.                            */
  if( num < 0 && num - 1 > 0 )          /* Check for most negative integer 			 */
  {
    for( i = 0; i < sizeof( maxneg ); i += 1 )
    itoa_buffer[ i + 1 ] = maxneg[ i ];
    i = 0;
  }
  else
  {
    if( num < 0 ) num = -num;           /* Make number positive.                 */
    i = ITOA_BUFSIZ - 2;                /* Location for first ASCII digit.       */
    do {
      itoa_buffer[ i ] = num % 10 + '0';/* Insert next digit.                    */
      num = num / 10;                   /* Remove digit from number.             */
      i -= 1;                           /* Move index to next empty position.    */
    } while( num > 0 );
    if( sign < 0 )
    {
      itoa_buffer[ i ] = '-';
      i -= 1;
    }
  }

  /* Since the loop always sets the index i to the next empty position,
   * we must add 1 in order to return a pointer to the first occupied position.  */
  return( &itoa_buffer[ i + 1 ] );
}


/****   OUR CODE BELOW   ****/

/* Display all black */
void display_clearBuffer(void){
	int i;

	for (i = 0; i < SCREEN_SIZE; i++)
	     displaybuffer[i] = 0x00;
}


/* Sends the whole displaybuffer to spi_send_recv */
void display_buffer_update(void){

	DISPLAY_CHANGE_TO_COMMAND_MODE;

	spi_send_recv(0x22);								/* Send set page command	 */
	spi_send_recv(0);										/* Start section					 */
	spi_send_recv(3);										/* End section 						 */

	spi_send_recv(0x21);								/* Send set column command */
	spi_send_recv(0);										/* Start column 					 */
	spi_send_recv(127);									/* End column              */

	DISPLAY_CHANGE_TO_DATA_MODE;
	int i;
	for(i = 0; i < SCREEN_SIZE; i++)
	    spi_send_recv(displaybuffer[i]);
}


/* Code taken from: https://codeforces.com/blog/entry/61587 */
int get_random(void){
	
	return (((random = random * 214013L + 2531011L) >> 16) & 0x7fff);
}


/* Generates an apple */
void get_apple(void){
	
	do{
		random = (long)timeout;
		apple_x = 1 + get_random() % (SCREEN_WIDTH - 2);
		apple_y = 1 + get_random() % (SCREEN_HEIGHT - 2);
	
	} while((get_displaybuffer(apple_x, apple_y) != 0));

	set_displaybuffer(apple_x, apple_y, 1);
}


/* Check whether the pixel at given x- and y-coordinate is on or off */
int get_displaybuffer(int x, int y){
	
	int shift_nr = y % 8;
	int page_nr = y / 8;

	return ((displaybuffer[(page_nr * 128) + x] >> (shift_nr)) & 1);
}


/* Turn on/off the pixel at the given x- and y-coordinate */
void set_displaybuffer(int x, int y, int pix){

	int shift_nr = y % 8;
	int page_nr = y / 8;

	uint8_t *p = &displaybuffer[(page_nr * 128) + x];
	*p &= ~(1 << shift_nr);
	*p |= (pix << shift_nr);
}


/* Check the direction at the given x- and y-coordinate */
int get_directionbuffer(int x, int y){

        int shift_nr = y % 8;
        int page_nr = y / 8;
  	
	return ((directionbuffer[(page_nr * 128) + x] >> (shift_nr * 2)) & 0b11);
}


/* Set the direction at the given x- and y-coordinate */
/* int dir = 00, 01, 10, 11 */
void set_directionbuffer(int x, int y, int dir){

	int shift_nr = y % 8;
	int page_nr = y / 8;

        uint16_t *p = &directionbuffer[(page_nr * 128) + x];
	*p &= ~(0b11 << (shift_nr * 2));
	*p |= (dir << (shift_nr * 2));
}


/*  Resets the game to start values */
void game_reset(int sw){

	display_clearBuffer();
	set_walls(sw);								/* set_walls updates walls in display buffer */

	snakesize = 8;
	int pos = (SCREEN_HEIGHT - snakesize) / 2;

	int i;
	for (i = 0; i < snakesize; i++){
	     
	     set_displaybuffer(SCREEN_WIDTH / 2, pos + i, 1);
             set_directionbuffer(SCREEN_WIDTH / 2, pos + i, DIRECTION_UP);
	}

	head_x = SCREEN_WIDTH / 2;   /* x-position 64 */
	head_y = pos;          			 /* y-position 12 */

	tail_x = SCREEN_WIDTH / 2;   /* x-position 64, same as head */
	tail_y = pos + (i - 1);      /* y-position 19, snake has start length 8 (12 inclusive to 19 inclusive) */

	score = 0;
	get_apple();
	display_buffer_update();									/* Update display */

	invalid_direction = DIRECTION_DOWN;
}


/* Turns walls on/off according to SW4 */
void set_walls(int sw){
	int x;
	int y;

	if (sw & 8)
	{
		for(x = 0; x < SCREEN_WIDTH; x++){
		    
		    set_displaybuffer(x, 0, 1);
		    set_displaybuffer(x, 31, 1);
		}

		for (y = 0; y < SCREEN_HEIGHT; y++){
		     
		     set_displaybuffer(0, y, 1);
	             set_displaybuffer(127, y, 1);
	 	}
	}
}


/* Updates head's and tail's x- and y-coordinates,
   stores the direction of the head and gets the direction of the tail,
   and updates the display.
	 Return 0 = Game over
	 Return 1 = Continue the game */
int game_iteration(int dir){

	/* Cannot go in the opposite direction */
	if (dir == invalid_direction){

	    switch (dir){

		    case DIRECTION_RIGHT:
			 dir = DIRECTION_LEFT;
			 break;

		    case DIRECTION_DOWN:
		         dir = DIRECTION_UP;
	                 break;

		    case DIRECTION_LEFT:
		         dir = DIRECTION_RIGHT;
		         break;

		    case DIRECTION_UP:
	       	         dir = DIRECTION_DOWN;
			 break;
	    }
	}

	set_displaybuffer(head_x, head_y, 1);
	set_directionbuffer(head_x, head_y, dir);
	display_buffer_update();

	switch (dir){

		case DIRECTION_RIGHT:
		     if (++head_x == SCREEN_WIDTH)
			 head_x = 0;

		     invalid_direction = DIRECTION_LEFT;
		     break;

		case DIRECTION_DOWN:
		     if (++head_y == SCREEN_HEIGHT)
			 head_y = 0;

		     invalid_direction = DIRECTION_UP;
		     break;

		case DIRECTION_LEFT:
		     if (--head_x == -1)
			 head_x += SCREEN_WIDTH;

		     invalid_direction = DIRECTION_RIGHT;
		     break;

		case DIRECTION_UP:
		     if (--head_y == -1)
			 head_y += SCREEN_HEIGHT;

	             invalid_direction = DIRECTION_DOWN;
		     break;
	}

	/* Check if the pixel at heads' next x, y value is on, and if it is not the apple, then return 0 (game over) */
	if((get_displaybuffer(head_x, head_y) & ((head_x != apple_x) && (head_y != apple_y)))){

	    do{

		 display_clearBuffer();
		 display_string(0, "   GAME OVER!  ");
		 display_string(1, "               ");
		 display_string(2, "Score");
		 display_string(3, itoaconv(score));
		 display_update();

		 button = getbtns();

	    } while(button == 0);

	return 0;
	}

	if ((head_x == apple_x) && (head_y == apple_y)){

	     score++;
	     get_apple();
	     snakesize++;

	     return 1;
	}

	int tail_dir = get_directionbuffer(tail_x, tail_y);
	set_displaybuffer(tail_x, tail_y, 0);

	switch (tail_dir){
		
		case DIRECTION_RIGHT:
		     if (++tail_x == SCREEN_WIDTH)
		         tail_x = 0;
		     
		     break;

		case DIRECTION_DOWN:
		     if (++tail_y == SCREEN_HEIGHT)
			 tail_y = 0;
		     
		     break;

		case DIRECTION_LEFT:
		     
		     if (--tail_x == -1)
		         tail_x += SCREEN_WIDTH;
		     
		     break;

		case DIRECTION_UP:
		     if (--tail_y == -1)
			 tail_y += SCREEN_HEIGHT;
		     
		     break;
	}

	return 1;
}
