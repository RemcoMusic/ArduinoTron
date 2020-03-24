#include <Arduino.h>
#include <EEPROM.h>
#include "FAT.h"
#include "serial_interface.h"

namespace fat{
    void writeFATEntry(char* name, int eeAddress, int length){
        file tempStoredFile = readFATEntry(eeAddress);
        
        strcpy(tempStoredFile.fileName, name);
        EEPROM.put(eeAddress, tempStoredFile);
    }

    file readFATEntry(int eeAddress){
        file storedFile;
        EEPROM.get(eeAddress, storedFile);
        Serial.println("Retreived file: ");
        Serial.println(storedFile.fileName);
        Serial.println(storedFile.startPosFile);
        Serial.println(storedFile.fileLenght);
        return storedFile;
    }

    void storeFile(char* fileName, char* fileLenght, char* data){
        int fileList[] = {0, 16, 32, 48, 64, 80, 96, 112, 128, 144};
        long tempFileLenght = atoi(fileLenght);
        if(tempFileLenght <= 86){    
            for(int i = 0; i < 10; i++)
            {
                file tempFile;
                EEPROM.get(fileList[i], tempFile);
                int compareForDoubleFile = strcmp(tempFile.fileName, fileName);
                int compareForEmptyEntry = strcmp(tempFile.fileName, "unknown");
                if(compareForDoubleFile == 0){
                    console::printToConsole("This file already exists!");
                    break;
                } else if(compareForEmptyEntry == 0){
                    int tempStartPosStorage = tempFile.startPosFile;
                    writeFATEntry(fileName, fileList[i], tempFileLenght);
                    EEPROM.put(tempStartPosStorage, data);
                    console::printToConsole("The file has been stored to the EEPROM");
                    break;
                } else if(compareForEmptyEntry < 0 && i == 10){
                    console::printToConsole("The storage is full. Try to remove another file first");
                }
            }
        } else {
           console::printToConsole("File is to large for this system");
        }   
    }

    void readFile(char* fileName){
        int fileList[] = {0, 16, 32, 48, 64, 80, 96, 112, 128, 144};
        for(int i = 0; i < 10; i++)
        {
            file tempFile;
            EEPROM.get(fileList[i], tempFile);
            int compareInput = strcmp(tempFile.fileName, fileName);
            if(compareInput == 0){
                char* dataFile;
                EEPROM.get(tempFile.startPosFile, dataFile);
                console::printToConsole(dataFile);
                break;
            } else if(compareInput < 0 && i == 10){
                console::printToConsole("There no such file stored in the EEPROM");
            }
        }
    }

    void eraseFile(char* fileName){
        int fileList[] = {0, 16, 32, 48, 64, 80, 96, 112, 128, 144};
        for(int i = 0; i < 10; i++)
        {
            file tempFile;
            EEPROM.get(fileList[i], tempFile);
            int compareInput = strcmp(tempFile.fileName, fileName);
            if(compareInput == 0){
                writeFATEntry("unknown", fileList[i], NULL);
                console::printToConsole("The requested file is deleted");
                break;
            } else if(compareInput < 0 && i == 10){
                console::printToConsole("There no such file stored in the EEPROM");
            }
        }
    }

    void retrieveFiles(){
        int fileList[] = {0, 16, 32, 48, 64, 80, 96, 112, 128, 144};
        console::printToConsole("List of files: ");
        for(int i = 0; i < 10; i++)
        {
            file tempFile;
            EEPROM.get(fileList[i], tempFile);
            int compareEmptyFiles = strcmp(tempFile.fileName, "unknown");
            if(compareEmptyFiles < 0){
                console::printToConsole(tempFile.fileName);
            }
        }
    }

    void noFatTable(){
        int fileList[] = {0, 16, 32, 48, 64, 80, 96, 112, 128, 144};
        int tempStartPosFile[] = {161, 247, 333, 419, 505, 591, 677, 763, 849, 935};
        for (int i = 0; i < 10; i++)
        {
            file tempFile = {
            "unknown",
            tempStartPosFile[i],
            86
            };
            EEPROM.put(fileList[i], tempFile);
        }
    }
}