#include <chrono>
#include <cstdint>
#include <iostream>
#include <vector>
#include <thread>
#include <stdint.h>



std::vector<uint16_t> ports = {9000, 9001, 9002, 9003};



int main()
{
    std::cout << "HOST: udp-many scaffold starting" << std::endl;


    // sleep for 100 ms to let the clients start up & connect
    std::this_thread::sleep_for(std::chrono::milliseconds{100});


    
    
    return 0;
}
