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


// Block32-skifte ligger ved 0x010000, 0x020000 osv.
// Block-skifte ligger ved 0x100000, 0x200000 osv.
#define KICK_ADRESS     0x110000
#define SNARE_ADRESS    0x120000
#define HAT_ADRESS      0x130000
#define CLAP_ADRESS     0x140000
  

  // 0: Disable 
  // 1: Enable
#define SHOULD_WIPE_WHOLE_CHIP        0
#define SHOULD_BLOCK_ERASE            1
#define SHOULD_LOCK_AFTER_PROGRAMMING 0

#define AREA_TO_LOCK                  B00011000
  /*  Tabel over områder der låses med AREA_TO_LOCK
   * BP3  BP2 BP1 BP0 64Mb  Area                            Binær kode
   *  0    0   0   0   0    (none)                          B00000000
   *  0    0   0   1   1    (2 blocks, block 126th-127th)   B00000100
   *  0    0   1   0   2    (4 blocks, block 124th-127th)   B00001000
   *  0    0   1   1   3    (8 blocks, block 120th-127th)   B00001100
   *  0    1   0   0   4    (16 blocks, block 112nd-127th)  B00010000
   *  0    1   0   1   5    (32 blocks, block 96th-127th)   B00010100
   *  0    1   1   0   6    (64 blocks, block 64th-127th)   B00011000       <- Den bruger vi
   *  1    1   1   1   15   (All)                           B00111100
   */


//    0: Disable
//    1: Enable
#define DEBUG_BLOCK_ERASE 0
#define DEBUG_WHILE_LOOPS 0
#define DEBUG_WRONG_DATA  0 
#define PRINT_WHERE_WE_ARE_READING  0
#define PRINT_WHERE_WE_ARE_WRITING  0 
#define PRINT_THE_READ_DATA         0
#define FIND_STUFF_INSTEAD          0


 
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
 *   
 * ⎯⎯⎯⎯⎯⎯⎯⎯⎯⎯⎯⎯⎯⎯⎯⎯⎯⎯⎯⎯⎯⎯⎯⎯⎯⎯⎯⎯⎯⎯⎯⎯⎯⎯⎯⎯⎯⎯⎯⎯⎯
 * ⎟    Arduino   ⏉ Port      ⏉  Papilio    ⏋
 * ⎟  Clock:   13 ⎟ B00100000 ⎟  13         ⎟
 * ⎟  MISO:    12 ⎟ B00010000 ⎟  12         ⎟
 * ⎟  MOSI:    11 ⎟ B00001000 ⎟  11         ⎟
 * ⎟  SS:      10 ⎟ B00000100 ⎟  09         ⏌
 * ⎯⎯⎯⎯⎯⎯⎯⎯⎯⎯⎯⎯⎯⎯⎯⎯⎯⎯⎯⎯⎯⎯⎯⎯⎯⎯⎯⎯⎯⎯⎯⎯⎯⎯⎯⎯⎯⎯⎯⎯⎯
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

  
// Sikrer lige det rigtige board er valgt
#if !defined(ARDUINO_SAM_DUE)
  #error Forkert board, brug Arduino Due!
#endif



        

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
byte storeReadData[NUMBER_OF_SAMPLES][MAX_SAMPLE_LENGTH]; // Her gemmer vi de to byte vi læser
byte storeRDSR  = 0x00;                // RDSR gemmes her. Omskriv til lokal variabel
byte RDSCUR     = 0x00;                   // RDSCUR gemmes her. Omskriv til lokal variabel

