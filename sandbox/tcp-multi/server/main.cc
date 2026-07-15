#include <charconv>
#include <cstddef>
#include <cstdint>
#include <iostream>
#include <ratio>
#include <string_view>

#include <sys/types.h>
#include <system_error>
#include <thread>
#include <unistd.h>
#include <vector>
#include <algorithm>
#include <ranges>

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

// fd is the only thing we need for the connection since each should get their own buffer 
// & handling
// we added port so that we can use that for debugging.
void handle_connection(int fd, uint16_t port) 
{
    std::array<char, 64> buffer;

    // we're going to hold the unproccessed bytes here to quickly send, clear, then continue processing.
    std::string output_msg;


    bool continueProcessessing{true};
    while(continueProcessessing)
    {
        auto bytesRead = read(fd, &buffer[0], buffer.size());

        // error handling
        if(bytesRead < 0)
        {
            std::cout << "Connection error for port " << port << ".  Closing" << std::endl;
            close(fd);
            return;
        }
        
        // first escape out if we have bytesRead == 0, this is connection closed.
        if(bytesRead == 0)
        {
            std::cout << "Connection closed for port " << port << std::endl;
            close(fd);
            return;
        }
        
        // for this section we want to scan through the buffer
        // 1) new line is found, we append everything up until what we've read to the string, perform the reverse, then output the string to our connection's fd.
        // 2) we hit the end of our buffer, we then copy everything up until what we've proccessed to the end of the string.
        std::string_view bytesAvailable(buffer.data(), bytesRead);
        while(bytesAvailable.size() > 0)
        {
            auto next = bytesAvailable.find('\n');
            // we did not find, time to bail
            if(next == std::string_view::npos)
            {
                output_msg += bytesAvailable;
                break;
            }

            output_msg += bytesAvailable.substr(0, next);
            std::ranges::reverse(output_msg);
            output_msg += '\n';
            size_t totalWritten = 0;
            int failSends = 0;
            while(totalWritten < output_msg.size())
            {
                auto written = write(fd, &output_msg[totalWritten], output_msg.size() - totalWritten);
                if(written < 0)
                {
                    int error = errno;
                    std::cout << "Connection error with server and port " << port << ". closing prematurely." << std::endl;
                    std::cout << "Error code: " << std::error_code(error, std::generic_category()).message() << std::endl;
                    close(fd);
                    return;
                }
                if(written == 0)
                {
                    failSends ++;
                    if(failSends > 2)
                    {
                        std::cout << "Failed to send more than twice, something is up with the connection and we are exiting" << std::endl;
                        close(fd);
                        return;
                    }
                    std::this_thread::sleep_for(std::chrono::milliseconds{125});
                }
                totalWritten += written;
            }
            
            output_msg.clear();

            bytesAvailable.remove_prefix(next + 1); // clear out our bytes to +1 of our index.
        }
    }
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

    
    return 0;
}
