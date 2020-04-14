#include <Arduino.h>
#include <EEPROM.h>
#include "serial_interface.h"
#include "FAT.h"

#define BUFSIZE 32

typedef struct {
  char name [BUFSIZE];
  void *func;
}commandType;

static commandType command[] = {
    {"store",     &fat::storeFile},
    {"retrieve",  &fat::readFile},
    {"erase",     &fat::eraseFile},
    {"files",     &fat::retrieveFiles},
    {"freespace", &fat::currentSpace},
    {"run",       &console::printToConsole},
    {"list",      &console::printToConsole},
    {"suspend",   &console::printToConsole},
    {"resume",    &console::printToConsole},
    {"kill",      &console::printToConsole},
};

void setup(){
  console::initialize();
  fat::initializeFATTable();
}

void handleInputCommand(char* line){
  char* _command = 0;
  char* param1 = 0;
  char* param2 = 0;
  char* param3 = 0;
  bool noCommand = true;
  int paramCounter = 0;

  char *token = strtok(line, " ");
  if(token){
    _command = token;
  }
  token = strtok(NULL, " ");
  if(token){
    param1 = token;
  }
   token = strtok(NULL, " ");
  if(token){
    param2 = token;
  }
   token = strtok(NULL, " ");
  if(token){
    param3 = token;
  }

  static int n = sizeof(command) / sizeof(commandType);
  for(int i = 0; i < n ; i++){
    if((strcmp(command[i].name,_command)) == 0){ //index found
      console::printToConsole("Command: ");
      console::printToConsole(command[i].name);
      noCommand = false;
      if(param1 != 0 ){
        paramCounter++;
        console::printToConsole("With parameters: ");
        console::printToConsole(param1);
        if(param2 != 0) {
        paramCounter++;
        }
        if(param3 != 0) {
        paramCounter++;
        }
      }
      if(paramCounter == 1){
        void (*func) (char* foo) = command[i].func;
        func(param1);
      } else if(paramCounter == 2){
        void (*func) (char* parameter, char* parameter2) = command[i].func;
        func(param1, param2);
      } else if(paramCounter == 3){
        void (*func) (char* parameter, void* parameter2, char* parameter3) = command[i].func;
        func(param1, param2, param3);
      } else if(paramCounter == 0){
        void (*func) () = command[i].func;
        func();
      }
    }
  }
  if(noCommand){
    console::printToConsole("Command has not been found try one of these: ");
      for (int i = 0; i < n; i++){
        console::printToConsole(command[i].name);
      }
  }
}

void loop(){
  if(console::handleInputData()){
    char* data = console::getLatestCommand();
    handleInputCommand(data);
  }
}