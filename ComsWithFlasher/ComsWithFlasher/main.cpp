//
//  main.cpp
//  ComsWithFlasher
//
//  Created by Mathias Lyngklip Kjeldgaard on 25/04/2017.
//  Copyright © 2017 Mathias Lyngklip Kjeldgaard. All rights reserved.
//

#include <iostream>
#include <stdio.h>
#include <fstream>
using namespace std;

#define WRITE_PLACE "/Users/iLyngklip/Documents/Arduino/Make_SPI_Great_Again/ComsWithFlasher/ComsWithFlasher/Færdig_arduino_kode/"
#define READ_PLACE  "/Users/iLyngklip/Documents/Arduino/Make_SPI_Great_Again/ComsWithFlasher/ComsWithFlasher/"



const int kickSampleLength  = 18941;
const int snareSampleLength = 13981;
const int hatSampleLength   = 18941;
const int clapSampleLength  = 13981;


#define NUMBER_OF_SAMPLES   4
const string sampleNames[] = {
    "kick.txt",
    "snare.txt",
    "hat.txt",
    "clap.txt"
};

// /Users/iLyngklip/Documents/Arduino/Make_SPI_Great_Again/ComsWithFlasher/ComsWithFlasher

int main () {
    string tempString;

    for(int i = 0; i < NUMBER_OF_SAMPLES; i++){
        string tempFileWriteString = "/Users/iLyngklip/Documents/Arduino/Make_SPI_Great_Again/ComsWithFlasher/ComsWithFlasher/Færdig_arduino_kode/";
        string tempFileReadString = "/Users/iLyngklip/Documents/Arduino/Make_SPI_Great_Again/ComsWithFlasher/ComsWithFlasher/";
        
        tempFileWriteString.append(sampleNames[i]);
        tempFileReadString.append(sampleNames[i]);
        
        ofstream writeFile (tempFileWriteString);
        ifstream readFile (tempFileReadString);
        
        if (writeFile.is_open() && readFile.is_open())
        {
            readFile.seekg(48*2);
            /*
            cout << (char)readFile.get();
            cout << (char)readFile.get();
            cout << (char)readFile.get();
            cout << (char)readFile.get();
            cout << (char)readFile.get();
            cout << (char)readFile.get() << "\n";
            */
            // print header
            int length = 0;
            
            // Print data
            while(!readFile.eof()){
                tempString.erase();
                
                tempString.append(("0x"));
                char tempChar;
                char tempChar2;
                do{
                    tempChar = (char)readFile.get();
                } while ((tempChar <= '0' && tempChar >= '9' && tempChar <= 'a' && tempChar >= 'z') || tempChar == 10 || tempChar == 13);
                if(readFile.eof()){
                    break;
                }
                
                do{
                    tempChar2 = (char)readFile.get();
                } while ((tempChar2 <= '0' && tempChar2 >= '9' && tempChar2 <= 'a' && tempChar2 >= 'z') || tempChar2 == 10 || tempChar2 == 13);
                
                tempString.push_back((char)tempChar2);
                tempString.push_back((char)tempChar);
                tempString.append(", ");
                length++;
                
                if(length % 16 == 0){
                    tempString.append("\n");
                }
                
                if(!readFile.eof()){
                    writeFile << tempString;
                }
                
            }
            /*
                tempString += (char)readFile.get();
                tempString += (char)readFile.get();
                tempString += (char)readFile.get();
                tempString += (char)readFile.get();
                tempString += (char)readFile.get();
                tempString += (char)readFile.get();
            */
            // cout << tempString;
            
            // writeFile << tempString;
            // writeFile << "This is another line.\n";
            tempString.erase();
            tempString.append("\nLength:\t"); tempString.append(to_string(length));
            writeFile << tempString;
            cout << "Length:\t" << length << "\n";
            writeFile.close();
            readFile.close();
            
        }
        else cout << "Unable to open file";
    }
    return 0;
}