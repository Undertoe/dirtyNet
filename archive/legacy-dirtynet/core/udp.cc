
#include "udp.hh"


dirtyNet::udp::udp(const dirtyNet::ip& ip, uint16_t port, std::function<bool(const std::vector<std::byte>&)> callback) : _callback(callback)
{
    // kill this full stop if this is an invalid 
    if(!ip.Valid())
    {
        // failure
        return;
    }
    int sock;

    switch(ip.Type())
    {
        case dirtyNet::ip_type::ipv4:
        {
            _socket.sin_family = AF_INET;
            _socket.sin_port = htons(port);
            _socket.sin_addr.s_addr = htonl(std::get<in_addr>(ip.Address()).s_addr);
            sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
            break;
        }
        // first pass not allowing ipv6 for now.
        case dirtyNet::ip_type::ipv6:
        {
            // _socket.sin_family = AF_INET6;
            // _socket.sin_port = htons(port);
            // _socket.sin_addr.s_addr = htonl(std::get<in_addr>(ip.Address()).s_addr);
            // break;
            return;
        }
        default:
        {
            // invalid socket type, break out here.
            return;
        }
    }

    if( bind(sock, (sockaddr*)&_socket, sizeof(_socket)) == -1)
    {
        // failure
        return;
    }

    _thread = std::thread(&udp::run, this);
}


dirtyNet::udp::~udp()
{
    _thread.join();
}



bool
dirtyNet::udp::Alive() const
{
    return _alive;
}

void 
dirtyNet::udp::Kill()
{
    _alive = false;
}


// this needs proper synchronization & what not.  Please fix this after
// we get this running
void
dirtyNet::udp::run()
{
    while(_alive)
    {
        
    }
}