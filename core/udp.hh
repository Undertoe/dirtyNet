#pragma once


#include <thread>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <functional>

#include "ip.hh"



// honestly we need to actually outline the extend of this. 
// this may be a place where we look at adding some template strategies?  or inheritance not sure.  
// UDP sockets should handle: 
// 1) always be non-blocking reads & writes
// 2) handle processing of data on another thread
// 3) This should have a read / write option.  probably have these completely separate.  
//      This could also be done by having a queue of data out & writing that per sequence on the separate thread.
// 4) Reads should forward back & the user should implement how to handle passing this back.  Not sure if this is a good idea yet.
// 5) What the fuck do we do about multicast?  Probably an implementation
// 6) honestly this should probably have both lockless & blocking reads & writes.  Probably better to implement these in specializations.
namespace dirtyNet
{

class udp
{
public:
    // ip & port to connect to.  Callback function needed for when data comes back.
    udp(const dirtyNet::ip&, uint16_t, std::function<bool(const std::vector<std::byte>&)>); 
    ~udp();

    udp() = delete;
    udp(const udp&) = delete;
    udp& operator=(const udp&) = delete;

    udp(udp&&) = delete;
    udp& operator=(udp&&) = delete;
    
    bool Alive() const;
    void Kill();
    bool Send(const std::vector<std::byte>&);

private:

    void run();

    std::function<bool(const std::vector<std::byte>&)> _callback;

    std::thread _thread;
    bool _alive{false};

    sockaddr_in _socket{0};

    
};

}