#include "Emulator.h"
#include "Log.h"
#include <string>
#include <sstream>
#include <signal.h>

namespace sn
{
    // void parseControllerConf(std::string filepath,
    //                         std::vector<sf::Keyboard::Key>& p1,
    //                         std::vector<sf::Keyboard::Key>& p2);
    Emulator *p_emulator;

    void exit_gracefully() {
        LOG(Info) << "Exit gracefully" << std::endl;
        if(p_emulator) {
            p_emulator->cleanup();
        }
        exit(0);
    }
}

void signal_handler(int sig) {
    LOG(sn::Info) << "Received SIGINT" << std::endl;
    sn::exit_gracefully();
}

int main(int argc, char** argv)
{
    std::ofstream logFile ("simplenes.log"), cpuTraceFile;
    sn::TeeStream logTee (logFile, std::cout);

    if (logFile.is_open() && logFile.good())
        sn::Log::get().setLogStream(logTee);
    else
        sn::Log::get().setLogStream(std::cout);

    sn::Log::get().setLevel(sn::Info);

    std::string path;

    //Default keybindings
    std::vector<uint16_t> p1 {KEY_J, KEY_K, KEY_RIGHTSHIFT, KEY_ENTER,
                              KEY_W, KEY_S, KEY_A, KEY_D},
                          p2 {KEY_KP5, KEY_KP6, KEY_KP8, KEY_KP9,
                              KEY_UP, KEY_DOWN, KEY_LEFT, KEY_RIGHT};
    sn::Emulator emulator;
    sn::p_emulator = &emulator;

    signal(SIGINT, signal_handler);

    for (int i = 1; i < argc; ++i)
    {
        std::string arg (argv[i]);
        if (arg == "-h" || arg == "--help")
        {
            std::cout << "SimpleNES is a simple NES emulator.\n"
                      << "It can run off .nes images.\n"
                      << "Set keybindings with keybindings.conf\n\n"
                      << "Usage: SimpleNES [options] rom-path\n\n"
                      << "Options:\n"
                      << "-h, --help             Print this help text and exit\n"
                    //   << "-s, --scale            Set video scale. Default: 2.\n"
                    //   << "                       Scale of 1 corresponds to " << sn::NESVideoWidth << "x" << sn::NESVideoHeight << std::endl
                    //   << "-w, --width            Set the width of the emulation screen (height is\n"
                    //   << "                       set automatically to fit the aspect ratio)\n"
                    //   << "-H, --height           Set the height of the emulation screen (width is\n"
                    //   << "                       set automatically to fit the aspect ratio)\n"
                    //   << "                       This option is mutually exclusive to --width\n"
                      << std::endl;
            return 0;
        }
        else if (std::strcmp(argv[i], "--log-cpu") == 0)
        {
            sn::Log::get().setLevel(sn::CpuTrace);
            cpuTraceFile.open("sn.cpudump");
            sn::Log::get().setCpuTraceStream(cpuTraceFile);
            LOG(sn::Info) << "CPU logging set." << std::endl;
        }
        else if (std::strcmp(argv[i], "-s") == 0 || std::strcmp(argv[i], "--scale") == 0)
        {
            float scale;
            std::stringstream ss;
            if (i + 1 < argc && ss << argv[i + 1] && ss >> scale)
                emulator.setVideoScale(scale);
            else
                LOG(sn::Error) << "Setting scale from argument failed" << std::endl;
            ++i;
        }
        else if (std::strcmp(argv[i], "-w") == 0 || std::strcmp(argv[i], "--width") == 0)
        {
            int width;
            std::stringstream ss;
            if (i + 1 < argc && ss << argv[i + 1] && ss >> width)
                emulator.setVideoWidth(width);
            else
                LOG(sn::Error) << "Setting width from argument failed" << std::endl;
            ++i;
        }
        else if (std::strcmp(argv[i], "-H") == 0 || std::strcmp(argv[i], "--height") == 0)
        {
            int height;
            std::stringstream ss;
            if (i + 1 < argc && ss << argv[i + 1] && ss >> height)
                emulator.setVideoHeight(height);
            else
                LOG(sn::Error) << "Setting height from argument failed" << std::endl;
            ++i;
        }
        else if (argv[i][0] != '-')
            path = argv[i];
        else
            std::cerr << "Unrecognized argument: " << argv[i] << std::endl;
    }

    if (path.empty())
    {
        std::cout << "Argument required: ROM path" << std::endl;
        return 1;
    }

    // sn::parseControllerConf("keybindings.conf", p1, p2);
    emulator.setKeys(p1, p2);
    emulator.run(path);
    return 0;
}
