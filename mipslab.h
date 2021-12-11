/* mipslab.h
   Header file for all labs.
   This file written 2015 by F Lundevall
   Some parts are original code written by Axel Isaksson

   Latest update 2021 by P.Khattar & K.Fält

   For copyright and licensing, see file COPYING */

   #include <stdint.h>   /* Declarations of uint_32 and the like */
   #include <pic32mx.h>  /* Declarations of system-specific addresses etc */

   #define ITOA_BUFSIZ       24

   #define SCREEN_WIDTH     128
   #define SCREEN_HEIGHT     32

   /* 4 pages * 128 bytes (8-pixel-high columns) */
   #define SCREEN_SIZE      512

   #define DIRECTION_UP       0
   #define DIRECTION_RIGHT    1
   #define DIRECTION_DOWN     2
   #define DIRECTION_LEFT     3

   /* Interrupt timeout counter */
   extern int timeout;

   /* From Lab 3 */

   /* Used with method itoaconv */
   #define ITOA_BUFSIZ       24

   #define DISPLAY_CHANGE_TO_COMMAND_MODE (PORTFCLR = 0x10)
   #define DISPLAY_CHANGE_TO_DATA_MODE (PORTFSET = 0x10)

   #define DISPLAY_ACTIVATE_RESET (PORTGCLR = 0x200)
   #define DISPLAY_DO_NOT_RESET (PORTGSET = 0x200)

   #define DISPLAY_ACTIVATE_VDD (PORTFCLR = 0x40)
   #define DISPLAY_ACTIVATE_VBAT (PORTFCLR = 0x20)

   #define DISPLAY_TURN_OFF_VDD (PORTFSET = 0x40)
   #define DISPLAY_TURN_OFF_VBAT (PORTFSET = 0x20)


   /**** mipslabmain.c ****/

   /*  Written By P.Khattar & K.Fält  */
   int main(void);
   void show_menu(void);
   void play_game(void);


   /**** mipslabfunc.c ****/

   /*  Originally from Lab 3  */
   extern char textbuffer[4][16]; /* Declare text buffer for display output */
   void quicksleep(int cyc);
   uint8_t spi_send_recv(uint8_t data);
   void display_init(void);
   void display_string(int line, char *s);
   void display_update(void);
   char * itoaconv(int num);

   /*  Written By P.Khattar & K.Fält  */
   extern uint8_t displaybuffer[SCREEN_SIZE]; /* Declare display buffer for display output */
   extern uint16_t directionbuffer[SCREEN_SIZE];
   void display_clearBuffer(void);
   void display_buffer_update(void);
   int get_random(void);
   void get_apple(void);
   int get_displaybuffer(int x, int y);
   void set_displaybuffer(int x, int y, int pix);
   int get_directionbuffer(int x, int y);
   void set_directionbuffer(int x, int y, int dir);
   void game_reset(int sw);
   void set_walls(int sw);
   int game_iteration(int dir);


   /**** mipslabdata.c ****/

   /*  Originally from Lab 3  */
   extern const uint8_t const font[128*8]; /* Declare bitmap array containing font */


   /**** time4io.c ****/ /* Written as part of i/o lab: labinit, getbtns, getsw, enable_interrupts */

   /*  Modified in Lab 3 by P.Khattar & K.Fält  */
   void io_init(void);
   void labinit(void);
   void enable_interrupts(void);  // vectors.S
   void user_isr(void);
   int getsw(void);
   int getbtns(void);


   /**** stubs.c ****/

   /*  Originally from Lab 3  */
   void _nmi_handler();
   void _on_reset();
   void _on_bootstrap();
