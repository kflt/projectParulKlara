/* mipslabmain.c

   This file written 2021 by Parul Khattar & Klara Fält
*/

#include <stdint.h>   /* Declarations of uint_32 and the like */
#include <pic32mx.h>  /* Declarations of system-specific addresses etc */
#include "mipslab.h"  /* Declarations for these labs */
#include <stdbool.h>

/*  Pointer to the selected function  */
void (*selectedFunc)();

int stored_timeout = 0;
int pressed_button;
int stored_button;
int old_button;
int direction;
bool b = true;

/*  Program entry point for SNAKE game.  */
int main(void){

	   io_init();
	   labinit();
	   display_init();
	   display_clearBuffer();
	   display_buffer_update();
	   selectedFunc = &show_menu;

	   while(1)

		/*  Calling function using function pointer  */
		selectedFunc();

	   return 0;
}


/* Shows the menu, and when any button is pressed the game resets and starts */
void show_menu(void){

	   quicksleep(1000000);

	   do{
		 display_string(0, "               ");
		 display_string(1, "      PLAY     ");
		 display_string(2, "  SW4 WALLS ON ");
		 display_string(3, "               ");
		 display_update();

		 if ((getbtns() & 1) || (getbtns() & 2) || (getbtns() & 4) || (getbtns() & 8)){

		      game_reset(getsw());
		      selectedFunc = &play_game;
		      b = false;
		 }

	    } while(b);
}


/* Stores the current button and selects the direction accordingly
   Loops until the game is over, and then sets the function pointer to menu */
void play_game(void){

	   if (timeout == stored_timeout)
	       return;

	   stored_timeout = timeout;

	   pressed_button = getbtns();

	   if (pressed_button)
	       stored_button = pressed_button;

	   if (old_button != stored_button){
	       old_button = stored_button;

	       if (stored_button & 1)
		   direction = DIRECTION_RIGHT;

	       else if (stored_button & 2)
			direction = DIRECTION_DOWN;

	       else if (stored_button & 8)
			direction = DIRECTION_LEFT;

	       else
		    direction = DIRECTION_UP;
	   }

	   /* Return 0 = Game over
	      Return 1 = Continue playing */
	   if (!game_iteration(direction))
		selectedFunc = &show_menu;
}
