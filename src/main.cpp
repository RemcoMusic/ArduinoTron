#include <Arduino.h>
#include "serial_interface.h"

#define BUFSIZE 32

typedef struct {
  char name [BUFSIZE];
  void * func;
}commandType;

static commandType command[] = {
    {"store",     &console::printToConsole},
    {"retrieve",  &console::printToConsole},
    {"erase",     &console::printToConsole},
    {"files",     &console::printToConsole},
    {"freespace", &console::printToConsole},
    {"run",       &console::printToConsole},
    {"list",      &console::printToConsole},
    {"suspend",   &console::printToConsole},
    {"resume",    &console::printToConsole},
    {"kill",      &console::printToConsole},
};

void setup(){
  console::initialize();
}

void handleInputCommand(char* line){
  char* _command = 0;
  char* param1 = 0;
  char* param2 = 0;
  char* param3 = 0;
  bool noCommand = true;

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
        console::printToConsole("With parameters: ");
        console::printToConsole(param1);
      }
      if(param2 != 0){
        console::printToConsole(param2);
      }
      if(param3 != 0){
        console::printToConsole(param3);
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