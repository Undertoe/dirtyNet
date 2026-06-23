#include <iostream>

int main(int argc, char* argv[])
{
    auto client_id = argc > 1 ? argv[1] : "0";
    std::cout << "CLIENT " << client_id << ": udp-many scaffold starting" << std::endl;
    std::cout << "CLIENT " << client_id << ": receiver implementation goes here" << std::endl;
    return 0;
}
