//
//  main.cpp
//  ComsWithFlasher
//
//  Created by Mathias Lyngklip Kjeldgaard on 25/04/2017.
//  Copyright © 2017 Mathias Lyngklip Kjeldgaard. All rights reserved.
//

#include <iostream>
#include <stdio.h>
using namespace std;

int
main()
{
    FILE *file = NULL;
    //Opening device file
    
    int getnum = 0;
    do{
        file = fopen("/dev/cu.usbmodem1D11111", "rw");
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
        
        */
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