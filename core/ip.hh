#include "ipv4.hh"
#include "ipv6.hh"

#include <variant>


namespace dirtyNet
{


enum class ip_type
{
    ipv4, ipv6
};

// This is a read only representation of an ip address, containing either ipv4 or ipv6
class ip{
public:
    using AddressType = std::variant<in_addr,in6_addr>;

    ip(ipv4&&);
    ip(ipv6&&);
    ip(in_addr);
    ip(in6_addr);
    ip(const AddressType&);
    ip(std::string_view);

    // read only, cannot copy or delete this object
    ip(const ip&) = delete;
    ip& operator=(const ip&) = delete;
    ip(ip&&) = delete;
    ip& operator=(ip&&) = delete;


    bool Valid() const;
    std::variant<in_addr,in6_addr> Address() const;
    std::string_view String() const;

    ip_type Type() const;
    

private:

    std::variant<ipv4, ipv6> _ip{in_addr{0u}};

    static std::variant<ipv4, ipv6> parse(std::string_view);

    static std::variant<ipv4, ipv6> init_from_addresstype(const dirtyNet::ip::AddressType& ip);
    


    struct AddressVisitor
    {
        in_addr operator()(dirtyNet::ipv4 ip) const
        {
            return ip.Address();
        }

        in6_addr operator()(dirtyNet::ipv6 ip) const
        {
            return ip.Address();
        }
    };

};

}