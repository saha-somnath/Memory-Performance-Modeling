//
//  main.cpp
//  MemoryModel
//
//  Created by Somnath Saha on 6/23/18.
//  Copyright © 2018 Somnath Saha. All rights reserved.
//
#include <iostream>
#include <vector>
#include <map>
#include <array>
#include <queue>
#include <cmath>
#include <fstream>
#include <unistd.h>
#include <sstream>
#include <unordered_set>
#include <cmath>
#include "memory.h"

using namespace std;


int main(int argc, const char * argv[]) {
    
    size_t memory_in_KB = std::stoi(argv[1]);
    size_t banks_count  = std::stoi(argv[2]);
    std::string  addressInitializationFile(argv[3]);
    std::string testInputFile(argv[4]);

    cout<<"MEM:"<<memory_in_KB << ":"<<banks_count<<endl;
    MemoryController myMem(memory_in_KB, banks_count);
    //std::string  addressInitializationFile("input_decimal.txt");
    myMem.initializeMemoryBank(addressInitializationFile);
    //myMem.display(4);
    
    
    // Testing:
    //std::string testInputFile("test_case_same_bank.txt");
    myMem.processTest(testInputFile);
    myMem.requestHandler();
    myMem.responseHandler();
    
    return 0;
}
