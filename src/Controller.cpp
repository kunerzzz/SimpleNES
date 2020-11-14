#include "Controller.h"

namespace sn
{
    Controller::Controller() :
        m_keyStates(0),
        m_keyBindings(TotalButtons)
    {
        m_keyBindings[A] = KEY_J;
        m_keyBindings[B] = KEY_K;
        m_keyBindings[Select] = KEY_RIGHTSHIFT;
        m_keyBindings[Start] = KEY_ENTER;
        m_keyBindings[Up] = KEY_W;
        m_keyBindings[Down] = KEY_S;
        m_keyBindings[Left] = KEY_A;
        m_keyBindings[Right] = KEY_D;
    }

    void Controller::create(const char *dev_path) {
        inputHandler.create(dev_path);
    }

    void Controller::setKeyBindings(const std::vector<uint16_t>& keys)
    {
        m_keyBindings = keys;
    }

    void Controller::strobe(Byte b)
    {
        m_strobe = (b & 1);
        if (!m_strobe)
        {
            m_keyStates = 0;
            int shift = 0;
            for (int button = A; button < TotalButtons; ++button)
            {
                m_keyStates |= (inputHandler.isKeyPressed(m_keyBindings[static_cast<Buttons>(button)]) << shift);
                ++shift;
            }
        }
    }

    Byte Controller::read()
    {
        Byte ret;
        if (m_strobe)
            ret = inputHandler.isKeyPressed(m_keyBindings[A]);
        else
        {
            ret = (m_keyStates & 1);
            m_keyStates >>= 1;
        }
        return ret | 0x40;
        return 0x40;
    }

}