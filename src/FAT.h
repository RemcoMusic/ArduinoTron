#pragma once

#include <Arduino.h>

struct file
{
    char fileName [12];
    int16_t startPosFile;
    int16_t fileLenght;
};

namespace fat{
    void writeFATEntry(char* name, int eeAddress, int length);
    file readFATEntry(int eeAddress);
    void storeFile(char* fileName, char* fileLenght, char* data);
    void readFile(char* fileName);
    void eraseFile(char* fileName);
    void retrieveFiles();
    void noFatTable();
}