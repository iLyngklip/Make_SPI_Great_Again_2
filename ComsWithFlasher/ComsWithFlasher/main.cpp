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


/*
int main()
{
    FILE *file = NULL;
    //Opening device file
    
    int getnum = 0;
    do{
        file = fopen("Audiosamples.h", "r");
    } while (file == NULL);
    printf("test");
    while (true)
    {
        /*
        // file = fopen("/dev/cu.usbmodem1D11111", "w");
        cout << ">>" << endl;
        while(getnum == 0){
            cin >> getnum;
        }
        fprintf(file, "%d\n", getnum); //Writing to the file
        getnum = 0;
        
 
        char temp = 0x00;
        for(int i = 0; i < 1024; i++){
            fprintf(file, "%c\n", i);
            cout << "test";
            temp = fgetc(file);
            printf("Temp:%c\n", temp);
            
        }
        fclose(file);
    }
    
}
*/
// /Users/iLyngklip/Documents/Arduino/Make_SPI_Great_Again/ComsWithFlasher/ComsWithFlasher

int main () {
    string tempString;
    
    ofstream writeFile ("/Users/iLyngklip/Documents/Arduino/Make_SPI_Great_Again/ComsWithFlasher/ComsWithFlasher/example.txt");
    ifstream readFile ("/Users/iLyngklip/Documents/Arduino/Make_SPI_Great_Again/ComsWithFlasher/ComsWithFlasher/Kick.txt");
    if (writeFile.is_open() && readFile.is_open())
    {
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
            
            
            for(int k = 0; k < 16; k++){
                tempString.append(("0x"));
                char tempChar;
                do{
                    tempChar = (char)readFile.get();
                } while ((tempChar <= '0' && tempChar >= '9' && tempChar <= 'a' && tempChar >= 'z') || tempChar == 10 || tempChar == 13);
                tempString.push_back((char)tempChar);
                
                do{
                    tempChar = (char)readFile.get();
                } while ((tempChar <= '0' && tempChar >= '9' && tempChar <= 'a' && tempChar >= 'z') || tempChar == 10 || tempChar == 13);
                tempString.push_back((char)tempChar);
                tempString.append(", ");
                length++;
            }
            tempString.append("\n");
            writeFile << tempString;
            
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
        writeFile.close();
        readFile.close();
        cout << "Length:\t" << length;
    }
    else cout << "Unable to open file";
    return 0;
}