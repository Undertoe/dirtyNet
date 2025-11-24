
#include <unistd.h>

namespace dirtyNet::detail
{
    // takes ownership of a socket file descriptor, does not create tho
    class SocketFD {
    public:
        SocketFD() = default;
        explicit SocketFD(int fd) : _fd(fd) {}
        ~SocketFD() { reset(); }

        SocketFD(const SocketFD&) = delete;
        SocketFD& operator=(const SocketFD&) = delete;
        SocketFD(SocketFD&& other) noexcept : _fd(other._fd) {
            other._fd = -1;
        }
        SocketFD& operator=(SocketFD&& other) noexcept {
            if (this != &other) {
                reset();
                _fd = other._fd;
                other._fd = -1;
            }
            return *this;
        }

        void reset() {
            if (_fd != -1) {
                ::close(_fd);
                _fd = -1;
            }
        }

        int get() const { return _fd; }
        explicit operator bool() const { return _fd != -1; }

    private:
        int _fd{-1};
    };
}