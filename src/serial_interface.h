#pragma once

namespace console{
    //global accesable variables
    //extern int printcounter;

    void initialize();
    bool handleInputData();
    char* getLatestCommand();
    void printToConsole(char* print_data);
}