boolean WIP   = true; // Write In Progress - bit
boolean WEL   = true; // Write Enable Latch - bit
boolean BP0   = true; //   |
boolean BP1   = true; //   |-> Protection
boolean BP2   = true; //   |
boolean BP3   = true; //   |
boolean QE    = false;// Quad Enable <- USELESS
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
  printMainMenu();
  #if FIND_STUFF_INSTEAD == 1
    Serial.println("Hello");
    
    
    byte arrayToFind[] = {  0x7C, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 
                            0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xAA, 0x99, 0x55, 0x66, 0x30};
    byte arrayToCompare[sizeof(arrayToFind)];
    
    byte tempByteForReading = 0xFF;
    uint32_t adressWeAreLookingFor = 0x00000000;
  
    bool haveWeFoundIt = false;
    int offsetAdress = 340695;
    
    lowSS();
  
    // Step 2
    sendReadInstruction();
    
    // Step 3
    sendAdress(adressWeAreLookingFor);
    do{
       
      
      do{
        tempByteForReading = readOneByteSPI();
        adressWeAreLookingFor++;
        Serial.println(adressWeAreLookingFor, HEX);
      } while(tempByteForReading != arrayToFind[0]);
  
      Serial.print("Found:\t"); Serial.println(tempByteForReading, HEX);
      
      arrayToCompare[0] = tempByteForReading;
      
      // Load data into the compare-array
      for(int i = 1; i < sizeof(arrayToFind); i++){
        arrayToCompare[i] = readOneByteSPI();
        adressWeAreLookingFor++;
        
        Serial.print("Found:\t"); Serial.println(arrayToCompare[i], HEX);
        if(arrayToFind[i] == arrayToCompare[i]){
          if(i == sizeof(arrayToFind) && haveWeFoundIt == true){
            Serial.println("Returning");
            return;
          }
          haveWeFoundIt = true;
          Serial.print("Found something @:\t"); Serial.print(adressWeAreLookingFor + i, HEX); Serial.print("\t"); Serial.println(haveWeFoundIt);
        } else {
          Serial.print("Lost it \t @:\t"); Serial.print(adressWeAreLookingFor + i, HEX); Serial.print("\t"); Serial.println(haveWeFoundIt);
          haveWeFoundIt = false;
          
        }
        
      }
      
    Serial.print("ad:\t"); Serial.println(adressWeAreLookingFor);  
    } while (!haveWeFoundIt && adressWeAreLookingFor < 0xFFFFFF + 1);
    
    highSS();  
  
    Serial.println("FOUND IT FAGGOTS!");
    Serial.print("AWALF:\t"); Serial.print(adressWeAreLookingFor, HEX); Serial.print("\t");Serial.print("Start adressen er:\t"); Serial.println(adressWeAreLookingFor - offsetAdress,HEX);
    while(1);
 
  
  #endif
  // Overstående er hvis vi leder efter ting i stedet. Bruges nok ikke mere...
  
  
  Serial.flush(); // Flusher lige Serial, så den ikke giver "Wrong input"
  Serial.flush(); // ... det gør vi lige en ekstra gang
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
void loop(){  
  if(Serial.available() == 1){   // check for serial data
    char tempCharForSwitch = Serial.read();
    if(tempCharForSwitch > 90){
      Serial.print("tempCharForSwitch:\t"); Serial.println(tempCharForSwitch);
      tempCharForSwitch -= 32;
    } 
    switch(tempCharForSwitch){       // see which command we received
    
    case 'A': // AUTO
      eraseAllRoutine();
      headerRoutine();
      unlockRoutine();
      programRoutine();
      readRoutine();
      verifyRoutine();
      lockRoutine();
      
      Serial.println("--- Process done --------------");
      printMainMenu();
      break;  

    case 'R': // READ
      headerRoutine();
      readRoutine();
      Serial.println("--- Process done --------------");
      printMainMenu();
      break;  

    case 'V': // Verify
      headerRoutine();
      verifyRoutine();
      Serial.println("--- Process done --------------");
      printMainMenu();
      break;  
    
    case 'P':// Program
      headerRoutine();
      programRoutine();
      Serial.println("--- Process done --------------");
      printMainMenu();
      break;

    case 'D':// Specific erase
      
      Serial.println("--- Process done --------------");
      printMainMenu();
      break;

    case 'E':// Erase all
      headerRoutine();
      chipErase();
      Serial.println("--- Process done --------------");
      printMainMenu();
      break;
    case 'L': // Lock chip
      break;

    case 'U': // Unlock chip
      break;

      
    default:
      // Serial.println("Wrong input.");
      break;
    }// Switch
    
  } else {    // seems like this is not necessary, but what the hey, leave it in for kicks
    if(Serial.available() > 1) // Har vi modtaget for mange bytes, er noget galt
      Serial.flush();          
   }// else

}// loop







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
void eraseAllRoutine(){        
  #if SHOULD_WIPE_WHOLE_CHIP == 1
    Serial.println("Full chip erase: ON");
    chipErase();
    Serial.println("Full chip erase:  DONE");
  #elif SHOULD_WIPE_WHOLE_CHIP == 0
    Serial.println("Full chip erase: OFF");

  #else 
    #error Fejl i "SHOULD_WIPE_WHOLE_CHIP"
  #endif
}  
  
