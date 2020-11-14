#include "Controller.h"
#include "Log.h"

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

        inputHandler = NULL;
    }

    void Controller::create(std::string dev_path) {
        inputHandler = InputHandler::Get(dev_path);
        inputHandler->bind(); 
    }

    void Controller::setKeyBindings(const std::vector<uint16_t>& keys)
    {
        m_keyBindings = keys;
    }

    void Controller::strobe(Byte b)
    {
        if(!inputHandler)
            return;
        m_strobe = (b & 1);
        if (!m_strobe)
        {
            m_keyStates = 0;
            int shift = 0;
            for (int button = A; button < TotalButtons; ++button)
            {
                m_keyStates |= (inputHandler->isKeyPressed(m_keyBindings[static_cast<Buttons>(button)]) << shift);
                ++shift;
            }
        }
    }

    Byte Controller::read()
    {
        if(!inputHandler)
            return 0x40;
        Byte ret;
        if (m_strobe)
            ret = inputHandler->isKeyPressed(m_keyBindings[A]);
        else
        {
            ret = (m_keyStates & 1);
            m_keyStates >>= 1;
        }
        return ret | 0x40;
    }

    Controller::~Controller() {
        LOG(Info) << "Delete controller in" << std::endl;
        if(inputHandler)
            inputHandler->unbind();
        LOG(Info) << "Delete controller out" << std::endl;
    }
}