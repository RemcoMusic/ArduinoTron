#pragma once
#include <Arduino.h>
enum varType{
    STRING,
    BYTE,
    INT
};
namespace sram{
    #define STACKSIZE 256
    #define MAX_VARS 25

    struct variable{
        byte name;
        String type;
        void* value;
        int16_t size;
        int16_t processID;
    };
    void pushByte ( byte b); //to stack
    byte popByte (); //from stack

    void storeFromStackToMem(byte name, int id, int size,varType type);
    void storeToMem(byte name, void* value, int size, int ID, varType type);
    void* getFromMem(byte name,void* dest, int ID);
    void changeMem(byte name,int size,void* newValue);


    extern byte stack[STACKSIZE];
    extern variable memTable[MAX_VARS];
    extern uint16_t noOfVars; //amount of valid values in memtable
    
}