void headerRoutine(){  
  Serial.println("---Korer process om 2 sec.---");
  delay(2000);
  Serial.println();  
}

void unlockRoutine(){
  Serial.print("Unlocking chip:\t");
  unlockChip();
  Serial.println("DONE");
}

void programRoutine(){
  
  /*##########################################
   *##########################################
   *  PROGRAMMERER                          ##
   *##########################################
   *##########################################
   */
   Serial.println("Programmerer");
  
  
  uint16_t tempVal = 0;
  /*
  0: #define KICK_ADRESS     0x110000
  1: #define SNARE_ADRESS    0x120000
  2: #define HAT_ADRESS      0x130000
  3: #define CLAP_ADRESS     0x140000
  */
  
  
  // Her kørers forskellige procedurer alt efter hvilken af de 4 samples
  // der skal programmeres. 
  for(int sampleNr = 0; sampleNr < NUMBER_OF_SAMPLES; sampleNr++){
    // Her udregnes hvor mange HELE pages der er (256 byte)
    tempVal = (arrayLengths[sampleNr] - (arrayLengths[sampleNr] % 0xFF)) / 0xFF;
  
    // En variabel til at gemme hvilken page vi skriver til.
    uint32_t tempAdresse = 0;
  
    // Debugging...
    #if PRINT_WHERE_WE_ARE_WRITING == 1
      Serial.print("tempVal:\t"); Serial.println(tempVal);
    #endif 
    
  
    
    boolean lykkesDetAtSkrive = false;   
    switch(sampleNr){
      
      
      case 0:
        // KICK sample
  
        // Loadingbar til brugeren
        Serial.println("\t[                ]");
        
        // Sætter adresse-variablen
        tempAdresse = KICK_ADRESS;
  
        // Tjekker om vi skal slette ne 32kB blok
        #if SHOULD_WIPE_WHOLE_CHIP == 0
          #if SHOULD_BLOCK_ERASE == 1
            block32Erase(tempAdresse);  
          #endif
        #endif
  
        // Programmerer antallet af HELE pages
        for(int antal256bytes = 0; antal256bytes < tempVal; antal256bytes++){      
          
          // do-while loop til at tjekke om det lykkes at skrive til chippen
          do{
            // Her gemmes status på skrive-processen. 
            lykkesDetAtSkrive = pageProgram(tempAdresse, antal256bytes, sampleNr, 0xFF);
          } while(!lykkesDetAtSkrive);
          
          // Er det lykkedes at skrive, tælles adressen op
          tempAdresse += 0x000100;
        }
  
        // Vi er nu færdige med at skrive antallet af HELE pages
        lykkesDetAtSkrive = false;
        do{
          // Samme procedure som overstående, bare denne gang er det ikke nødvendigvis en hel page
          // hvilket ses på den sidste parameter i pageProgram()
          lykkesDetAtSkrive = pageProgram(tempAdresse, tempVal, sampleNr, (arrayLengths[sampleNr] % 0xFF));
        } while(!lykkesDetAtSkrive);
  
        // Done med programmering af sample 0
        break;
  
  
  
      case 1:
      // For kommentering til casen, se case 0 da det er det samme i alle 4.
        Serial.println("\t[====            ]");
        tempAdresse = SNARE_ADRESS;
        #if SHOULD_WIPE_WHOLE_CHIP == 0
          #if SHOULD_BLOCK_ERASE == 1
            block32Erase(tempAdresse);  
          #endif  
        #endif
        for(int antal256bytes = 0; antal256bytes < tempVal; antal256bytes++){
          do{
            
            lykkesDetAtSkrive = pageProgram(tempAdresse, antal256bytes, sampleNr, 0xFF);
          } while(!lykkesDetAtSkrive);
          tempAdresse += 0x000100;
        }
        lykkesDetAtSkrive = false;
        do{
          lykkesDetAtSkrive = pageProgram(tempAdresse, tempVal, sampleNr, (arrayLengths[sampleNr] % 0xFF));
        }while(!lykkesDetAtSkrive);
        
        break;
  
  
  
      case 2:
      // For kommentering til casen, se case 0 da det er det samme i alle 4.
        Serial.println("\t[========        ]");
        tempAdresse = HAT_ADRESS;
        #if SHOULD_WIPE_WHOLE_CHIP == 0
          #if SHOULD_BLOCK_ERASE == 1
            block32Erase(tempAdresse);  
          #endif 
        #endif
        for(int antal256bytes = 0; antal256bytes < tempVal; antal256bytes++){
          do{
            
            lykkesDetAtSkrive = pageProgram(tempAdresse, antal256bytes, sampleNr, 0xFF);
          } while(!lykkesDetAtSkrive);
          tempAdresse += 0x000100;
        }
        lykkesDetAtSkrive = false;
        do{
          lykkesDetAtSkrive = pageProgram(tempAdresse, tempVal, sampleNr, (arrayLengths[sampleNr] % 0xFF));
        }while(!lykkesDetAtSkrive);
        
        break;
  
  
  
      case 3:
      // For kommentering til casen, se case 0 da det er det samme i alle 4.
        Serial.println("\t[============    ]");
        tempAdresse = CLAP_ADRESS;
        #if SHOULD_WIPE_WHOLE_CHIP == 0
          #if SHOULD_BLOCK_ERASE == 1
            block32Erase(tempAdresse);  
          #endif
        #endif
        
        for(int antal256bytes = 0; antal256bytes < tempVal; antal256bytes++){
          do{
            lykkesDetAtSkrive = pageProgram(tempAdresse, antal256bytes, sampleNr, 0xFF);
          } while(!lykkesDetAtSkrive);
          tempAdresse += 0x000100;
        }
        lykkesDetAtSkrive = false;
        do{
          lykkesDetAtSkrive = pageProgram(tempAdresse, tempVal, sampleNr, (arrayLengths[sampleNr] % 0xFF));
        }while(!lykkesDetAtSkrive);
        break;
    }
  
    
  }// for 
  Serial.println("\t[================]");
  
  
  Serial.println("Programmering done\n");
  
  highSS();
}
  
