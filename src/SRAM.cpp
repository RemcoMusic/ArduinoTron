#include <Arduino.h>
#include "SRAM.H"

byte sp = 0;

namespace sram{
    byte stack[STACKSIZE];
    variable memTable[MAX_VARS];
    uint16_t noOfVars  = 0; //default value

    byte popByte() {
        return stack [--sp];
    }
    void pushByte(byte b) {
        stack [sp++] = b;
    }

    void storeFromStackToMem(byte name, int id, int size,varType type){
        byte arr[size];
        for(int i = size-1; i>=0;i--){
            arr[i] = popByte();
        }
        storeToMem(name,arr,size,id,type);
    }

    void storeToMem(byte name,void* value, int size, int ID, varType type){
        memTable[name].name = name;
        memTable[name].size = size;
        memTable[name].value = malloc(size);
        memTable[name].processID = ID;
        memTable[name].type = type;

        memcpy(memTable[name].value,value,size);

        noOfVars++;
    }

    void* getFromMem(byte name,void* dest, int ID){
        memcpy(dest,memTable[name].value,memTable[name].size);
        return dest;
    }

    void changeMem(byte name,int size,void* newValue){
        memcpy(memTable[name].value,newValue,size);
    }
}