
/*
   .-------------.       .    .   *       *   
  /_/_/_/_/_/_/_/ \         *       .   )    .
 //_/_/_/_/_/_// _ \ __          .        .   
/_/_/_/_/_/_/_/|/ \.' .`-o                    
 |             ||-'(/ ,--'                    
 |             ||  _ |                        
 |             ||'' ||                        
 |_____________|| |_|L                    
 */

 
/*  Dette program sender indholdet af "arrayToSaveToFlash[]" over SPI
 *   til MX25L6445E flashen som sidder på Papilioen. 
 *   
 * 
 */





/*
#################################################################################################
   _____   ______   _______   _______   _____   _   _    _____    _____ 
  / ____| |  ____| |__   __| |__   __| |_   _| | \ | |  / ____|  / ____|
 | (___   | |__       | |       | |      | |   |  \| | | |  __  | (___  
  \___ \  |  __|      | |       | |      | |   | . ` | | | |_ |  \___ \ 
  ____) | | |____     | |       | |     _| |_  | |\  | | |__| |  ____) |
 |_____/  |______|    |_|       |_|    |_____| |_| \_|  \_____| |_____/ 
#################################################################################################
*/
#define CHOOSE_PROGRAM_MODE     PP
  /* Valid commands:
   *  CP: Continously program mode (For large files
   *  PP: Page program (Max 4k-bit files)
   *
   */

// Hvis CHOOSE_PROGRAM_MODE == PP gælder STOP_ADRESS:
  // Block-skifte ligger ved 0x010000, 0x020000 osv.
  // Adressen hvorfra der startes med at skrive 
  #define START_ADRESS      0x003000

    #define KICK_ADRESS     0x010000
    #define SNARE_ADRESS    0x020000
    #define HAT_ADRESS      0x030000
    #define CLAP_ADRESS     0x040000
  
  // Adressen hvortil der skrives. 0x0 for at deaktivere
  #define STOP_ADRESS       0x000000


  // 0: Disable 
  // 1: Enable
#define SHOULD_WIPE_WHOLE_CHIP        1
#define SHOULD_BLOCK_ERASE            0
#define SHOULD_LOCK_AFTER_PROGRAMMING 0


//    0: Disable
//    1: Enable
#define DEBUG_BLOCK_ERASE 1
#define DEBUG_WHILE_LOOPS 0
#define DEBUG_WRONG_DATA  0 
#define PRINT_WHERE_WE_ARE_READING  1
#define PRINT_WHERE_WE_ARE_WRITING  0 
#define PRINT_THE_READ_DATA         0


 
/*
#################################################################################################
  _____    _____   _   _      ____    _    _   _______ 
 |  __ \  |_   _| | \ | |    / __ \  | |  | | |__   __|
 | |__) |   | |   |  \| |   | |  | | | |  | |    | |   
 |  ___/    | |   | . ` |   | |  | | | |  | |    | |   
 | |       _| |_  | |\  |   | |__| | | |__| |    | |   
 |_|      |_____| |_| \_|    \____/   \____/     |_|   
#################################################################################################
*/   
/*  PINOUT
 * ------------------------------------------
 * |     Arduino  T Port      T  Papilio    |
 * |  Clock:   13 | B00100000 |  13         |
 * |  MISO:    12 | B00010000 |  12         |
 * |  MOSI:    11 | B00001000 |  11         |
 * |  SS:      10 | B00000100 |  09         |
 * ------------------------------------------
 */
  
 
/*
#################################################################################################
  _____    ______   ______   _____   _   _   ______    _____ 
 |  __ \  |  ____| |  ____| |_   _| | \ | | |  ____|  / ____|
 | |  | | | |__    | |__      | |   |  \| | | |__    | (___  
 | |  | | |  __|   |  __|     | |   | . ` | |  __|    \___ \ 
 | |__| | | |____  | |       _| |_  | |\  | | |____   ____) |
 |_____/  |______| |_|      |_____| |_| \_| |______| |_____/ 
#################################################################################################
*/
#include <avr/pgmspace.h>
#include "Audiosamples.h"

#define DUE_CLOCK   13
#define DUE_MISO    12
#define DUE_MOSI    11
#define DUE_SS      10
#define DUE_BP       9
  


#define CP  1
#define PP  2   


#define USE_BIG_ARRAY     5
        /*
         * Vælger hvilket data-array der skal bruges
         *  5: fra AudioSamples.h
         *  4: 2048 byte
         *  3:  512 byte  <- Der er tilsyneladende en fejl med denne størrelse
         *  2:  256 byte
         *  1:  128 byte
         *  0:    2 byte
         */
        

/*
#################################################################################################
 __      __             _____    _____              ____    _        ______    _____ 
 \ \    / /     /\     |  __ \  |_   _|     /\     |  _ \  | |      |  ____|  / ____|
  \ \  / /     /  \    | |__) |   | |      /  \    | |_) | | |      | |__    | (___  
   \ \/ /     / /\ \   |  _  /    | |     / /\ \   |  _ <  | |      |  __|    \___ \ 
    \  /     / ____ \  | | \ \   _| |_   / ____ \  | |_) | | |____  | |____   ____) |
     \/     /_/    \_\ |_|  \_\ |_____| /_/    \_\ |____/  |______| |______| |_____/
#################################################################################################
*/

  
      uint32_t adressenViHusker = START_ADRESS;
const uint32_t startAdressen = START_ADRESS;


// Da flashen er fyldt med 1'ere, skriver vi bevidst 0'ere
// til den, så vi tjekker om vi rammer rigtigt.
  //byte arrayToSaveToFlash[] = {0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F};