void readRoutine(){
  /*##########################################
   *##########################################
   *  LÆSER                                 ##
   *##########################################
   *##########################################
   */
    Serial.print("Reading\n");
      uint32_t tempVal = 0x0;
      for(int sampleNr = 0; sampleNr < NUMBER_OF_SAMPLES; sampleNr++){
        // Udregner antallet af HELE pages (256 byte)
        tempVal = (arrayLengths[sampleNr] - (arrayLengths[sampleNr] % 0xFF)) / 0xFF;
        
        switch(sampleNr){
          
          case 0:
              Serial.println("\t[                ]");
              readTwoBytes(KICK_ADRESS, tempVal, sampleNr); 
            break;
  
          case 1:
              Serial.println("\t[====            ]");
              readTwoBytes(SNARE_ADRESS, tempVal, sampleNr); 
            break;
  
          case 2:
              Serial.println("\t[========        ]");
              readTwoBytes(HAT_ADRESS, tempVal, sampleNr); 
            break;
  
          case 3:          
              Serial.println("\t[============    ]");
              readTwoBytes(CLAP_ADRESS, tempVal, sampleNr);
            break;
        }// Switch
        
      }// for
    Serial.print("\t[================]\t");  
    delay(100);
    Serial.println("Done\n");
    highSS();
}

void verifyRoutine(){
  /*##########################################
   *##########################################
   *  VERIFICERER                           ##
   *##########################################
   *##########################################
   */
    Serial.println("Verificerer");
  
    // Sammenligner det læste med LUT til der skrevne
    // Er der forskel får brugeren det at vide
    for(int i = 0; i < NUMBER_OF_SAMPLES; i++){
      Serial.print("Sample "); Serial.print(i); Serial.print(":\t");  
      if(compareData(i)){
        Serial.println("FAILED");
      } else {
        Serial.println("SUCCEEDED");
      }
    }
  
    // In case, brugeren vil have printet al den læste tata ud igen
    #if PRINT_THE_READ_DATA == 1
      printReadData();
    #endif
  
    
   Serial.println("Verificering done\n");  
   highSS();
}

