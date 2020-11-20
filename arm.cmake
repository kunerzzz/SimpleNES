set(CMAKE_SYSTEM_NAME Linux)
set(CMAKE_SYSTEM_PROCESSOR arm)

set(tools /opt/arm-buildroot-linux-gnueabihf_sdk-buildroot)
set(CMAKE_C_COMPILER ${tools}/bin/arm-none-linux-gnueabihf-gcc)
set(CMAKE_CXX_COMPILER ${tools}/bin/arm-none-linux-gnueabihf-g++)
