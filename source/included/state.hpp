#include "global.hpp"

namespace state{
    extern bool isMenu;
    extern bool isNewspaper;
    extern bool isNightinfo;
    extern bool isOffice;
    extern bool isCustomNight;
    extern bool isSixAm;
    extern bool isPowerOut;
    extern bool isDead;
    extern bool isEnding;
    extern bool isJumpscare;
    extern volatile bool isFoxyAttackPaused; // volatile for thread safety

    //extern bool endOfNight;
}