void lockRoutine(){
  // Hvis den skal låse chippen bagefter den har skrevet til den
  #if SHOULD_LOCK_AFTER_PROGRAMMING == 1
    Serial.print("Locking the chip:\t");
    lockChip();
    Serial.println("DONE\n");
  #endif
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
  for(int i = 0; i < (0xFF + (numberOfPagesToRead * 0xFF)); i++){
    if(i % 0xFF == 0 && i != 0){
      readOneByteSPI();
    }
    storeReadData[sampleNr][i] = readOneByteSPI();
    // delayMicroseconds(0);
    // Serial.print("i:\t"); Serial.print(i); Serial.print("\t"); Serial.print(storeReadData[sampleNr][i], HEX); Serial.print("\t"); Serial.println(arrayToSaveToFlash[sampleNr][i],HEX);
    reading++;
  }
  
  
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

void lockChip(){
  do{
    readStatusRegister();
    writeEnable();
    lowSS();
    // Serial.print("Stuff: "); Serial.println(B00111100 | storeRDSR, BIN);
    transmitOneByteSPI(0x01);
    
    transmitOneByteSPI(AREA_TO_LOCK | storeRDSR);
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
    
    transmitOneByteSPI(!(AREA_TO_LOCK) & storeRDSR);
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
  
  highMosi();
  
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
  
  lowMosi();
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

void blockErase(uint32_t adress){
  /*  
   * Sletter en hel block (64k-byte)
   *  Er blokken låst (BP0-BP3 = 1, eller WPSEL = 1) sker der ingen ting
   *    Bruges ikke mere.
   */
  #if DEBUG_BLOCK_ERASE == 1
   Serial.print("blockErase: adress:\t"); Serial.println(adress, HEX);
  #endif
  lowSS();
  writeEnable(); waitUntilWEL();
  lowSS();
  transmitOneByteSPI(0xD8);
  sendAdress(adress);
  highSS();
  
  waitUntilWorkIsDone();
}

void block32Erase(uint32_t adress){
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
  transmitOneByteSPI(0x52);
  sendAdress(adress);
  highSS();
  
  waitUntilWorkIsDone();
}

void sectorErase(uint32_t adress){
  lowSS();
  writeEnable(); waitUntilWEL();
  lowSS();
  transmitOneByteSPI(0x20);
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
  highSS();       // Sikrer SS er høj
  waitUntilWEL(); // Venter på Write Enable
  lowSS();        // Sætter SS low før kommando
  transmitOneByteSPI(0xC7); // Slet alt kommando
  highSS();       // Høj SS igen, for vi er done med første del
  
  delay(20000);   // Giver chippen tid til at slette
                  // Typ: 50s Max: 80s
  
  waitUntilWorkIsDone();  // Da delayet over er for kort, venter vi på 
                          // chippen er klar igen, så vi tjekker WIP
  highSS();       // Høj SS da vi er helt done
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
    for(int i = 0; i < (arrayLengths[i]); i+=2){
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

void printMainMenu(){
  delay(3000);
  Serial.print("\n\n\n\n\n\n");
  Serial.println("---------------------------------");
  Serial.println("|\t MAIN MENU\t\t|");
  Serial.println("---------------------------------");
  Serial.println("|Command\tAction\t\t|");
  Serial.println("---------------------------------");
  Serial.println("| A\tAuto\t\t\t|");
  Serial.println("| R\tRead\t\t\t|");
  Serial.println("| V\tVerify\t\t\t|");
  Serial.println("| P\tProgram\t\t\t|");
  Serial.println("| U\tUnlock chip\t\t|");
  Serial.println("| L\tLock chip\t\t|");
  Serial.println("| D\tErase specific area\t|");
  Serial.println("| E\tErase whole chip\t|");
  Serial.println("---------------------------------\n\n");
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
/* PINOUT
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
