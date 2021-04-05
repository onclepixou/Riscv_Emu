#include <Cpu.h>
#include <Memory.h>

#include <iostream>
#include <stdexcept>


int main(int argc, char** argv){


    try{
    
        Cpu myCpu("I");
        Memory myMemory(128 * 1024 *1024);
        myCpu.AttachMemory(&myMemory);
        myCpu.LoadElf("simple");
        myCpu.SetTraceActivated(true);
        myCpu.Start();

    }

    catch(const std::exception& e){

        std::cout << e.what() << std::endl;
    }

    

    return EXIT_SUCCESS;
}