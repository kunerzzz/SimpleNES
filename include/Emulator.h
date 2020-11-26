#ifndef EMULATOR_H
#define EMULATOR_H
#include <chrono>

#include "CPU.h"
#include "PPU.h"
#include "MainBus.h"
#include "PictureBus.h"
#include "Controller.h"
#include "FrameBuffer.h"

namespace sn
{
    using TimePoint = std::chrono::steady_clock::time_point;

    const int NESVideoWidth = ScanlineVisibleDots;
    const int NESVideoHeight = VisibleScanlines;

    class Emulator
    {
    public:
        Emulator();
        void run(std::string rom_path);
        void setVideoWidth(int width);
        void setVideoHeight(int height);
        void setVideoScale(float scale);
        void setKeys(std::vector<uint16_t>& p1, std::vector<uint16_t>& p2);
        void cleanup();
    private:
        void DMA(Byte page);

        MainBus m_bus;
        PictureBus m_pictureBus;
        CPU m_cpu;
        PPU m_ppu;
        Cartridge m_cartridge;
        std::unique_ptr<Mapper> m_mapper;

        Controller m_controller1, m_controller2;

        VirtualScreen m_emulatorScreen;
        FrameBuffer m_framebuffer;

        TimePoint m_cycleTimer;

        std::chrono::high_resolution_clock::duration m_elapsedTime;
        std::chrono::nanoseconds m_cpuCycleDuration;
    };
}
#endif // EMULATOR_H
