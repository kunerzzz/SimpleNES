#ifndef FRAMEBUFFER_H
#define FRAMEBUFFER_H

#include <stdint.h>
#include <chrono>
#include <deque>

#include "VirtualScreen.h"

namespace sn
{
    using TimePoint = std::chrono::steady_clock::time_point;
    class FrameBuffer
    {
    public:
        void create(int originalWidth, int originalHeight);
        void draw(VirtualScreen &m_emulatorScreen);
        void display();
        ~FrameBuffer();

    private:
        int fbdev;
        uint32_t *fbmem;
        uint32_t *fbmem_off; //Off-Screen Buffer
        uint32_t fbmem_size;

        //Original image size
        int originalWidth;
        int originalHeight;

        //Scaled image size
        int scaledWidth;
        int scaledHeight;
        int scaledXMargin;
        int scaledYMargin;

        //Framebuffer variable props
        int fbWidth;
        int fbHeight;
        int fbVWidth;
        int fbVHeight;
        int fbOffsetX;
        int fbOffsetY;

        #ifdef ENABLE_PERFORMANCE_REPORT
        //Performance data
        const int RECORD_FRAMES = 200;
        const std::chrono::milliseconds REPORT_INTERVAL = std::chrono::milliseconds(1000);
        TimePoint last_display_time;
        TimePoint last_report_time;
        std::deque<std::chrono::milliseconds> recent_frame_costs;
        #endif

        int getVMemOffset(int x, int y)
        {
            return (fbOffsetY + y) * fbVWidth + fbOffsetX + x;
        }

        uint32_t getOrginalPixel(VirtualScreen &m_emulatorScreen, int x, int y)
        {
            int ox = x * originalWidth / scaledWidth;
            int oy = y * originalHeight / scaledHeight;

            return m_emulatorScreen.getPixel(ox, oy);
        }
    };
} // namespace sn

#endif