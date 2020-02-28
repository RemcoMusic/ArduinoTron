#include <Arduino.h>
#include "serial_interface.h"

namespace console{
    //int printcounter = 0;

    const byte numChars = 32;
    char receivedCommand[numChars] = {0};
    bool hasValidCommand = false;

    void initialize(){
        Serial.begin(9600);
        Serial.println("ArduinoTron 2.0 is ready");
    }

    bool handleInputData(){
        static uint8_t index = 0;
        if (Serial.available() > 0) {
            char r = Serial.read();
            if(r == '\n'){
                receivedCommand[index++] = 0;
                hasValidCommand = true;
                index = 0;
                return true;
            }else{
                receivedCommand[index++] = r;
            }

            if(index == numChars){
                printToConsole("Input is too large. Try another command");
                index = 0;
                receivedCommand[0] = 0;
            }
        }
        return false;
    }

    char* getLatestCommand(){
        return receivedCommand;
    }

    void printToConsole(char* print_data){
        Serial.println(print_data);
    }
}
