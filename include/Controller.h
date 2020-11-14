#ifndef CONTROLLER_H
#define CONTROLLER_H
#include <cstdint>
#include <vector>
#include "InputHandler.h"

namespace sn
{
    using Byte = std::uint8_t;
    class Controller
    {
    public:
        Controller();
        enum Buttons
        {
            A,
            B,
            Select,
            Start,
            Up,
            Down,
            Left,
            Right,
            TotalButtons,
        };

        void strobe(Byte b);
        Byte read();
        void setKeyBindings(const std::vector<uint16_t>& keys);
        void create(const char *dev_path);
    private:
        bool m_strobe;
        unsigned int m_keyStates;

        InputHandler inputHandler;
        std::vector<uint16_t> m_keyBindings;
//         sf::Keyboard::Key m_keyBindings[TotalButtons];
    };
}

#endif // CONTROLLER_H