#if   USE_BIG_ARRAY == 4
// 2048 byte
  const byte arrayToSaveToFlash[] = {
    0xAA, 0x10, 0x20, 0x30, 0x40, 0x50, 0x60, 0x70, 0x80, 0x90, 0xA0, 0xB0, 0xC0, 0xD0, 0xE0, 0xFA,
    0xAA, 0x10, 0x20, 0x30, 0x40, 0x50, 0x60, 0x70, 0x80, 0x90, 0xA0, 0xB0, 0xC0, 0xD0, 0xE0, 0xFA,
    0xAA, 0x10, 0x20, 0x30, 0x40, 0x50, 0x60, 0x70, 0x80, 0x90, 0xA0, 0xB0, 0xC0, 0xD0, 0xE0, 0xFA,
    0xAA, 0x10, 0x20, 0x30, 0x40, 0x50, 0x60, 0x70, 0x80, 0x90, 0xA0, 0xB0, 0xC0, 0xD0, 0xE0, 0xFA,
    0xAA, 0x10, 0x20, 0x30, 0x40, 0x50, 0x60, 0x70, 0x80, 0x90, 0xA0, 0xB0, 0xC0, 0xD0, 0xE0, 0xFA,
    0xAA, 0x10, 0x20, 0x30, 0x40, 0x50, 0x60, 0x70, 0x80, 0x90, 0xA0, 0xB0, 0xC0, 0xD0, 0xE0, 0xFA,
    0xAA, 0x10, 0x20, 0x30, 0x40, 0x50, 0x60, 0x70, 0x80, 0x90, 0xA0, 0xB0, 0xC0, 0xD0, 0xE0, 0xFA,
    0xAA, 0x10, 0x20, 0x30, 0x40, 0x50, 0x60, 0x70, 0x80, 0x90, 0xA0, 0xB0, 0xC0, 0xD0, 0xE0, 0xFA,
    0xAA, 0x10, 0x20, 0x30, 0x40, 0x50, 0x60, 0x70, 0x80, 0x90, 0xA0, 0xB0, 0xC0, 0xD0, 0xE0, 0xFA,
    0xAA, 0x10, 0x20, 0x30, 0x40, 0x50, 0x60, 0x70, 0x80, 0x90, 0xA0, 0xB0, 0xC0, 0xD0, 0xE0, 0xFA,
    0xAA, 0x10, 0x20, 0x30, 0x40, 0x50, 0x60, 0x70, 0x80, 0x90, 0xA0, 0xB0, 0xC0, 0xD0, 0xE0, 0xFA,
    0xAA, 0x10, 0x20, 0x30, 0x40, 0x50, 0x60, 0x70, 0x80, 0x90, 0xA0, 0xB0, 0xC0, 0xD0, 0xE0, 0xFA,
    0xAA, 0x10, 0x20, 0x30, 0x40, 0x50, 0x60, 0x70, 0x80, 0x90, 0xA0, 0xB0, 0xC0, 0xD0, 0xE0, 0xFA,
    0xAA, 0x10, 0x20, 0x30, 0x40, 0x50, 0x60, 0x70, 0x80, 0x90, 0xA0, 0xB0, 0xC0, 0xD0, 0xE0, 0xFA,
    0xAA, 0x10, 0x20, 0x30, 0x40, 0x50, 0x60, 0x70, 0x80, 0x90, 0xA0, 0xB0, 0xC0, 0xD0, 0xE0, 0xFA,
    0xAA, 0x10, 0x20, 0x30, 0x40, 0x50, 0x60, 0x70, 0x80, 0x90, 0xA0, 0xB0, 0xC0, 0xD0, 0xE0, 0xFA,
      0xAA, 0x10, 0x20, 0x30, 0x40, 0x50, 0x60, 0x70, 0x80, 0x90, 0xA0, 0xB0, 0xC0, 0xD0, 0xE0, 0xFA,
    0xAA, 0x10, 0x20, 0x30, 0x40, 0x50, 0x60, 0x70, 0x80, 0x90, 0xA0, 0xB0, 0xC0, 0xD0, 0xE0, 0xFA,
    0xAA, 0x10, 0x20, 0x30, 0x40, 0x50, 0x60, 0x70, 0x80, 0x90, 0xA0, 0xB0, 0xC0, 0xD0, 0xE0, 0xFA,
    0xAA, 0x10, 0x20, 0x30, 0x40, 0x50, 0x60, 0x70, 0x80, 0x90, 0xA0, 0xB0, 0xC0, 0xD0, 0xE0, 0xFA,
    0xAA, 0x10, 0x20, 0x30, 0x40, 0x50, 0x60, 0x70, 0x80, 0x90, 0xA0, 0xB0, 0xC0, 0xD0, 0xE0, 0xFA,
    0xAA, 0x10, 0x20, 0x30, 0x40, 0x50, 0x60, 0x70, 0x80, 0x90, 0xA0, 0xB0, 0xC0, 0xD0, 0xE0, 0xFA,
    0xAA, 0x10, 0x20, 0x30, 0x40, 0x50, 0x60, 0x70, 0x80, 0x90, 0xA0, 0xB0, 0xC0, 0xD0, 0xE0, 0xFA,
    0xAA, 0x10, 0x20, 0x30, 0x40, 0x50, 0x60, 0x70, 0x80, 0x90, 0xA0, 0xB0, 0xC0, 0xD0, 0xE0, 0xFA,
    0xAA, 0x10, 0x20, 0x30, 0x40, 0x50, 0x60, 0x70, 0x80, 0x90, 0xA0, 0xB0, 0xC0, 0xD0, 0xE0, 0xFA,
    0xAA, 0x10, 0x20, 0x30, 0x40, 0x50, 0x60, 0x70, 0x80, 0x90, 0xA0, 0xB0, 0xC0, 0xD0, 0xE0, 0xFA,
    0xAA, 0x10, 0x20, 0x30, 0x40, 0x50, 0x60, 0x70, 0x80, 0x90, 0xA0, 0xB0, 0xC0, 0xD0, 0xE0, 0xFA,
    0xAA, 0x10, 0x20, 0x30, 0x40, 0x50, 0x60, 0x70, 0x80, 0x90, 0xA0, 0xB0, 0xC0, 0xD0, 0xE0, 0xFA,
    0xAA, 0x10, 0x20, 0x30, 0x40, 0x50, 0x60, 0x70, 0x80, 0x90, 0xA0, 0xB0, 0xC0, 0xD0, 0xE0, 0xFA,
    0xAA, 0x10, 0x20, 0x30, 0x40, 0x50, 0x60, 0x70, 0x80, 0x90, 0xA0, 0xB0, 0xC0, 0xD0, 0xE0, 0xFA,
    0xAA, 0x10, 0x20, 0x30, 0x40, 0x50, 0x60, 0x70, 0x80, 0x90, 0xA0, 0xB0, 0xC0, 0xD0, 0xE0, 0xFA,
    0xAA, 0x10, 0x20, 0x30, 0x40, 0x50, 0x60, 0x70, 0x80, 0x90, 0xA0, 0xB0, 0xC0, 0xD0, 0xE0, 0xFA,
    0xAA, 0x10, 0x20, 0x30, 0x40, 0x50, 0x60, 0x70, 0x80, 0x90, 0xA0, 0xB0, 0xC0, 0xD0, 0xE0, 0xFA,
     0xAA, 0x10, 0x20, 0x30, 0x40, 0x50, 0x60, 0x70, 0x80, 0x90, 0xA0, 0xB0, 0xC0, 0xD0, 0xE0, 0xFA,
    0xAA, 0x10, 0x20, 0x30, 0x40, 0x50, 0x60, 0x70, 0x80, 0x90, 0xA0, 0xB0, 0xC0, 0xD0, 0xE0, 0xFA,
    0xAA, 0x10, 0x20, 0x30, 0x40, 0x50, 0x60, 0x70, 0x80, 0x90, 0xA0, 0xB0, 0xC0, 0xD0, 0xE0, 0xFA,
    0xAA, 0x10, 0x20, 0x30, 0x40, 0x50, 0x60, 0x70, 0x80, 0x90, 0xA0, 0xB0, 0xC0, 0xD0, 0xE0, 0xFA,
    0xAA, 0x10, 0x20, 0x30, 0x40, 0x50, 0x60, 0x70, 0x80, 0x90, 0xA0, 0xB0, 0xC0, 0xD0, 0xE0, 0xFA,
    0xAA, 0x10, 0x20, 0x30, 0x40, 0x50, 0x60, 0x70, 0x80, 0x90, 0xA0, 0xB0, 0xC0, 0xD0, 0xE0, 0xFA,
    0xAA, 0x10, 0x20, 0x30, 0x40, 0x50, 0x60, 0x70, 0x80, 0x90, 0xA0, 0xB0, 0xC0, 0xD0, 0xE0, 0xFA,
    0xAA, 0x10, 0x20, 0x30, 0x40, 0x50, 0x60, 0x70, 0x80, 0x90, 0xA0, 0xB0, 0xC0, 0xD0, 0xE0, 0xFA,
    0xAA, 0x10, 0x20, 0x30, 0x40, 0x50, 0x60, 0x70, 0x80, 0x90, 0xA0, 0xB0, 0xC0, 0xD0, 0xE0, 0xFA,
    0xAA, 0x10, 0x20, 0x30, 0x40, 0x50, 0x60, 0x70, 0x80, 0x90, 0xA0, 0xB0, 0xC0, 0xD0, 0xE0, 0xFA,
    0xAA, 0x10, 0x20, 0x30, 0x40, 0x50, 0x60, 0x70, 0x80, 0x90, 0xA0, 0xB0, 0xC0, 0xD0, 0xE0, 0xFA,
    0xAA, 0x10, 0x20, 0x30, 0x40, 0x50, 0x60, 0x70, 0x80, 0x90, 0xA0, 0xB0, 0xC0, 0xD0, 0xE0, 0xFA,
    0xAA, 0x10, 0x20, 0x30, 0x40, 0x50, 0x60, 0x70, 0x80, 0x90, 0xA0, 0xB0, 0xC0, 0xD0, 0xE0, 0xFA,
    0xAA, 0x10, 0x20, 0x30, 0x40, 0x50, 0x60, 0x70, 0x80, 0x90, 0xA0, 0xB0, 0xC0, 0xD0, 0xE0, 0xFA,
    0xAA, 0x10, 0x20, 0x30, 0x40, 0x50, 0x60, 0x70, 0x80, 0x90, 0xA0, 0xB0, 0xC0, 0xD0, 0xE0, 0xFA,
    0xAA, 0x10, 0x20, 0x30, 0x40, 0x50, 0x60, 0x70, 0x80, 0x90, 0xA0, 0xB0, 0xC0, 0xD0, 0xE0, 0xFA,
    0xAA, 0x10, 0x20, 0x30, 0x40, 0x50, 0x60, 0x70, 0x80, 0x90, 0xA0, 0xB0, 0xC0, 0xD0, 0xE0, 0xFA,
     0xAA, 0x10, 0x20, 0x30, 0x40, 0x50, 0x60, 0x70, 0x80, 0x90, 0xA0, 0xB0, 0xC0, 0xD0, 0xE0, 0xFA,
    0xAA, 0x10, 0x20, 0x30, 0x40, 0x50, 0x60, 0x70, 0x80, 0x90, 0xA0, 0xB0, 0xC0, 0xD0, 0xE0, 0xFA,
    0xAA, 0x10, 0x20, 0x30, 0x40, 0x50, 0x60, 0x70, 0x80, 0x90, 0xA0, 0xB0, 0xC0, 0xD0, 0xE0, 0xFA,
    0xAA, 0x10, 0x20, 0x30, 0x40, 0x50, 0x60, 0x70, 0x80, 0x90, 0xA0, 0xB0, 0xC0, 0xD0, 0xE0, 0xFA,
    0xAA, 0x10, 0x20, 0x30, 0x40, 0x50, 0x60, 0x70, 0x80, 0x90, 0xA0, 0xB0, 0xC0, 0xD0, 0xE0, 0xFA,
    0xAA, 0x10, 0x20, 0x30, 0x40, 0x50, 0x60, 0x70, 0x80, 0x90, 0xA0, 0xB0, 0xC0, 0xD0, 0xE0, 0xFA,
    0xAA, 0x10, 0x20, 0x30, 0x40, 0x50, 0x60, 0x70, 0x80, 0x90, 0xA0, 0xB0, 0xC0, 0xD0, 0xE0, 0xFA,
    0xAA, 0x10, 0x20, 0x30, 0x40, 0x50, 0x60, 0x70, 0x80, 0x90, 0xA0, 0xB0, 0xC0, 0xD0, 0xE0, 0xFA,
    0xAA, 0x10, 0x20, 0x30, 0x40, 0x50, 0x60, 0x70, 0x80, 0x90, 0xA0, 0xB0, 0xC0, 0xD0, 0xE0, 0xFA,
    0xAA, 0x10, 0x20, 0x30, 0x40, 0x50, 0x60, 0x70, 0x80, 0x90, 0xA0, 0xB0, 0xC0, 0xD0, 0xE0, 0xFA,
    0xAA, 0x10, 0x20, 0x30, 0x40, 0x50, 0x60, 0x70, 0x80, 0x90, 0xA0, 0xB0, 0xC0, 0xD0, 0xE0, 0xFA,
    0xAA, 0x10, 0x20, 0x30, 0x40, 0x50, 0x60, 0x70, 0x80, 0x90, 0xA0, 0xB0, 0xC0, 0xD0, 0xE0, 0xFA,
    0xAA, 0x10, 0x20, 0x30, 0x40, 0x50, 0x60, 0x70, 0x80, 0x90, 0xA0, 0xB0, 0xC0, 0xD0, 0xE0, 0xFA,
    0xAA, 0x10, 0x20, 0x30, 0x40, 0x50, 0x60, 0x70, 0x80, 0x90, 0xA0, 0xB0, 0xC0, 0xD0, 0xE0, 0xFA,
    0xAA, 0x10, 0x20, 0x30, 0x40, 0x50, 0x60, 0x70, 0x80, 0x90, 0xA0, 0xB0, 0xC0, 0xD0, 0xE0, 0xFA,
    0xAA, 0x10, 0x20, 0x30, 0x40, 0x50, 0x60, 0x70, 0x80, 0x90, 0xA0, 0xB0, 0xC0, 0xD0, 0xE0, 0xFA,
    0xAA, 0x10, 0x20, 0x30, 0x40, 0x50, 0x60, 0x70, 0x80, 0x90, 0xA0, 0xB0, 0xC0, 0xD0, 0xE0, 0xFA,
     0xAA, 0x10, 0x20, 0x30, 0x40, 0x50, 0x60, 0x70, 0x80, 0x90, 0xA0, 0xB0, 0xC0, 0xD0, 0xE0, 0xFA,
    0xAA, 0x10, 0x20, 0x30, 0x40, 0x50, 0x60, 0x70, 0x80, 0x90, 0xA0, 0xB0, 0xC0, 0xD0, 0xE0, 0xFA,
    0xAA, 0x10, 0x20, 0x30, 0x40, 0x50, 0x60, 0x70, 0x80, 0x90, 0xA0, 0xB0, 0xC0, 0xD0, 0xE0, 0xFA,
    0xAA, 0x10, 0x20, 0x30, 0x40, 0x50, 0x60, 0x70, 0x80, 0x90, 0xA0, 0xB0, 0xC0, 0xD0, 0xE0, 0xFA,
    0xAA, 0x10, 0x20, 0x30, 0x40, 0x50, 0x60, 0x70, 0x80, 0x90, 0xA0, 0xB0, 0xC0, 0xD0, 0xE0, 0xFA,
    0xAA, 0x10, 0x20, 0x30, 0x40, 0x50, 0x60, 0x70, 0x80, 0x90, 0xA0, 0xB0, 0xC0, 0xD0, 0xE0, 0xFA,
    0xAA, 0x10, 0x20, 0x30, 0x40, 0x50, 0x60, 0x70, 0x80, 0x90, 0xA0, 0xB0, 0xC0, 0xD0, 0xE0, 0xFA,
    0xAA, 0x10, 0x20, 0x30, 0x40, 0x50, 0x60, 0x70, 0x80, 0x90, 0xA0, 0xB0, 0xC0, 0xD0, 0xE0, 0xFA,
    0xAA, 0x10, 0x20, 0x30, 0x40, 0x50, 0x60, 0x70, 0x80, 0x90, 0xA0, 0xB0, 0xC0, 0xD0, 0xE0, 0xFA,
    0xAA, 0x10, 0x20, 0x30, 0x40, 0x50, 0x60, 0x70, 0x80, 0x90, 0xA0, 0xB0, 0xC0, 0xD0, 0xE0, 0xFA,
    0xAA, 0x10, 0x20, 0x30, 0x40, 0x50, 0x60, 0x70, 0x80, 0x90, 0xA0, 0xB0, 0xC0, 0xD0, 0xE0, 0xFA,
    0xAA, 0x10, 0x20, 0x30, 0x40, 0x50, 0x60, 0x70, 0x80, 0x90, 0xA0, 0xB0, 0xC0, 0xD0, 0xE0, 0xFA,
    0xAA, 0x10, 0x20, 0x30, 0x40, 0x50, 0x60, 0x70, 0x80, 0x90, 0xA0, 0xB0, 0xC0, 0xD0, 0xE0, 0xFA,
    0xAA, 0x10, 0x20, 0x30, 0x40, 0x50, 0x60, 0x70, 0x80, 0x90, 0xA0, 0xB0, 0xC0, 0xD0, 0xE0, 0xFA,
    0xAA, 0x10, 0x20, 0x30, 0x40, 0x50, 0x60, 0x70, 0x80, 0x90, 0xA0, 0xB0, 0xC0, 0xD0, 0xE0, 0xFA,
    0xAA, 0x10, 0x20, 0x30, 0x40, 0x50, 0x60, 0x70, 0x80, 0x90, 0xA0, 0xB0, 0xC0, 0xD0, 0xE0, 0xFA,
    0xAA, 0x10, 0x20, 0x30, 0x40, 0x50, 0x60, 0x70, 0x80, 0x90, 0xA0, 0xB0, 0xC0, 0xD0, 0xE0, 0xFA,
     0xAA, 0x10, 0x20, 0x30, 0x40, 0x50, 0x60, 0x70, 0x80, 0x90, 0xA0, 0xB0, 0xC0, 0xD0, 0xE0, 0xFA,
    0xAA, 0x10, 0x20, 0x30, 0x40, 0x50, 0x60, 0x70, 0x80, 0x90, 0xA0, 0xB0, 0xC0, 0xD0, 0xE0, 0xFA,
    0xAA, 0x10, 0x20, 0x30, 0x40, 0x50, 0x60, 0x70, 0x80, 0x90, 0xA0, 0xB0, 0xC0, 0xD0, 0xE0, 0xFA,
    0xAA, 0x10, 0x20, 0x30, 0x40, 0x50, 0x60, 0x70, 0x80, 0x90, 0xA0, 0xB0, 0xC0, 0xD0, 0xE0, 0xFA,
    0xAA, 0x10, 0x20, 0x30, 0x40, 0x50, 0x60, 0x70, 0x80, 0x90, 0xA0, 0xB0, 0xC0, 0xD0, 0xE0, 0xFA,
    0xAA, 0x10, 0x20, 0x30, 0x40, 0x50, 0x60, 0x70, 0x80, 0x90, 0xA0, 0xB0, 0xC0, 0xD0, 0xE0, 0xFA,
    0xAA, 0x10, 0x20, 0x30, 0x40, 0x50, 0x60, 0x70, 0x80, 0x90, 0xA0, 0xB0, 0xC0, 0xD0, 0xE0, 0xFA,
    0xAA, 0x10, 0x20, 0x30, 0x40, 0x50, 0x60, 0x70, 0x80, 0x90, 0xA0, 0xB0, 0xC0, 0xD0, 0xE0, 0xFA,
    0xAA, 0x10, 0x20, 0x30, 0x40, 0x50, 0x60, 0x70, 0x80, 0x90, 0xA0, 0xB0, 0xC0, 0xD0, 0xE0, 0xFA,
    0xAA, 0x10, 0x20, 0x30, 0x40, 0x50, 0x60, 0x70, 0x80, 0x90, 0xA0, 0xB0, 0xC0, 0xD0, 0xE0, 0xFA,
    0xAA, 0x10, 0x20, 0x30, 0x40, 0x50, 0x60, 0x70, 0x80, 0x90, 0xA0, 0xB0, 0xC0, 0xD0, 0xE0, 0xFA,
    0xAA, 0x10, 0x20, 0x30, 0x40, 0x50, 0x60, 0x70, 0x80, 0x90, 0xA0, 0xB0, 0xC0, 0xD0, 0xE0, 0xFA,
    0xAA, 0x10, 0x20, 0x30, 0x40, 0x50, 0x60, 0x70, 0x80, 0x90, 0xA0, 0xB0, 0xC0, 0xD0, 0xE0, 0xFA,
    0xAA, 0x10, 0x20, 0x30, 0x40, 0x50, 0x60, 0x70, 0x80, 0x90, 0xA0, 0xB0, 0xC0, 0xD0, 0xE0, 0xFA,
    0xAA, 0x10, 0x20, 0x30, 0x40, 0x50, 0x60, 0x70, 0x80, 0x90, 0xA0, 0xB0, 0xC0, 0xD0, 0xE0, 0xFA,
    0xAA, 0x10, 0x20, 0x30, 0x40, 0x50, 0x60, 0x70, 0x80, 0x90, 0xA0, 0xB0, 0xC0, 0xD0, 0xE0, 0xFA,
    0xAA, 0x10, 0x20, 0x30, 0x40, 0x50, 0x60, 0x70, 0x80, 0x90, 0xA0, 0xB0, 0xC0, 0xD0, 0xE0, 0xFA,
     0xAA, 0x10, 0x20, 0x30, 0x40, 0x50, 0x60, 0x70, 0x80, 0x90, 0xA0, 0xB0, 0xC0, 0xD0, 0xE0, 0xFA,
    0xAA, 0x10, 0x20, 0x30, 0x40, 0x50, 0x60, 0x70, 0x80, 0x90, 0xA0, 0xB0, 0xC0, 0xD0, 0xE0, 0xFA,
    0xAA, 0x10, 0x20, 0x30, 0x40, 0x50, 0x60, 0x70, 0x80, 0x90, 0xA0, 0xB0, 0xC0, 0xD0, 0xE0, 0xFA,
    0xAA, 0x10, 0x20, 0x30, 0x40, 0x50, 0x60, 0x70, 0x80, 0x90, 0xA0, 0xB0, 0xC0, 0xD0, 0xE0, 0xFA,
    0xAA, 0x10, 0x20, 0x30, 0x40, 0x50, 0x60, 0x70, 0x80, 0x90, 0xA0, 0xB0, 0xC0, 0xD0, 0xE0, 0xFA,
    0xAA, 0x10, 0x20, 0x30, 0x40, 0x50, 0x60, 0x70, 0x80, 0x90, 0xA0, 0xB0, 0xC0, 0xD0, 0xE0, 0xFA,
    0xAA, 0x10, 0x20, 0x30, 0x40, 0x50, 0x60, 0x70, 0x80, 0x90, 0xA0, 0xB0, 0xC0, 0xD0, 0xE0, 0xFA,
    0xAA, 0x10, 0x20, 0x30, 0x40, 0x50, 0x60, 0x70, 0x80, 0x90, 0xA0, 0xB0, 0xC0, 0xD0, 0xE0, 0xFA,
    0xAA, 0x10, 0x20, 0x30, 0x40, 0x50, 0x60, 0x70, 0x80, 0x90, 0xA0, 0xB0, 0xC0, 0xD0, 0xE0, 0xFA,
    0xAA, 0x10, 0x20, 0x30, 0x40, 0x50, 0x60, 0x70, 0x80, 0x90, 0xA0, 0xB0, 0xC0, 0xD0, 0xE0, 0xFA,
    0xAA, 0x10, 0x20, 0x30, 0x40, 0x50, 0x60, 0x70, 0x80, 0x90, 0xA0, 0xB0, 0xC0, 0xD0, 0xE0, 0xFA,
    0xAA, 0x10, 0x20, 0x30, 0x40, 0x50, 0x60, 0x70, 0x80, 0x90, 0xA0, 0xB0, 0xC0, 0xD0, 0xE0, 0xFA,
    0xAA, 0x10, 0x20, 0x30, 0x40, 0x50, 0x60, 0x70, 0x80, 0x90, 0xA0, 0xB0, 0xC0, 0xD0, 0xE0, 0xFA,
    0xAA, 0x10, 0x20, 0x30, 0x40, 0x50, 0x60, 0x70, 0x80, 0x90, 0xA0, 0xB0, 0xC0, 0xD0, 0xE0, 0xFA,
    0xAA, 0x10, 0x20, 0x30, 0x40, 0x50, 0x60, 0x70, 0x80, 0x90, 0xA0, 0xB0, 0xC0, 0xD0, 0xE0, 0xFA,
    0xAA, 0x10, 0x20, 0x30, 0x40, 0x50, 0x60, 0x70, 0x80, 0x90, 0xA0, 0xB0, 0xC0, 0xD0, 0xE0, 0xFA,
    0xAA, 0x10, 0x20, 0x30, 0x40, 0x50, 0x60, 0x70, 0x80, 0x90, 0xA0, 0xB0, 0xC0, 0xD0, 0xE0, 0xFA,
     0xAA, 0x10, 0x20, 0x30, 0x40, 0x50, 0x60, 0x70, 0x80, 0x90, 0xA0, 0xB0, 0xC0, 0xD0, 0xE0, 0xFA,
    0xAA, 0x10, 0x20, 0x30, 0x40, 0x50, 0x60, 0x70, 0x80, 0x90, 0xA0, 0xB0, 0xC0, 0xD0, 0xE0, 0xFA,
    0xAA, 0x10, 0x20, 0x30, 0x40, 0x50, 0x60, 0x70, 0x80, 0x90, 0xA0, 0xB0, 0xC0, 0xD0, 0xE0, 0xFA,
    0xAA, 0x10, 0x20, 0x30, 0x40, 0x50, 0x60, 0x70, 0x80, 0x90, 0xA0, 0xB0, 0xC0, 0xD0, 0xE0, 0xFA,
    0xAA, 0x10, 0x20, 0x30, 0x40, 0x50, 0x60, 0x70, 0x80, 0x90, 0xA0, 0xB0, 0xC0, 0xD0, 0xE0, 0xFA,
    0xAA, 0x10, 0x20, 0x30, 0x40, 0x50, 0x60, 0x70, 0x80, 0x90, 0xA0, 0xB0, 0xC0, 0xD0, 0xE0, 0xFA,
    0xAA, 0x10, 0x20, 0x30, 0x40, 0x50, 0x60, 0x70, 0x80, 0x90, 0xA0, 0xB0, 0xC0, 0xD0, 0xE0, 0xFA,
    0xAA, 0x10, 0x20, 0x30, 0x40, 0x50, 0x60, 0x70, 0x80, 0x90, 0xA0, 0xB0, 0xC0, 0xD0, 0xE0, 0xFA,
    0xAA, 0x10, 0x20, 0x30, 0x40, 0x50, 0x60, 0x70, 0x80, 0x90, 0xA0, 0xB0, 0xC0, 0xD0, 0xE0, 0xFA,
    0xAA, 0x10, 0x20, 0x30, 0x40, 0x50, 0x60, 0x70, 0x80, 0x90, 0xA0, 0xB0, 0xC0, 0xD0, 0xE0, 0xFA,
    0xAA, 0x10, 0x20, 0x30, 0x40, 0x50, 0x60, 0x70, 0x80, 0x90, 0xA0, 0xB0, 0xC0, 0xD0, 0xE0, 0xFA,
    0xAA, 0x10, 0x20, 0x30, 0x40, 0x50, 0x60, 0x70, 0x80, 0x90, 0xA0, 0xB0, 0xC0, 0xD0, 0xE0, 0xFA,
    0xAA, 0x10, 0x20, 0x30, 0x40, 0x50, 0x60, 0x70, 0x80, 0x90, 0xA0, 0xB0, 0xC0, 0xD0, 0xE0, 0xFA,
    0xAA, 0x10, 0x20, 0x30, 0x40, 0x50, 0x60, 0x70, 0x80, 0x90, 0xA0, 0xB0, 0xC0, 0xD0, 0xE0, 0xFA,
    0xAA, 0x10, 0x20, 0x30, 0x40, 0x50, 0x60, 0x70, 0x80, 0x90, 0xA0, 0xB0, 0xC0, 0xD0, 0xE0, 0xFA,
    0xAA, 0x10, 0x20, 0x30, 0x40, 0x50, 0x60, 0x70, 0x80, 0x90, 0xA0, 0xB0, 0xC0, 0xD0, 0xE0, 0xFA,
    0xAA, 0x10, 0x20, 0x30, 0x40, 0x50, 0x60, 0x70, 0x80, 0x90, 0xA0, 0xB0, 0xC0, 0xD0, 0xE0, 0xFA
  };
