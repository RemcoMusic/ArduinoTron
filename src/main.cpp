#include <Arduino.h>
#include <EEPROM.h>
#include "serial_interface.h"
#include "FAT.h"
#include "SRAM.H"
#include <Arduino_FreeRTOS.h>

//Function prototypes
void handleInputCommand(char* line);

enum taskState{
  IDLE,
  RUNNING,
  SUSPENDED,
  STOPPED
};

struct runningTasks{
  bool isRunning = false;
  TaskHandle_t handle;
  String filename;
  taskState state = taskState::IDLE;
  int programCounter = 0;
};

#define BUFSIZE 32
#define TASKLISTSIZE 10
runningTasks taskList[TASKLISTSIZE];
int taskCounter = 0;

void suspend(char* fileToSuspend){
  for(int i = 0; i<TASKLISTSIZE; i++){
    if(strcmp(fileToSuspend,taskList[i].filename.c_str()) == 0){
      vTaskSuspend(taskList[i].handle);
      taskList[i].state = taskState::SUSPENDED;
    }
  }
}

void resume(char* fileToSuspend){
  for(int i = 0; i<TASKLISTSIZE; i++){
    if(strcmp(fileToSuspend,taskList[i].filename.c_str()) == 0){
      vTaskResume(taskList[i].handle);
      taskList[i].state = taskState::RUNNING;
    }
  }
}

void stopTask(char* taskToStop){
  for(int i = 0; i<TASKLISTSIZE; i++){
    if(strcmp(taskToStop,taskList[i].filename.c_str()) == 0){
      taskList[i].state = taskState::STOPPED;
      //vTaskDelete(taskList[i].handle);
    }
  }
}

void delayTask(char* delValue){
  vTaskDelay(atoi(delValue)/portTICK_PERIOD_MS);
}

char* replace_char(char* str, char find, char replace){ //Helper function for handling files with commands
    char *current_pos = strchr(str,find);
    while (current_pos){
        *current_pos = replace;
        current_pos = strchr(current_pos,find);
    }
    return str;
}

void task(void* parm1WhichisTheTaskName){
  String file = fat::getFile((char*)parm1WhichisTheTaskName);
  char editableString[file.length()+1];
  strcpy(editableString,file.c_str());

  char *stringLines[10];
  byte index = 0;
  char* line = strtok(editableString, "/n");
    while(line != NULL)
    {
        stringLines[index] = line;
        index++;
        line = strtok(NULL, "/n");
    }

    for (int i = 0; i < index; i++)
    {
      replace_char(stringLines[i],'_',' ');
      handleInputCommand(stringLines[i]);
    }
  stopTask((char*)parm1WhichisTheTaskName);
  vTaskDelay(1000/portTICK_PERIOD_MS);
  vTaskDelete(nullptr);
}

void runTask(char* fileName){

  xTaskCreate(
    task,
    fileName,
    512,
    fileName,
    2,
    &taskList[taskCounter].handle
  );

  taskList[taskCounter].filename = fileName;

  taskCounter++;
  //vTaskSuspend( taskList[0] );
}

void listTasks(){
  for (int i = 0; i < taskCounter; i++)
  {
    Serial.println("==========TASK_LIST===============");
    Serial.print("TASK NAME:     ");Serial.println(taskList[i].filename);
    Serial.print("TASK STATUS:   ");    
    if(taskList[i].state == taskState::STOPPED){Serial.println("STOPPED");} else
    if(taskList[i].state == taskState::RUNNING){Serial.println("RUNNNING");} else
    if(taskList[i].state == taskState::SUSPENDED){Serial.println("SUSPENDED");} else
    if(taskList[i].state == taskState::IDLE){Serial.println("IDLE");} 

    Serial.print("TASK C:        ");Serial.println(taskList[i].programCounter);
  }
}

void echo(char* toPrint){
  console::printToConsole(toPrint);
}

void eraseFAT(){
  for (int i = 0 ; i < EEPROM.length() ; i++) {
    EEPROM.write(i, 0);
  }
  fat::noFatTable();
  Serial.println("fat reset done");
}

typedef struct {
  char name [BUFSIZE];
  void *func;
}commandType;

void memDemo(){  
  sram::pushByte(1);
  sram::pushByte(2);
  sram::pushByte(3);
  sram::pushByte(4);
  sram::pushByte(5);
  sram::pushByte(6);
  sram::pushByte(7);

  //from stack to memory
  sram::storeFromStackToMem(1,123,7,varType::INT);

  //get values from memory
  byte num[10] = {0};
  sram::getFromMem(1,num,123);
  for(int i = 0; i<10; i++){
    Serial.println(num[i]);
  }


  //push new values to stack
  sram::pushByte('h');
  sram::pushByte('e');
  sram::pushByte('l');
  sram::pushByte('l');
  sram::pushByte('o');
  sram::pushByte('\0');
  sram::storeFromStackToMem(2,124,6,varType::STRING);
  char cha[10] = {0};
  sram::getFromMem(2,cha,124);
  Serial.println(cha);

  char* newVal = "World\0";
  sram::changeMem(2,7,newVal);
  sram::getFromMem(2,cha,124);
  Serial.println(cha);
}

static commandType command[] = {
    {"store",     &fat::storeFile},
    {"retrieve",  &fat::readFile},
    {"erase",     &fat::eraseFile},
    {"files",     &fat::retrieveFiles},
    {"freespace", &fat::currentSpace},
    {"run",       &runTask},
    {"list",      &listTasks},
    {"suspend",   &suspend},
    {"resume",    &resume},
    {"kill",      &stopTask},
    // {"pop",       &sram::popByte},
    // {"push",      &sram::pushByte},
    // {"storeStackToMem",  &sram::storeFromStackToMem},
    // {"storeToMem",  &sram::storeToMem},
    // {"getFromMem",  &sram::getFromMem},
    // {"changeMem",  &sram::changeMem},
    {"memDemo",  &memDemo},
    {"echo",      &echo},
    {"eraseFat",  &eraseFAT},
    {"delay",  &delayTask},
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
  char* param4 = 0;
  char* param5 = 0;
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
   token = strtok(NULL, " ");
  if(token){
    param4 = token;
  }
   token = strtok(NULL, " ");
  if(token){
    param5 = token;
  }

  static int n = sizeof(command) / sizeof(commandType);
  for(int i = 0; i < n ; i++){
    if((strcmp(command[i].name,_command)) == 0){
      noCommand = false;
      if(param1 != 0 ){
        paramCounter++;
        if(param2 != 0) {
        paramCounter++;
        }
        if(param3 != 0) {
        paramCounter++;
        }
        if(param4 != 0) {
        paramCounter++;
        }
        if(param5 != 0) {
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
      } else if(paramCounter == 4){
        void (*func) (char* parameter, char* parameter2, char* parameter3, char* parameter4) = command[i].func;
        func(param1, param2, param3, param4);
      } else if(paramCounter == 5){
        void (*func) (char* parameter, void* parameter2, char* parameter3, char* parameter4, char* parameter5) = command[i].func;
        func(param1, param2, param3, param4, param5);
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