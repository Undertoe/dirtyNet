#include "ipv4.hh"
#include "ipv6.hh"

#include <variant>


namespace dirtyNet
{


enum class ip_type
{
    ipv4, ipv6
};
class ip{
public:
    using AddressType = std::variant<in_addr,in6_addr>;

    ip(ipv4&&);
    ip(ipv6&&);
    ip(in_addr);
    ip(in6_addr);
    ip(const AddressType&);
    ip(std::string_view);

    ip(const ip&) = delete;
    ip& operator=(const ip&) = delete;
    ip(ip&&) = delete;
    ip& operator=(ip&&) = delete;


    bool Valid() const;
    std::variant<in_addr,in6_addr> Address() const;
    std::string_view String() const;

    ip_type Type() const;
    

private:

    std::variant<ipv4, ipv6> _ip;


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