#elif USE_BIG_ARRAY == 3
// 512 byte
  const byte arrayToSaveToFlash[] = {
    0xAA, 0x10, 0x20, 0x30, 0x40, 0x50, 0x60, 0x70, 0x80, 0x90, 0xA0, 0xB0, 0xC0, 0xD0, 0xE0, 0xFA,
    0xAA, 0x10, 0x20, 0x30, 0x40, 0x50, 0x60, 0x70, 0x80, 0x90, 0xA0, 0xB0, 0xC0, 0xD0, 0xE0, 0xFA,
    0xAA, 0x10, 0x20, 0x30, 0x40, 0x50, 0x60, 0x70, 0x80, 0x90, 0xA0, 0xB0, 0xC0, 0xD0, 0xE0, 0xFA,
    0xAA, 0x10, 0x20, 0x30, 0x40, 0x50, 0x60, 0x70, 0x80, 0x90, 0xA0, 0xB0, 0xC0, 0xD0, 0xE0, 0xFA,
    0xAA, 0x10, 0x20, 0x30, 0x40, 0x50, 0x60, 0x70, 0x80, 0x90, 0xA0, 0xB0, 0xC0, 0xD0, 0xE0, 0xFA,
    0xAA, 0x10, 0x20, 0x30, 0x40, 0x50, 0x60, 0x70, 0x80, 0x90, 0xA0, 0xB0, 0xC0, 0xD0, 0xE0, 0xFA,
    0xAA, 0x10, 0x20, 0x30, 0x40, 0x50, 0x60, 0x70, 0x80, 0x90, 0xA0, 0xB0, 0xC0, 0xD0, 0xE0, 0xFA,
    0xAA, 0x10, 0x20, 0x30, 0x40, 0x50, 0x60, 0x70, 0x80, 0x90, 0xA0, 0xB0, 0xC0, 0xD0, 0xE0, 0xFA,
    0xAA, 0x10, 0x20, 0x30, 0x40, 0x50, 0x60, 0x70, 0x80, 0x90, 0xA0, 0xB0, 0xC0, 0xD0, 0xE0, 0xFA,
    0xAA, 0x10, 0x20, 0x30, 0x40, 0x50, 0x60, 0x70, 0x80, 0x90, 0xA0, 0xB0, 0xC0, 0xD0, 0xE0, 0xFA,
    0xAA, 0x10, 0x20, 0x30, 0x40, 0x50, 0x60, 0x70, 0x80, 0x90, 0xA0, 0xB0, 0xC0, 0xD0, 0xE0, 0xFA,
    0xAA, 0x10, 0x20, 0x30, 0x40, 0x50, 0x60, 0x70, 0x80, 0x90, 0xA0, 0xB0, 0xC0, 0xD0, 0xE0, 0xFA,
    0xAA, 0x10, 0x20, 0x30, 0x40, 0x50, 0x60, 0x70, 0x80, 0x90, 0xA0, 0xB0, 0xC0, 0xD0, 0xE0, 0xFA,
    0xAA, 0x10, 0x20, 0x30, 0x40, 0x50, 0x60, 0x70, 0x80, 0x90, 0xA0, 0xB0, 0xC0, 0xD0, 0xE0, 0xFA,
    0xAA, 0x10, 0x20, 0x30, 0x40, 0x50, 0x60, 0x70, 0x80, 0x90, 0xA0, 0xB0, 0xC0, 0xD0, 0xE0, 0xFA,
    0xAA, 0x10, 0x20, 0x30, 0x40, 0x50, 0x60, 0x70, 0x80, 0x90, 0xA0, 0xB0, 0xC0, 0xD0, 0xE0, 0xFA,
    0xAA, 0x10, 0x20, 0x30, 0x40, 0x50, 0x60, 0x70, 0x80, 0x90, 0xA0, 0xB0, 0xC0, 0xD0, 0xE0, 0xFA,
    0xAA, 0x10, 0x20, 0x30, 0x40, 0x50, 0x60, 0x70, 0x80, 0x90, 0xA0, 0xB0, 0xC0, 0xD0, 0xE0, 0xFA,
    0xAA, 0x10, 0x20, 0x30, 0x40, 0x50, 0x60, 0x70, 0x80, 0x90, 0xA0, 0xB0, 0xC0, 0xD0, 0xE0, 0xFA,
    0xAA, 0x10, 0x20, 0x30, 0x40, 0x50, 0x60, 0x70, 0x80, 0x90, 0xA0, 0xB0, 0xC0, 0xD0, 0xE0, 0xFA,
    0xAA, 0x10, 0x20, 0x30, 0x40, 0x50, 0x60, 0x70, 0x80, 0x90, 0xA0, 0xB0, 0xC0, 0xD0, 0xE0, 0xFA,
    0xAA, 0x10, 0x20, 0x30, 0x40, 0x50, 0x60, 0x70, 0x80, 0x90, 0xA0, 0xB0, 0xC0, 0xD0, 0xE0, 0xFA,
    0xAA, 0x10, 0x20, 0x30, 0x40, 0x50, 0x60, 0x70, 0x80, 0x90, 0xA0, 0xB0, 0xC0, 0xD0, 0xE0, 0xFA,
    0xAA, 0x10, 0x20, 0x30, 0x40, 0x50, 0x60, 0x70, 0x80, 0x90, 0xA0, 0xB0, 0xC0, 0xD0, 0xE0, 0xFA,
    0xAA, 0x10, 0x20, 0x30, 0x40, 0x50, 0x60, 0x70, 0x80, 0x90, 0xA0, 0xB0, 0xC0, 0xD0, 0xE0, 0xFA,
    0xAA, 0x10, 0x20, 0x30, 0x40, 0x50, 0x60, 0x70, 0x80, 0x90, 0xA0, 0xB0, 0xC0, 0xD0, 0xE0, 0xFA,
    0xAA, 0x10, 0x20, 0x30, 0x40, 0x50, 0x60, 0x70, 0x80, 0x90, 0xA0, 0xB0, 0xC0, 0xD0, 0xE0, 0xFA,
    0xAA, 0x10, 0x20, 0x30, 0x40, 0x50, 0x60, 0x70, 0x80, 0x90, 0xA0, 0xB0, 0xC0, 0xD0, 0xE0, 0xFA,
    0xAA, 0x10, 0x20, 0x30, 0x40, 0x50, 0x60, 0x70, 0x80, 0x90, 0xA0, 0xB0, 0xC0, 0xD0, 0xE0, 0xFA,
    0xAA, 0x10, 0x20, 0x30, 0x40, 0x50, 0x60, 0x70, 0x80, 0x90, 0xA0, 0xB0, 0xC0, 0xD0, 0xE0, 0xFA,
    0xAA, 0x10, 0x20, 0x30, 0x40, 0x50, 0x60, 0x70, 0x80, 0x90, 0xA0, 0xB0, 0xC0, 0xD0, 0xE0, 0xFA,
    0xAA, 0x10, 0x20, 0x30, 0x40, 0x50, 0x60, 0x70, 0x80, 0x90, 0xA0, 0xB0, 0xC0, 0xD0, 0xE0, 0xFA
  };
