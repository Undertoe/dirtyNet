#include "shared.hh"

#include <iostream>

int main()
{
    std::cout << "TCP packet server: " << tcp_packet::hello_world() << '\n';
    return 0;
}
