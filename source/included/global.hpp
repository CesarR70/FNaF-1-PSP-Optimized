#pragma once

#include <pspkernel.h>
#include <pspdebug.h>
#include <pspctrl.h>


#include <pspgu.h>
#include <pspgum.h>

#include <pspdisplay.h>
#include <stdint.h>

#include <pspmoduleinfo.h>

#include <fstream>
#include <iostream>
#include <ostream>
#include <sstream>
#include <string>
#include <unordered_map>
#include <functional>

extern "C" {
    #include "graphics.h"
    #include "image.h"
    #include "vram.h"
    #include "include/oslib.h"
}

using namespace std;

// Debug logging control - set to 0 to disable all debug prints for release
#define DEBUG_LOGGING 0

#if DEBUG_LOGGING
    #define DEBUG_PRINTF(...) printf(__VA_ARGS__)
#else
    #define DEBUG_PRINTF(...) do {} while(0)
#endif

template <typename T>
std::string toString(const T& value) {
    std::ostringstream oss;
    oss << value;
    return oss.str();
}