#elif USE_BIG_ARRAY == 2
// 256 byte
  const byte arrayToSaveToFlash[] = {
    0xAA, 0x10, 0x20, 0x30, 0x40, 0x50, 0x60, 0x70, 0x80, 0x90, 0xA0, 0xB0, 0xC0, 0xD0, 0xE0, 0xFA,
    0xAA, 0x10, 0x20, 0x30, 0x40, 0x50, 0x60, 0x70, 0x80, 0x90, 0xA0, 0xB0, 0xC0, 0xD0, 0xE0, 0xFA,
    0xAA, 0x10, 0x20, 0x30, 0x40, 0x50, 0x60, 0x70, 0x80, 0x90, 0xA0, 0xB0, 0xC0, 0xD0, 0xE0, 0xFA,
    0xAA, 0x10, 0x20, 0x30, 0x40, 0x50, 0x60, 0x70, 0x80, 0x90, 0xA0, 0xB0, 0xC0, 0xD0, 0xE0, 0xFA,
    0xAA, 0x10, 0x20, 0x30, 0x40, 0x50, 0x60, 0x70, 0x80, 0x90, 0xA0, 0xB0, 0xC0, 0xD0, 0xE0, 0xFA,
    0xAA, 0x10, 0x20, 0x30, 0x40, 0x50, 0x60, 0x70, 0x80, 0x90, 0xA0, 0xB0, 0xC0, 0xD0, 0xE0, 0xFA,
    0xAA, 0x10, 0x20, 0x30, 0x40, 0x50, 0x60, 0x70, 0x80, 0x90, 0xA0, 0xB0, 0xC0, 0xD0, 0xE0, 0xFA,
    0xAA, 0x10, 0x20, 0x30, 0x40, 0x50, 0x60, 0x70, 0x80, 0x90, 0xA0, 0xB0, 0xC0, 0xD0, 0xE0, 0xFA,
    0xAA, 0x10, 0x20, 0x30, 0x40, 0x50, 0x60, 0x70, 0x80, 0x90, 0xA0, 0xB0, 0xC0, 0xD0, 0xE0, 0xFA,
    0xAA, 0x10, 0x20, 0x30, 0x40, 0x50, 0x60, 0x70, 0x80, 0x90, 0xA0, 0xB0, 0xC0, 0xD0, 0xE0, 0xFA,
    0xAA, 0x10, 0x20, 0x30, 0x40, 0x50, 0x60, 0x70, 0x80, 0x90, 0xA0, 0xB0, 0xC0, 0xD0, 0xE0, 0xFA,
    0xAA, 0x10, 0x20, 0x30, 0x40, 0x50, 0x60, 0x70, 0x80, 0x90, 0xA0, 0xB0, 0xC0, 0xD0, 0xE0, 0xFA,
    0xAA, 0x10, 0x20, 0x30, 0x40, 0x50, 0x60, 0x70, 0x80, 0x90, 0xA0, 0xB0, 0xC0, 0xD0, 0xE0, 0xFA,
    0xAA, 0x10, 0x20, 0x30, 0x40, 0x50, 0x60, 0x70, 0x80, 0x90, 0xA0, 0xB0, 0xC0, 0xD0, 0xE0, 0xFA,
    0xAA, 0x10, 0x20, 0x30, 0x40, 0x50, 0x60, 0x70, 0x80, 0x90, 0xA0, 0xB0, 0xC0, 0xD0, 0xE0, 0xFA,
    0xAA, 0x10, 0x20, 0x30, 0x40, 0x50, 0x60, 0x70, 0x80, 0x90, 0xA0, 0xB0, 0xC0, 0xD0, 0xE0, 0xFA
  };
#elif USE_BIG_ARRAY == 1
// 128 byte
  const byte arrayToSaveToFlash[] = {
    0xAA, 0x10, 0x20, 0x30, 0x40, 0x50, 0x60, 0x70, 0x80, 0x90, 0xA0, 0xB0, 0xC0, 0xD0, 0xE0, 0xFA,
    0xAA, 0x10, 0x20, 0x30, 0x40, 0x50, 0x60, 0x70, 0x80, 0x90, 0xA0, 0xB0, 0xC0, 0xD0, 0xE0, 0xFA,
    0xAA, 0x10, 0x20, 0x30, 0x40, 0x50, 0x60, 0x70, 0x80, 0x90, 0xA0, 0xB0, 0xC0, 0xD0, 0xE0, 0xFA,
    0xAA, 0x10, 0x20, 0x30, 0x40, 0x50, 0x60, 0x70, 0x80, 0x90, 0xA0, 0xB0, 0xC0, 0xD0, 0xE0, 0xFA,
    0xAA, 0x10, 0x20, 0x30, 0x40, 0x50, 0x60, 0x70, 0x80, 0x90, 0xA0, 0xB0, 0xC0, 0xD0, 0xE0, 0xFA,
    0xAA, 0x10, 0x20, 0x30, 0x40, 0x50, 0x60, 0x70, 0x80, 0x90, 0xA0, 0xB0, 0xC0, 0xD0, 0xE0, 0xFA,
    0xAA, 0x10, 0x20, 0x30, 0x40, 0x50, 0x60, 0x70, 0x80, 0x90, 0xA0, 0xB0, 0xC0, 0xD0, 0xE0, 0xFA,
    0xAA, 0x10, 0x20, 0x30, 0x40, 0x50, 0x60, 0x70, 0x80, 0x90, 0xA0, 0xB0, 0xC0, 0xD0, 0xE0, 0xFA
  };
#elif USE_BIG_ARRAY == 0
  const byte arrayToSaveToFlash[] = { 0xFF, 0xFF};
#endif



byte storeReadData[4][13998]; // Her gemmer vi de to byte vi læser
byte storeRDSR  = 0x00;                // RDSR gemmes her. Omskriv til lokal variabel
byte RDSCUR     = 0x00;                   // RDSCUR gemmes her. Omskriv til lokal variabel

