#include <charconv>
#include <cstdint>
#include <iostream>
#include <string_view>

namespace {

bool parse_port(std::string_view text, std::uint16_t& port)
{
    unsigned int parsed_port{};
    const auto [end, error] =
        std::from_chars(text.data(), text.data() + text.size(), parsed_port);

    if (error != std::errc{} || end != text.data() + text.size()
        || parsed_port == 0 || parsed_port > 65'535) {
        return false;
    }

    port = static_cast<std::uint16_t>(parsed_port);
    return true;
}

} // namespace

int main(int argc, char* argv[])
{
    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " <port>\n";
        return 1;
    }

    std::uint16_t port{};
    if (!parse_port(argv[1], port)) {
        std::cerr << "Invalid port: " << argv[1] << '\n';
        return 1;
    }

    std::cout << "TCP multi-client client scaffold for port " << port << '\n';
    return 0;
}
