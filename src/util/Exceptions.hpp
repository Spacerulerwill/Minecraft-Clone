#include <string>
#include <stdexcept>

namespace engine{
    class WorldCorruptException: public std::runtime_error{
        public:
        WorldCorruptException():runtime_error("World data is corrupted!"){}
        WorldCorruptException(std::string msg):runtime_error(msg.c_str()){}
    };
}