boolean WIP   = true; // Write In Progress - bit
boolean WEL   = true; // Write Enable Latch - bit
boolean BP0   = true; //   |
boolean BP1   = true; //   |-> Protection
boolean BP2   = true; //   |
boolean BP3   = true; //   |
boolean QE    = false;// Quad Enable
boolean SRWD  = true; // Status Register Write Protect 

boolean CP_SEL  = false;
boolean P_FAIL  = true;
boolean E_FAIL  = true;
boolean WPSEL   = true;


/*
#################################################################################################
 __      __   ____    _____   _____       _____   ______   _______   _    _   _____  
 \ \    / /  / __ \  |_   _| |  __ \     / ____| |  ____| |__   __| | |  | | |  __ \ 
  \ \  / /  | |  | |   | |   | |  | |   | (___   | |__       | |    | |  | | | |__) |
   \ \/ /   | |  | |   | |   | |  | |    \___ \  |  __|      | |    | |  | | |  ___/ 
    \  /    | |__| |  _| |_  | |__| |    ____) | | |____     | |    | |__| | | |     
     \/      \____/  |_____| |_____/    |_____/  |______|    |_|     \____/  |_|     
#################################################################################################
*/
void setup() {
  #if defined(ARDUINO_AVR_UNO)
    DDRB = DDRB|B00101111; // Set input/output pinmodes
  
  #elif defined(ARDUINO_SAM_DUE)
    pinMode(DUE_CLOCK,  OUTPUT);
    pinMode(DUE_MISO,   INPUT_PULLUP); 
    pinMode(DUE_MOSI,   OUTPUT);
    pinMode(DUE_SS,     OUTPUT);
    pinMode(DUE_BP,     OUTPUT);
  #endif
  

  Serial.begin(250000);

  // blockErase(adressenViHusker);
  /*
    Serial.print("sizeof(arrayToSaveToFlash):\t"); Serial.println(sizeof(arrayToSaveToFlash));
    Serial.print("sizeof(kickSample):\t"); Serial.println(sizeof(kickSample));
    Serial.print("sizeof(snareSample):\t"); Serial.println(sizeof(snareSample));
    Serial.print("sizeof(hatSample):\t"); Serial.println(sizeof(hatSample));
    Serial.print("sizeof(clapSample):\t"); Serial.println(sizeof(clapSample));
  */
  #if SHOULD_WIPE_WHOLE_CHIP == 1
    Serial.println("Full chip erase: ON");
    chipErase();
    Serial.println("Full chip erase:  DONE");
  #elif SHOULD_WIPE_WHOLE_CHIP == 0
    Serial.println("Full chip erase: OFF");
  #endif
  
  
  delay(2000);
  Serial.println("---Korer process om 2 sec.---");
  Serial.println();  
 /*
  for(int i = 0; i < sizeof(clapSample); i+=1){
    Serial.print("SRD["); Serial.print(i);   Serial.print("]:\t"); Serial.print(clapSample[i], HEX); Serial.print("\t");
    if(i%4 == 0){
      Serial.print("\n");
    }

  }
*/


  Serial.println("Unlocking chip");
  unlockChip();
  Serial.println("Chip unlocked");



/*##########################################
 *##########################################
 *  PROGRAMMERER                          ##
 *##########################################
 *##########################################
 */
 Serial.println("Programmerer");
  #if CHOOSE_PROGRAM_MODE == PP
  
  uint16_t tempVal = 0;
    /*
    0: #define KICK_ADRESS     0x010000
    1: #define SNARE_ADRESS    0x020000
    2: #define HAT_ADRESS      0x030000
    3: #define CLAP_ADRESS     0x040000
   */
   
    for(int sampleNr = 0; sampleNr < 4; sampleNr++){
      tempVal = (arrayLengths[sampleNr] - (arrayLengths[sampleNr] % 0xFF)) / 0xFF;
      uint32_t tempAdresse = 0;
      #if PRINT_WHERE_WE_ARE_WRITING == 1
        Serial.print("tempVal:\t"); Serial.println(tempVal);
      #endif 
      // Serial.print("arrayLengths[sampleNr]%256:\t"); Serial.println((arrayLengths[sampleNr] % 256));
      boolean lykkesDetAtSkrive = false;

      
      switch(sampleNr){
        
        
        case 0:
          // KICK sample
          tempAdresse = KICK_ADRESS;
          for(int antal256bytes = 0; antal256bytes < tempVal; antal256bytes++){
            
            do{
              #if SHOULD_WIPE_WHOLE_CHIP == 0
                #if SHOULD_BLOCK_ERASE == 1
                  blockErase(tempAdresse);  
                #endif
              #endif
              lykkesDetAtSkrive = pageProgram(tempAdresse, antal256bytes, sampleNr, 0xFF);  // Page program først!
            } while(!lykkesDetAtSkrive);
            tempAdresse += 0x000100;
          }
          lykkesDetAtSkrive = false;
          do{
            
            #if SHOULD_WIPE_WHOLE_CHIP == 0
              #if SHOULD_BLOCK_ERASE == 1
                  blockErase(tempAdresse);  
                #endif 
            #endif
            lykkesDetAtSkrive = pageProgram(tempAdresse, tempVal, sampleNr, (arrayLengths[sampleNr] % 0xFF));  // Page program først!
          } while(!lykkesDetAtSkrive);
          
          break;



        case 1:
          tempAdresse = SNARE_ADRESS;
          for(int antal256bytes = 0; antal256bytes < tempVal; antal256bytes++){
            do{
              #if SHOULD_WIPE_WHOLE_CHIP == 0
                #if SHOULD_BLOCK_ERASE == 1
                  blockErase(tempAdresse);  
                #endif  
              #endif
              lykkesDetAtSkrive = pageProgram(tempAdresse, antal256bytes, sampleNr, 0xFF);  // Page program først!
            } while(!lykkesDetAtSkrive);
            tempAdresse += 0x000100;
          }
          lykkesDetAtSkrive = false;
          do{
            #if SHOULD_WIPE_WHOLE_CHIP == 0
              #if SHOULD_BLOCK_ERASE == 1
                  blockErase(tempAdresse);  
                #endif 
            #endif
            lykkesDetAtSkrive = pageProgram(tempAdresse, tempVal, sampleNr, (arrayLengths[sampleNr] % 0xFF));  // Page program først!
          }while(!lykkesDetAtSkrive);
          
          break;



        case 2:
          tempAdresse = HAT_ADRESS;
          for(int antal256bytes = 0; antal256bytes < tempVal; antal256bytes++){
            do{
              #if SHOULD_WIPE_WHOLE_CHIP == 0
                #if SHOULD_BLOCK_ERASE == 1
                  blockErase(tempAdresse);  
                #endif 
              #endif
              lykkesDetAtSkrive = pageProgram(tempAdresse, antal256bytes, sampleNr, 0xFF);  // Page program først!
            } while(!lykkesDetAtSkrive);
            tempAdresse += 0x000100;
          }
          lykkesDetAtSkrive = false;
          do{
            #if SHOULD_WIPE_WHOLE_CHIP == 0
              #if SHOULD_BLOCK_ERASE == 1
                  blockErase(tempAdresse);  
                #endif 
            #endif
            lykkesDetAtSkrive = pageProgram(tempAdresse, tempVal, sampleNr, (arrayLengths[sampleNr] % 0xFF));  // Page program først!
          }while(!lykkesDetAtSkrive);
          
          break;



        case 3:
          tempAdresse = CLAP_ADRESS;
          for(int antal256bytes = 0; antal256bytes < tempVal; antal256bytes++){
            
            do{
              #if SHOULD_WIPE_WHOLE_CHIP == 0
                #if SHOULD_BLOCK_ERASE == 1
                  blockErase(tempAdresse);  
                #endif
              #endif
              lykkesDetAtSkrive = pageProgram(tempAdresse, antal256bytes, sampleNr, 0xFF);  // Page program først!
            } while(!lykkesDetAtSkrive);
            tempAdresse += 0x000100;
          }
          lykkesDetAtSkrive = false;
          do{
            
            #if SHOULD_WIPE_WHOLE_CHIP == 0
              #if SHOULD_BLOCK_ERASE == 1
                  blockErase(tempAdresse);  
                #endif
            #endif
            lykkesDetAtSkrive = pageProgram(tempAdresse, tempVal, sampleNr, (arrayLengths[sampleNr] % 0xFF));  // Page program først!
          }while(!lykkesDetAtSkrive);
          break;
      }

      
    }// for 
    
  #elif CHOOSE_PROGRAM_MODE == CP
    contProgram(adressenViHusker);
    
  #else
    #error Fejl i CHOOSE_PROGRAM_MODE!
  #endif
  
  Serial.println("Programmering done");
  adressenViHusker = START_ADRESS;
  highSS();

/*##########################################
 *##########################################
 *  LÆSER                                 ##
 *##########################################
 *##########################################
 */
  Serial.println("Laeser");
  #if CHOOSE_PROGRAM_MODE == PP
    
    for(int sampleNr = 0; sampleNr < NUMBER_OF_SAMPLES; sampleNr++){
      tempVal = (arrayLengths[sampleNr] - (arrayLengths[sampleNr] % 0xFF)) / 0xFF;
      
      switch(sampleNr){
        
        case 0:
          readTwoBytes(KICK_ADRESS, tempVal, sampleNr); 
          break;

        case 1:
            readTwoBytes(SNARE_ADRESS, tempVal, sampleNr); 
          break;

        case 2:
            readTwoBytes(HAT_ADRESS, tempVal, sampleNr); 
          break;

        case 3:          
            readTwoBytes(CLAP_ADRESS, tempVal, sampleNr); 
          break;
        
     // }
      }// switch
    }// for

   
  #elif CHOOSE_PROGRAM_MODE == CP
    uint8_t tempVal = ((sizeof(arrayToSaveToFlash) - (sizeof(arrayToSaveToFlash) % 0xFF)) / 0xFF);
    Serial.print("tempVal:\t"); Serial.println(tempVal);
    for(int antal256bytes = 0; antal256bytes < tempVal; antal256bytes++){
      readTwoBytes(adressenViHusker, antal256bytes); 
      adressenViHusker +=0x000100;
    }
  #else
    #error Fejl i CHOOSE_PROGRAM_MODE!
  #endif
  Serial.println("Laesning done");
  highSS();
/*##########################################
 *##########################################
 *  VERIFICERER                           ##
 *##########################################
 *##########################################
 */
  Serial.println("Verificerer");

  for(int i = 0; i < NUMBER_OF_SAMPLES; i++){
    if(compareData(i)){
      Serial.print("\tFEJL i samlple:\t"); Serial.println(i);
    } else {
      Serial.print("\tGODT i sample:\t"); Serial.println(i);
    }
  }
  
  #if PRINT_THE_READ_DATA == 1
    printReadData();
  #endif
 Serial.println("Verificering done");  
 highSS();

#if SHOULD_LOCK_AFTER_PROGRAMMING == 1
  Serial.println("Locking the chip");
  lockChip();
  Serial.println("Chip locked");
#endif


 
 Serial.println("---Process done--------------");
}

/*
#################################################################################################
 __      __   ____    _____   _____          _         ____     ____    _____  
 \ \    / /  / __ \  |_   _| |  __ \        | |       / __ \   / __ \  |  __ \ 
  \ \  / /  | |  | |   | |   | |  | |       | |      | |  | | | |  | | | |__) |
   \ \/ /   | |  | |   | |   | |  | |       | |      | |  | | | |  | | |  ___/ 
    \  /    | |__| |  _| |_  | |__| |       | |____  | |__| | | |__| | | |     
     \/      \____/  |_____| |_____/        |______|  \____/   \____/  |_|      
#################################################################################################
*/
void loop() {
  while(1){};
//  
//  // Resetter globale variabler
//  for(int i = 0; i < sizeof(arrayToSaveToFlash); i++){
//    storeReadData[i] = 0x00;
//  }
//  storeRDSR         = 0x00;
//  RDSCUR            = 0x00;
//
//  
//  
//  Serial.println("Loop");
//  
//  // pulseBreakPin();
//  // Serial.print("adressenViHusker:\t"); Serial.println(adressenViHusker, HEX);
//  
//  
//  
//  
//  while(isBlocksLocked()){
//    // Der er låst for nogle blokke. Lås dem op!
//  }
//
///*  
//  readTwoBytes(); // Herefter læser vi ting
//  
//
//  compareData(); // Her sammenlignes det læste med det skrevne. Er der forskel bliver Serial-porten spammet! (Bool)
//*/
//  
//
//  if(adressenViHusker >= STOP_ADRESS && STOP_ADRESS != 0x00000){
//      while(1); // Skal der kun skrives til et område? 
//  } else {
//    adressenViHusker += (sizeof(arrayToSaveToFlash));
//  }
//  
//  
//  adressenViHusker = START_ADRESS;
//  delay(2000);
}


