#include "FrameBuffer.h"
#include "Log.h"

#include <unistd.h>
#include <fcntl.h>
#include <linux/fb.h>
#include <sys/mman.h>
#include <sys/ioctl.h>
#include <linux/kd.h>
#include <cstdlib>
#include <vector>
#include <algorithm>

namespace sn
{
    void FrameBuffer::create(int w, int h) {
        char default_fbpath[] = "/dev/fb0";

        this->originalWidth = w;
        this->originalHeight = h;

        char *fbpath = getenv("FRAMEBUFFER");
        if(!fbpath)
            fbpath = default_fbpath;

        this->fbdev = open(fbpath, O_RDWR);
        if(this->fbdev < 0) {
            LOG(Error) << "Can not open fb device " << fbpath << "." << std::endl;
            return;
        }

        struct fb_var_screeninfo vinfo;
        if(ioctl(fbdev, FBIOGET_VSCREENINFO, &vinfo)) {
            LOG(Error) << "Can not read vinfo." << std::endl;
            return;
        }

        if(vinfo.bits_per_pixel != 32) {
            LOG(Error) << "Can not handle bits_per_pixel " << vinfo.bits_per_pixel << std::endl;
            return;
        }

        int console_fd = open("/dev/console", O_RDWR);   
        if(ioctl(console_fd, KDSETMODE, KD_GRAPHICS)) {
            LOG(Error) << "Can not set console mode to graphic " << errno << std::endl;
        }
        close(console_fd);

        fbWidth = vinfo.xres;
        fbHeight = vinfo.yres;
        fbVWidth = vinfo.xres_virtual;
        fbVHeight = vinfo.yres_virtual;
        fbOffsetX = vinfo.xoffset;
        fbOffsetY = vinfo.yoffset;

        float xratio = float(fbWidth) / originalWidth;
        float yratio = float(fbHeight) / originalHeight;
        if(xratio > yratio) {
            scaledHeight = fbHeight;
            scaledWidth = originalWidth * yratio;
            if(scaledWidth > fbWidth)
                scaledWidth = fbWidth;
            
            scaledYMargin = 0;
            scaledXMargin = (fbWidth - scaledWidth) / 2;
        } else {
            scaledWidth = fbWidth;
            scaledHeight = originalHeight * xratio;
            if(scaledHeight > fbHeight)
                scaledHeight = fbHeight;
            
            scaledXMargin = 0;
            scaledYMargin = (fbHeight - scaledHeight) / 2;
        }

        fbmem_size = fbVWidth * fbVHeight * 4;
        fbmem = (uint32_t *)mmap(0, fbmem_size, PROT_READ | PROT_WRITE, MAP_SHARED, fbdev, 0);
        fbmem_off = (uint32_t *)malloc(fbmem_size);

        #ifdef ENABLE_PERFORMANCE_REPORT
        last_display_time = last_report_time = std::chrono::steady_clock::now();
        #endif

        LOG(Info) << "Init Frame Buffer: \n";
        LOG(Info) << "Res: " << fbWidth << "x" << fbHeight << std::endl;
        LOG(Info) << "VRes: " << fbVWidth << "x" << fbVHeight << std::endl;
        LOG(Info) << "Original Image size: " << originalWidth << "x" << originalHeight << std::endl;
        LOG(Info) << "Scaled Image size: " << scaledWidth << "x" << scaledHeight << std::endl;   
    }

    void FrameBuffer::draw(VirtualScreen &m_emulatorScreen) {
        for(unsigned y = 0;y < scaledHeight;++y)
            for(unsigned x = 0;x < scaledWidth;++x) {
                unsigned fx = x + scaledXMargin;
                unsigned fy = y + scaledYMargin;
                fbmem_off[getVMemOffset(fx, fy)] = getOrginalPixel(m_emulatorScreen, x, y);
            }
    }

    void FrameBuffer::drawWithoutScaling(uint32_t *buffer) {
        unsigned marginX = (fbWidth - originalWidth) / 2 + fbOffsetX;
        unsigned marginY = (fbHeight - originalHeight) / 2 + fbOffsetY;
        unsigned start = marginY * fbVWidth + marginX;
        for(int i = 0;i < originalHeight;++i) {
            memcpy(fbmem_off + start, buffer + i * originalWidth, sizeof(uint32_t) * originalWidth);
            start += fbVWidth;
        }
    }

    void FrameBuffer::display() {
        #ifdef ENABLE_PERFORMANCE_REPORT
        TimePoint cur_time = std::chrono::steady_clock::now();
        // static const int RECORD_FRAMES = 200;
        // static const std::chrono::milliseconds REPORT_INTERVAL = std::chrono::milliseconds(1000);
        // static TimePoint last_display_time;
        // static TimePoint last_report_time;
        // static std::deque<std::chrono::milliseconds> recent_frame_costs;
        std::chrono::milliseconds frame_cost = std::chrono::duration_cast<std::chrono::milliseconds>(cur_time - last_display_time);
        std::chrono::milliseconds since_last_report = std::chrono::duration_cast<std::chrono::milliseconds>(cur_time - last_report_time);

        recent_frame_costs.push_back(frame_cost);
        if(recent_frame_costs.size() > RECORD_FRAMES) {
            recent_frame_costs.pop_front();
            if(since_last_report > REPORT_INTERVAL) {
                
                auto temp = recent_frame_costs;
                std::sort(temp.begin(), temp.end(), std::greater<std::chrono::milliseconds>());
                int percent_1_pos = RECORD_FRAMES / 100;
                int percent_5_pos = RECORD_FRAMES / 20;
                int percent_10_pos = RECORD_FRAMES / 10;

                long percent_1_val = temp[percent_1_pos].count();
                long percent_5_val = temp[percent_5_pos].count();
                long percent_10_val = temp[percent_10_pos].count();
                
                std::chrono::milliseconds sum = std::chrono::milliseconds(0);
                for(std::chrono::milliseconds cost : temp) {
                    sum += cost;
                }

                double avg_frame_rate = RECORD_FRAMES * 1000 / sum.count();


                LOG(Info) << std::endl
                        << "==========Performance Report==========" << std::endl
                        << "Frame Rate (fps): " << avg_frame_rate << std::endl
                        << "1% max Frame Cost (ms): " << percent_1_val << std::endl
                        << "5% max Frame Cost (ms): " << percent_5_val << std::endl
                        << "10% max Frame Cost (ms): " << percent_10_val << std::endl
                        << "==================END=================" << std::endl;
                last_report_time = cur_time;
            }
        }

        last_display_time = cur_time;
        #endif
        memcpy(fbmem, fbmem_off, fbmem_size);
    }

    FrameBuffer::~FrameBuffer() {
        int console_fd = open("/dev/console", O_RDWR);   
        if(ioctl(console_fd, KDSETMODE, KD_TEXT)) {
            LOG(Error) << "Can not set console mode to text " << errno << std::endl;
        }
        close(console_fd);
        // LOG(Info) << "Delete FrameBuffer" << std::endl;
    }
}