/*
#################################################################################################
  _______    ____    _____          ______   _    _   _   _    _____     
 |__   __|  / __ \  |  __ \        |  ____| | |  | | | \ | |  / ____|    
    | |    | |  | | | |__) |       | |__    | |  | | |  \| | | |         
    | |    | |  | | |  ___/        |  __|   | |  | | | . ` | | |         
    | |    | |__| | | |            | |      | |__| | | |\  | | |____   _ 
    |_|     \____/  |_|            |_|       \____/  |_| \_|  \_____| (_)
#################################################################################################
*/
void readTwoBytes(uint32_t adress, uint8_t numberOfPagesToRead, byte sampleNr){
  /*  Læser lige så mange bytes som der er blevet skrevet fra
   *   arrayToSaveToFlash[] og gemmer dem i storeReadData[]
   *   Disse kan senere sammenlignes med compareData().
   *   
   *   The sequence of issuing READ instruction is: 
   *   1 → CS# goes low
   *   2 → sending READ instruction code
   *   3 → 3-byte address on SI 
   *   4 → data out on MISO
   *   5 → to end READ operation can use CS# to high at any time during data out. 
   *   
   *   Kilde: Datablad pp. 19
   */

  #if PRINT_WHERE_WE_ARE_READING  == 1
    Serial.print("adress:\t"); Serial.print(adress, HEX); Serial.print("\tnumberPagesToRead:\t"); Serial.println(numberOfPagesToRead);
  #endif
  // Step 1
  lowSS();

  // Step 2
  sendReadInstruction();
  
  // Step 3
  sendAdress(adress); 

  // Serial.print("Læser til:\t"); Serial.println(0xFF + (numberOfPagesToRead * 0xFF));
  
  // Step 4 
  int reading = 1;
  for(int i = 0; i < (0xFF + (numberOfPagesToRead * 0xFF)+ 1); i++){
    if(i % 0xFF == 0 && i != 0){
      readOneByteSPI();
    }
    storeReadData[sampleNr][i] = readOneByteSPI();
    // delayMicroseconds(0);
    Serial.print("i:\t"); Serial.print(i); Serial.print("\t"); Serial.print(storeReadData[sampleNr][i], HEX); Serial.print("\t"); Serial.println(arrayToSaveToFlash[sampleNr][i],HEX);
    reading++;
  }
  Serial.print("reading:\t"); Serial.println(reading);
  
  // Step 5
  highSS();  
  
  // Vi er done
}

boolean pageProgram(uint32_t adress, byte numberOfPagesToWrite, int sampleSelection, byte numberOfBytes){
  /* 
   * Skriver data til adressen adressenViGemmer. 
   *  Området der skrives til SKAL være slettet før
   *  ændringer kan bruges til noget, da Page Program 
   *  ikke kan skrive '1'ere, men kun '0'ere.
   *  
   *  Returnerer 1 hvis det lykkes
   *             0 hvis fejl
   *  
   *  The sequence of issuing PP instruction is: 
   *  → Write Enable
   *  
   *  → CS# goes low
   *  → sending PP instruction code
   *  → 3-byte address on SI
   *    → at least 1-byte on data on SI
   *  → CS# goes high.  
   */
  #if PRINT_WHERE_WE_ARE_WRITING == 1
    Serial.print("adress:\t"); Serial.print(adress, HEX); Serial.print("\tsampleSelection:\t"); Serial.println(sampleSelection);
  #endif
    //    De adresse-variabler vi har
    // uint32_t adressenViHusker = START_ADRESS;
      // Serial.print("adress:\t"); Serial.print(adress); Serial.print("\tnumberPage:\t"); Serial.println(numberOfPagesToWrite);

    // Giver write-enable og tjekker den er klar
    do{                     
      writeEnable();        // Write Enable 
      lowSS();
      readStatusRegister(); // Write Enable 
      highSS();
      delayMicroseconds(5);
    }while(!WEL || WIP);    // Tjekker den er klar

    // Cycler clocken så chippen er klar over der nu kommer en kommando
    cycleSS();

    transmitOneByteSPI(0x02);     // Sender kommandoen om Page Program


    sendAdress(adress); // Sender adressen

  
    // Sender data afsted
    for(int i = 0; i <= numberOfBytes; i++){
      byte tempByte = arrayToSaveToFlash[sampleSelection][(i + (numberOfPagesToWrite * 0xFF))];
      transmitOneByteSPI(tempByte);
      // Serial.print("Skriver:\t"); Serial.println(arrayToSaveToFlash[i], HEX);
      // Serial.print("Skriver til:\t"); Serial.println(i + (numberOfPagesToWrite * 0xFF));
    }
    highSS();// Høj SS herefter

    // Nu gemmer chippen sager!
    // delay(5);// Ifølge databladet (tPP) er chippen max 5 ms om at gemme

    
    // Vent på Write-In-Progress bitten bliver 0 igen
    waitUntilWorkIsDone();

    
    do{                     // Step 4
      writeDisable();
      readStatusRegister(); // Step 4
      readRDSCUR();           // Step 5
      // Serial.println("Venter slut");
      delayMicroseconds(1);
    }while(WIP || WEL);            // Step 4


    // writeDisable();
    highSS(); // Vi er done nu

    
    if(P_FAIL || E_FAIL){
      // Det lykkedes ikke at skrive
      throwErrorMessage();
      return false;
    } else {
      // Det lykkedes at skrive
      return true;
    }
    
    /*
     * Mon der burde låses igen? hmm
     */
}

void writeStuff(){
  /* DENNE FUNKTION BRUGES IKKE MERE
   *  
   *  SÅDAN SER WRITE-CYCLEN UD!
   * ----------------------------
   * WREN                                         0x06
   * RDSR                                         0x05
   *  ↳ WREN=1?                                    Bit 1 fra RDSR
   * Contenious program mode                      0xAD 
   *  ↳ Adressen                                   ADD(24)
   *  ↳ Write data                                 DATA(16)
   * RDSR command                                 0x05
   *  ↳ WIP = 0?                                   Bit 0 fra RDSR      
   * RDSCUR command - Tjek om det lykkedes        0x2B
   *  ↳ P_FAIL / E_FAIL = 1?                       FORFRA! ALT ER DONE! :o
   * WREN = 0   0x04
   *
   * ------------------------------------------------------------------------------
   * | TEKST-version:                                                             |
   * ------------------------------------------------------------------------------
   * || Write-Enable sendes (step 1) hvorefter vi afventer chippen er klar        |
   * || til at modtage data (step 2). Dette tjekkes med bit 1 fra                 |
   * || statusregistret (RDSR). Nu sættes chippen i Continously-Program (CP) mode |
   * || (step 3). Hvordan denne fungerer kan læses i funktionen                   |
   * || continouslyProgram(). Når continouslyProgram() er færdig med at skrive    |
   * || data til flash'en tjekkes bit 0 fra RDSR (step 4) Er denne = 0, er        |
   * || chippen færdig med at gemme og derfor klar til nye ting.                  |
   * || For at tjekke om dataen blev skrevet til flashen tjekkes P_FAIL & E_FAIL  |
   * || som er bit 5 & 6 fra RDSR. Er disse HIGH mislykkedes hele operationen og  |
   * || hele writeStuff() skal køres forfra.                                      |
   * ------------------------------------------------------------------------------
   */
   
    

    // Vent på Write-Enable-Latch bliver 1
    do{                     
      writeEnable();        // Step 1  
      writeStatusRegister();
      readStatusRegister(); // Step 2
    // Serial.print("RDSR: "); Serial.println(storeRDSR, BIN);
    }while(!WEL);           // Step 2

      // setGBULK();
    RDBLOCK();



    
    // Fyr data afsted
    // continouslyProgram();   // Step 3 <-- EKSISTERER IKKE MERE!


    // Vent på Write-In-Progress bitten bliver 0 igen
    do{                     // Step 4
      readStatusRegister(); // Step 4
      if(WIP){
        Serial.println("WIP 1");
      }
    }while(WIP);            // Step 4

    readRDSCUR();           // Step 5
    
    // Her tjekker vi om det faktisk lykkedes
    if(bitRead(RDSCUR, 5) == 1 || bitRead(RDSCUR, 6) == 1){
      // The programming failed! 
      throwErrorMessage();
    } else {
      // Ting virkede!
      // Denne else er overflødig
      Serial.println("Ting virkede!");
    }
    // setGBLK();
    writeDisable();
    highSS(); // Vi er done nu
}
/*
void contProgram(uint32_t adress){
  do{
    waitUntilWEL();
    waitUntilWorkIsDone();
    lowSS();
    transmitOneByteSPI(0xAD); // CP command
    delayMicroseconds(1);
    
    
    
    sendAdress(adressenViHusker);
    
    transmitOneByteSPI(arrayToSaveToFlash[adressenViHusker]);
    transmitOneByteSPI(arrayToSaveToFlash[adressenViHusker + 1]);
    cycleSS();
    readRDSCUR();
    // Serial.print("RDSCUR:\t"); Serial.println(RDSCUR, BIN);
  }while(!CP_SEL);
    
    adressenViHusker += 2;
    if(sizeof(arrayToSaveToFlash) % 2 > 0){
      // Det er et ulige antal, så der skal sendes en 0xFF byte til sidst
      
    } else {
      // Det er et lige antal, så ingen dummy-byte
      int i = 2;
      while(i < sizeof(arrayToSaveToFlash) && CP_SEL){
      
        // Loopet der skal sende bytes afsted
        cycleSS();
        sendContinouslyProgramCommand();   
        
        transmitOneByteSPI(arrayToSaveToFlash[i]);
        transmitOneByteSPI(arrayToSaveToFlash[i + 1]);
        
        
        // Skal slutte med:
        // Serial.print("i:\t"); Serial.println(i);
        // Serial.print("adressenViHusker1:\t"); Serial.println(adressenViHusker, HEX);
        // Serial.print("Forskel:\t"); Serial.println(sizeof(arrayToSaveToFlash) > (START_ADRESS - adressenViHusker));
        i += 2;
      }
    }

    writeDisable();
    readStatusRegister();
    readRDSCUR();
    // Serial.println("Jeg er sej");
}
*/

void lockChip(){
  do{
    readStatusRegister();
    writeEnable();
    lowSS();
    // Serial.print("Stuff: "); Serial.println(B00111100 | storeRDSR, BIN);
    transmitOneByteSPI(0x01);
    
    transmitOneByteSPI(B00111100 | storeRDSR);
    highSS();
    
    delayMicroseconds(1);
  } while(!isBlocksLocked());
  writeDisable();
}


void unlockChip(){
  do{
    readStatusRegister();
    writeEnable();
    lowSS();
    // Serial.print("Stuff: "); Serial.println(B11000011 & storeRDSR, BIN);
    transmitOneByteSPI(0x01);
    
    transmitOneByteSPI(B11000011 & storeRDSR);
    highSS();
    
    delayMicroseconds(1);
  } while(isBlocksLocked());
  writeDisable();
}


/*
#################################################################################################
 __          __  _____    _____   _______   ______         ______   _    _   _   _    _____     
 \ \        / / |  __ \  |_   _| |__   __| |  ____|       |  ____| | |  | | | \ | |  / ____|    
  \ \  /\  / /  | |__) |   | |      | |    | |__          | |__    | |  | | |  \| | | |         
   \ \/  \/ /   |  _  /    | |      | |    |  __|         |  __|   | |  | | | . ` | | |         
    \  /\  /    | | \ \   _| |_     | |    | |____        | |      | |__| | | |\  | | |____   _
     \/  \/     |_|  \_\ |_____|    |_|    |______|       |_|       \____/  |_| \_|  \_____| (_)
#################################################################################################
*/
          /* SÅDAN SER WRITE-CYCLEN UD
           * -----------------------------------------------------------------------------------
           *  Step | Navn                                        | Kommando
           * 1:----|→ WREN                                         0x06
           * 2:----|→ RDSR                                         0x05
           *       |  ↳ WREN=1?                                    Bit 1 fra RDSR
           * 3:----|→ Page Program                                 0x02 
           *       |  ↳ Adressen                                   ADD(24)
           *       |  ↳ Write data                                 DATA(AntalBytes) Fri længde
           * 4:----|→ RDSR command                                 0x05
           *       |  ↳ WIP = 0?                                   Bit 0 fra RDSR      
           * 5:----|→ RDSCUR command - Tjek om det lykkedes        0x2B
           *       |  ↳ P_FAIL / E_FAIL = 1?                       FORFRA! ALT ER RISTET! :o
           * 6:----|→ WREN = 0   0x04
           */


void writeEnable(){
  /* 
   * Skrive Write Enable til chippen
   * 
   *  The sequence of issuing WREN instruction is: 
   *  1 → CS# goes low
   *  2 → sending WREN instruction code
   *  3 → CS# goes high.
   * 
   *  Kilde: Datablad pp. 16
   */
   
  lowSS();                  // Step 1
  transmitOneByteSPI(0x06); // Step 2
  highSS();                 // Step 3
} // writeEnable

void writeDisable(){
  /* 
   * Skriver Write Disable til chippen
   *  
   *  The sequence of issuing WRDI instruction is: 
   *  1 → CS# goes low
   *  2 → sending WRDI instruction code
   *  3 → CS# goes high. 
   *  
   *  Kilde: Datablad pp. 16
   */
   
  lowSS();
  transmitOneByteSPI(0x04); // WEL = 0
  highSS();
} // writeDisable



void sendContinouslyProgramCommand(){
  transmitOneByteSPI(0xAD); // CP command
  
}


/*
#################################################################################################
  _____    ______              _____          ______   _    _   _   _    _____     
 |  __ \  |  ____|     /\     |  __ \        |  ____| | |  | | | \ | |  / ____|    
 | |__) | | |__       /  \    | |  | |       | |__    | |  | | |  \| | | |         
 |  _  /  |  __|     / /\ \   | |  | |       |  __|   | |  | | | . ` | | |         
 | | \ \  | |____   / ____ \  | |__| |       | |      | |__| | | |\  | | |____   _ 
 |_|  \_\ |______| /_/    \_\ |_____/        |_|       \____/  |_| \_|  \_____| (_)
#################################################################################################
*/
                /*  The sequence of issuing READ instruction is: 
                 *   1 → CS# goes low
                 *   2 → sending READ instruction code
                 *   3 → 3-byte address on SI 
                 *   4 → data out on SO
                 *   5 → to end READ operation can use CS# to high at any time during data out. 
                 *   
                 *   Kilde: Datablad pp. 19
                 */

void sendReadInstruction(){
  /*  Send read-kommandoen til flashen
   */
  transmitOneByteSPI(0x03); // Read command
}

void readData(char lenghtInBytes){
  /*  Denne funktion skal læse lengthInBytes-antal bytes
   *   fra flash'en
   */
}


/*
#################################################################################################
  __  __   _____   _____          ______   _    _   _   _    _____     
 |  \/  | |_   _| |  __ \        |  ____| | |  | | | \ | |  / ____|    
 | \  / |   | |   | |  | |       | |__    | |  | | |  \| | | |         
 | |\/| |   | |   | |  | |       |  __|   | |  | | | . ` | | |         
 | |  | |  _| |_  | |__| |       | |      | |__| | | |\  | | |____   _ 
 |_|  |_| |_____| |_____/        |_|       \____/  |_| \_|  \_____| (_)
#################################################################################################
*/
void sendAdress(uint32_t adress){
  /*
   * Sender adressen over SPI
   *  Ignorer mere end 24 bit, så ingen problemer der
   */

  transmitOneByteSPI((adress >> 16) & 0xFF);  // ----|
  transmitOneByteSPI((adress >> 8 ) & 0xFF);  //     |-> START Adressen i 24 bit
  transmitOneByteSPI(adress&0x0000FF);        // ----|     á 8 bit pr. gang
}


byte readOneByteSPI(){
  /*
   * Denne funktion læser én byte fra SPI
   */
   
  byte tempInputData = 0x00;
  // Læs data
  lowMosi();
  // cycleClock();    DA DER LIGE ER BLEVET SENDT EN KOMMANDO ER DER
  //                  IKKE BEHOV FOR AT CYCLE CLOCKEN
  #if defined(ARDUINO_AVR_UNO)
    
    for(int k = 7; k >= 0; k--){
      highClock();
      // Hvis data in er HIGH efter falling-edge clock
      if(bitRead(PINB, 4)){
        bitSet(tempInputData, k);  // Sæt den pågældende bit high
      } 
      lowClock();
    }// for
    
  #elif defined(ARDUINO_SAM_DUE)
    
    for(int k = 7; k >= 0; k--){
      highClock();
      if(digitalRead(DUE_MISO)){
        bitSet(tempInputData, k);
      }
      lowClock();
    }
    
  #endif
  
  return tempInputData;
}

void transmitOneByteSPI(byte data){
  /*
   * Sender én byte via. SPI
   */
  
  // DDRB = DDRB|B00101111; // Set as output - Overflødig
  for(int i = 7; i >= 0; i--){
    if(bitRead(data,i) == 1){
      highMosi();
    } else {
      lowMosi();
    }
    cycleClock();   
  }// for
 
}


void readRDSCUR(){
  /* 
   *  Read Security Register
   *    Denne bruges til at læse sikkerheds-registret
   *    Bits betyder følgende:
   *      0:  4k-bit factory lock
   *      1:  Lockdown 
   *      2:  Reserved
   *      3:  Reserved
   *      4:  Continously Program Mode (CP Mode)
   *      5:  P_FAIL  (Program lykkes hvis denne er 0)
   *      6:  E_FAIL  (Erase lykkes hvis denne er 0)
   *      7:  Write Protection Select (WPSEL. 0 = Normal mode)
   */
  lowSS();
  transmitOneByteSPI(0x2B);
  RDSCUR = readOneByteSPI();
  highSS();

  CP_SEL = bitRead(RDSCUR, 4);
  P_FAIL = bitRead(RDSCUR, 5);
  E_FAIL = bitRead(RDSCUR, 6);
  WPSEL  = bitRead(RDSCUR, 7);
}

void readStatusRegister(){
  /* The sequence of issuing RDSR instruction is: 
   *  1 → CS# goes low
   *  2 → sending RDSR instruction code
   *  3 → Status Register data out on MISO 
   *  
   *  Kilde: Databled pp. 17
   */
  highSS();                     // Cycle
  lowSS();                      // Slave-select
  transmitOneByteSPI(0x05);     // Step 2
  storeRDSR = readOneByteSPI(); // Step 3
  
  WIP   = bitRead(storeRDSR, 0);
  WEL   = bitRead(storeRDSR, 1);
  BP0   = bitRead(storeRDSR, 2);
  BP1   = bitRead(storeRDSR, 3);
  BP2   = bitRead(storeRDSR, 4);
  BP3   = bitRead(storeRDSR, 5);
  QE    = bitRead(storeRDSR, 6);
  SRWD  = bitRead(storeRDSR, 7);
  
  highSS();
}

void RDBLOCK(){
  /*  Send en RDBLOCK kommando og gem svaret
   *   Denne kan kun bruges hvis WPSEL = 1
   *    1 = Låst
   *    0 = ulåst
   *    
   *    Funktionen bruges ikke
   *  
   *  → CS# goes low 
   *  → send RDBLOCK (3Ch) instruction 
   *  → send 3 address bytes to assign one block on SI pin  (24 bit)
   *  → read block's protection lock status bit on SO pin   ( 1 bit)
   *  → CS# goes high. 
   */
  lowSS();
  transmitOneByteSPI(0x3C);
  sendAdress(adressenViHusker);
  byte tempRDBLOCK = 0x00;
  tempRDBLOCK = readOneByteSPI();
  tempRDBLOCK = tempRDBLOCK & 0xFF;
  
  if(tempRDBLOCK == 0x01){
    // Der er låst!
    Serial.print("L--RDBLOCK:\t"); Serial.println(tempRDBLOCK, BIN);
  } else {
    // Der er IKKE låst!
    Serial.print("UL-RDBLOCK:\t"); Serial.println(tempRDBLOCK, BIN);
  }
  highSS();
}

void blockErase(uint32_t adress){
  /*
   * Sletter en hel block (64k-byte)
   *  Er blokken låst (BP0-BP3 = 1, eller WPSEL = 1) sker der ingen ting
   *  
   */
  #if DEBUG_BLOCK_ERASE == 1
   Serial.print("blockErase: adress:\t"); Serial.println(adress, HEX);
  #endif
  lowSS();
  writeEnable();
  lowSS();
  transmitOneByteSPI(0xD8);
  sendAdress(adress);
  highSS();
  
  waitUntilWorkIsDone();
}

void waitUntilWorkIsDone(){
  /*
   * Denne funktion venter på WIP bliver LOW
   */
  #if DEBUG_WHILE_LOOPS == 1
    Serial.println("waitUntilWorkIsDone\n");
  #endif
  do{
    readStatusRegister();
    highSS(); // High SS afterwards
    delayMicroseconds(1);
  } while(WIP); 
}

void waitUntilWEL(){
  #if DEBUG_WHILE_LOOPS == 1
    Serial.println("waitUntilWEL\n");
  #endif 
  do{
    writeEnable();
    delayMicroseconds(1);
    readStatusRegister();
  }while(!WEL);
}

void setGBULK(){
  /*  
   *  Gang Block Unlock
   *    Låser en hel blok op
   *    
   *   Flow: 
   * → CS# goes low 
   * → send GBULK (0x98) instruction 
   * → CS# goes high <- DEN SKAL VÆRE SAMMEN MED DEN SIDSTE DATA-BIT!
   */
#if defined(ARDUINO_AVR_UNO)
  lowSS();

  highMosi();//   |
  cycleClock();// |-> Bit 7

  lowMosi();//    |
  cycleClock();// |-> Bit 6

  lowMosi();//    |
  cycleClock();// |-> Bit 5

  highMosi();//   |
  cycleClock();// |-> Bit 4
  
  highMosi();//   |
  cycleClock();// |-> Bit 3

  lowMosi();//    |
  cycleClock();// |-> Bit 2

  lowMosi();//    |
  cycleClock();// |-> Bit 1

  lowMosi();//    |-> Bit 0
  PORTB |=    B00100100;// Her settes clocken OG SS samtidig
  PORTB &=    B11011111;// Low clock
  
  // Ikke brug for highSS() bagefter


  /* Bruges til skabelon til Bit 0
    PORTB &=    B11110111;   // Set low MOSI
    PORTB |=    B00100000;  // Set it to high // CLOCK
    PORTB |=    B00000100;  // Set it to high // SS
  */
  #elif defined(ARDUINO_SAM_DUE)
    // GØR TING FOR DUO-EN
    lowSS();
    transmitOneByteSPI(0x98);
    highSS();
      
  #endif
}

void setGBLK(){
  /*  Gang Block Lock
   *   Låser en hel blok
   *   
   *   Flow: 
   * → CS# goes low 
   * → send GBLK (0x7E) instruction 
   * → CS# goes high <- DEN SKAL VÆRE SAMMEN MED DEN SIDSTE DATA-BIT!
   */
#if defined(ARDUINO_AVR_UNO)
  lowSS();

  lowMosi();//    |
  cycleClock();// |-> Bit 7

  highMosi();//   |
  cycleClock();// |-> Bit 6

  highMosi();//   |
  cycleClock();// |-> Bit 5

  highMosi();//   |
  cycleClock();// |-> Bit 4
  
  highMosi();//   |
  cycleClock();// |-> Bit 3

  highMosi();//   |
  cycleClock();// |-> Bit 2

  highMosi();//   |
  cycleClock();// |-> Bit 1

  lowMosi();//    |-> Bit 0
  PORTB |=    B00100100;// Her settes clocken OG SS samtidig
  PORTB &=    B11011111;// Low clock
  
  // No need for highSS();


  /* Bruges til skabelon til Bit 0
    PORTB &=    B11110111;   // Set low MOSI
    PORTB |=    B00100000;  // Set it to high // CLOCK
    PORTB |=    B00000100;  // Set it to high // SS
  */
  #elif defined(ARDUINO_SAM_DUE)
    // GØR TING FOR DUO-EN
    lowSS();
    transmitOneByteSPI(0x7E);
    highSS();
  #endif
  
  
}



boolean isBlocksLocked(){
  /*
   * Tjekker om nogle blocke er låst.
   *  Returnerer true hvis låst
   *  Returnerer false hvis ulåst
   */
  if(BP0 ||  BP1 || BP2 || BP3){
    // Serial.println("Something is protected");
    return true;
  } else {
    // Serial.println("NOT protected");
    return false;
  }
}

void writeStatusRegister(){
  /* 
   *  Write Status Register
   *  
   *  The sequence of issuing WRSR instruction is: 
   *  → CS# goes low
   *  → sending WRSR instruction code
   *  → Status Register data on SI
   *  → CS# goes high.
   * 
   */
  #if defined(ARDUINO_AVR_UNO)
    lowSS();
    do{                     
        writeEnable();        //   
        readStatusRegister(); // 
      // Serial.print("RDSR: "); Serial.println(storeRDSR, BIN);
      }while(!WEL);           // 
    
    highSS();
    lowSS();
    
    transmitOneByteSPI(0x01);
    
  
    highMosi();//   |
    cycleClock();// |-> Bit 7
  
    lowMosi();//    |
    cycleClock();// |-> Bit 6
  
    lowMosi();//    |
    cycleClock();// |-> Bit 5
  
    lowMosi();//    |
    cycleClock();// |-> Bit 4
    
    lowMosi();//    |
    cycleClock();// |-> Bit 3
  
    lowMosi();//    |
    cycleClock();// |-> Bit 2
  
    highMosi();//   |
    cycleClock();// |-> Bit 1
  
    lowMosi();//    |-> Bit 0
    PORTB |=    B00100100;// Her settes clocken OG SS samtidig
    PORTB &=    B11011111;// Low clock
    // SS er high
    
  #elif defined(ARDUINO_SAM_DUE)
    // GØR TING FOR DUO-EN
      
  #endif

  
}
  
void chipErase(){
  /*
   * Denne funktion sletter hele chippen
   * alts, skriver '1'ere til alle pladser
   */
  #if DEBUG_WHILE_LOOPS == 1
    Serial.println("Chip Erase\n");
  #endif
  highSS();
  waitUntilWEL();
  lowSS();
  transmitOneByteSPI(0xC7);
  highSS();
  delay(10000);
  waitUntilWorkIsDone();
}
/*
#################################################################################################
   ____    _______   _    _   ______   _____          ______   _    _   _   _    _____     
  / __ \  |__   __| | |  | | |  ____| |  __ \        |  ____| | |  | | | \ | |  / ____|    
 | |  | |    | |    | |__| | | |__    | |__) |       | |__    | |  | | |  \| | | |         
 | |  | |    | |    |  __  | |  __|   |  _  /        |  __|   | |  | | | . ` | | |         
 | |__| |    | |    | |  | | | |____  | | \ \        | |      | |__| | | |\  | | |____   _ 
  \____/     |_|    |_|  |_| |______| |_|  \_\       |_|       \____/  |_| \_|  \_____| (_)
#################################################################################################
*/
void throwErrorMessage(){
  /*
   * Skriver bare en fejlmeddelelse
   */
  Serial.println("-------------------------------------------");
  Serial.println("Something went wrong because you are stupid");
  Serial.println("-------------------------------------------");
  Serial.print("RDSCUR:\t\t");      Serial.println(RDSCUR, BIN);
  Serial.print("storeRDSR:\t");  Serial.println(storeRDSR, BIN);
  Serial.println("-------------------------------------------");
}

void printReadData(){   
  /*
   * Skriver den læste data til Serial
   */
  
  Serial.println();  
  for(int sampleNr = 0; sampleNr < NUMBER_OF_SAMPLES; sampleNr++){
    for(int i = 0; i < (SAMPLE_LENGTH); i+=2){
      Serial.print("SRD["); Serial.print(i); Serial.print("]:\t"); Serial.print(storeReadData[sampleNr][i], HEX); Serial.print("\t");
      Serial.print("SRD["); Serial.print(i+1); Serial.print("]:\t"); Serial.print(storeReadData[sampleNr][i+1], HEX); Serial.print("\t");
      if(i % 4 == 0){
        Serial.println();
      }
    }
  }
  
}

bool compareData(int sampleNr){
  /*
   * Sammenligner det læste med det skrevne.
   *  Returnerer:
   *    false:  Alt er godt
   *    true :  Det lårt
   *    arrayLengths
   */

  bool mistake = false;

  for(int i = 0; i < arrayLengths[sampleNr]; i++){  
    if(storeReadData[sampleNr][i] != arrayToSaveToFlash[sampleNr][i]){
      #if DEBUG_WRONG_DATA  ==  1
        Serial.print("SRD[");Serial.print(sampleNr); Serial.print("]["); Serial.print(i); Serial.print("]\t"); Serial.print(storeReadData[sampleNr][i], HEX); Serial.print("\t!=\t");
        Serial.print(arrayToSaveToFlash[sampleNr][i], HEX); Serial.print("\n");
      #endif
      mistake = true;
    }
  }
  
  #if DEBUG_WRONG_DATA == 1
    Serial.print("CD returning:\t"); Serial.println(mistake);
  #endif
  
  return mistake;// mistake;
  
}


/*
#################################################################################################
  ____                _____   _____    _____         ______   _    _   _   _    _____     
 |  _ \      /\      / ____| |_   _|  / ____|       |  ____| | |  | | | \ | |  / ____|    
 | |_) |    /  \    | (___     | |   | |            | |__    | |  | | |  \| | | |         
 |  _ <    / /\ \    \___ \    | |   | |            |  __|   | |  | | | . ` | | |         
 | |_) |  / ____ \   ____) |  _| |_  | |____        | |      | |__| | | |\  | | |____   _ 
 |____/  /_/    \_\ |_____/  |_____|  \_____|       |_|       \____/  |_| \_|  \_____| (_)
#################################################################################################
*/
/*
#define DUE_CLOCK   13
#define DUE_MISO    12
#define DUE_MOSI    11
#define DUE_SS      10
 */

void pulseBreakPin(){
  #if defined(ARDUINO_AVR_UNO)
    
  
  #elif defined(ARDUINO_SAM_DUE)
    digitalWrite(DUE_BP,   HIGH);
    delayMicroseconds(1);
    digitalWrite(DUE_BP, LOW);
  #endif
}

void emptyBytes(){
  lowMosi();
  for(int i = 8; i > 0; i--){
    cycleClock();
  }
}


void cycleSS(){
  highSS();
  lowSS();
}

void cycleClock(){
  highClock();
  delayMicroseconds(1);
  lowClock();
  delayMicroseconds(1);
}

void lowMosi(){
  //DDRB = DDRB|B00101111;  // Set as output - Overflødig
  #if defined(ARDUINO_AVR_UNO)
    PORTB &=    B11110111;   // Set low
  
  #elif defined(ARDUINO_SAM_DUE)
    digitalWrite(DUE_MOSI, LOW);
  #endif
  
}

void highMosi(){
  #if defined(ARDUINO_AVR_UNO)
    //DDRB = DDRB|B00101111; // Set as output - Overflødig
    PORTB |=    B00001000;  // Set it to high
  #elif defined(ARDUINO_SAM_DUE)
    digitalWrite(DUE_MOSI, HIGH);
  #endif
}

void lowSS(){
  #if defined(ARDUINO_AVR_UNO)
    //DDRB = DDRB|B00101111;  // Set as output - Overflødig
    PORTB &=    B11111011;   // Set low
  #elif defined(ARDUINO_SAM_DUE)
    digitalWrite(DUE_SS, LOW);
  #endif
}

void highSS(){
  #if defined(ARDUINO_AVR_UNO)
    //DDRB = DDRB|B00101111; // Set as output - Overflødig
    PORTB |=    B00000100;  // Set it to high
  #elif defined(ARDUINO_SAM_DUE)
    digitalWrite(DUE_SS, HIGH);
  #endif
}

void highClock(){
  #if defined(ARDUINO_AVR_UNO)
    //DDRB = DDRB|B00101111; // Set as output - Overflødig
    PORTB |=    B00100000;  // Set it to high
  #elif defined(ARDUINO_SAM_DUE)
    digitalWrite(DUE_CLOCK, HIGH);
  #endif
}

void lowClock(){
  #if defined(ARDUINO_AVR_UNO)
    //DDRB = DDRB|B00101111;  // Set as output - Overflødig
    PORTB &=    B11011111;   // Set low
  #elif defined(ARDUINO_SAM_DUE)
    digitalWrite(DUE_CLOCK, LOW);
  #endif    
}



//void continouslyProgram(){
///* BRUGES IKKE MERE!
// *  The sequence of issuing CP instruction is: 
// *  1 → CS# goes low  
// *  2 → sending CP instruction code
// *  3 → 3-byte address on SI pin
// *  4 → two data bytes on SI
// *  5 → CS# goes high to low 
// *  6 → sending CP instruction and then continue two data bytes are programmed
// *  7 → CS# goes high to low
// *  8 → till last desired two data bytes are programmed
// *  9 → CS# goes high to low
// * 10 → sending WRDI (Write Disable) instruction to end CP mode
// *   
// * 10.5 → send RDSR instruction to verify if CP mode word program ends, or send RDSCUR to check bit4 to verify if CP mode ends. 
// */
//  highSS();                           // just to be safe
//  lowSS();                            // Step 1
//// Serial.println("sendContinouslyProgramCommand");
//  sendContinouslyProgramCommand();    // Step 2
//  sendAdress(adressenViHusker);           // Step 3 
//// Serial.println("Adress sent");
//  // Herefter bliver al dataen sendt afsted
//  for(int i = 0; i < 2; i++){
//  // Serial.println("For-loop");
//    // Først skal dataen opdeles, da de ligger i 16-bit samples
//    // og det kun er muligt at smide 1 byte afsted ad gangen.
////    transmitOneByteSPI((arrayToSaveToFlash[i]>>8)&0xFF);       // First data byte
////    transmitOneByteSPI(arrayToSaveToFlash[i]&0xFF);   // Second
//
//    // Cycle Slave-Select (HIGH → LOW)
//    cycleSS();  // Step 5
//
//    // Hvis der er mere data der skal afsted (aka min. 1 sample mere)
//    if(i < sizeof(arrayToSaveToFlash) - 1){
//      sendContinouslyProgramCommand(); // Step 6
//    }// if
//  }// for
//
//  // Cycle Slave-Select (HIGH → LOW)
//  cycleSS();      // Step 9 -> Jeg er ikke sikker på denne egentlig skal være her? 
//  writeDisable(); // Step 10
//  
//}// continouslyProgram

  /*
    #define KICK_ADRESS     0x010000
    #define SNARE_ADRESS    0x020000
    #define HAT_ADRESS      0x030000
    #define CLAP_ADRESS     0x040000
   */





   
