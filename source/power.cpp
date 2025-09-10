#include "included/power.hpp"

namespace power{
    int usage = 0;
    int total = 99;
    int drainTime = 600;

    int lightUsage = 0;
    int doorUsage = 0;
    int camUsage = 0;

    int tenths = 9;
    int ones = 9;

    //std::string keyState = "up";

    void reset(){
        usage = 0;
        total = 99;
        lightUsage = 0;
        doorUsage = 0;
        camUsage = 0;
        tenths = 9;
        ones = 9;
    }

    namespace render{
        void renderPowerLeft(){
            drawSpriteAlpha(0, 0, 72, 17, sprite::UI::office::usageFrame, 10, 215, 0);
            drawSpriteAlpha(0, 0, 55, 20, sprite::UI::office::powerBar[usage], 92, 215, 0);
            drawSpriteAlpha(0, 0, 127, 17, sprite::UI::office::powerLeft, 10, 245, 0);
            if (total > 9){
                drawSpriteAlpha(0, 0, 20, 20, text::global::nightNumbersPixel[tenths], 140, 243, 0);
            }
            if (total > 0){
                drawSpriteAlpha(0, 0, 20, 20, text::global::nightNumbersPixel[ones], 155, 243, 0);
            }
            drawSpriteAlpha(0, 0, 20, 20, text::global::symbols, 170, 243, 0);
        }
    }

namespace update {

    void checkDrain() {
        lightUsage = (office::leftOn ? 1 : 0) + (office::rightOn ? 1 : 0);
        doorUsage  = (office::leftClosed ? 1 : 0) + (office::rightClosed ? 1 : 0);
        camUsage   = camera::isUsing ? 1 : 0;

        usage = lightUsage + doorUsage + camUsage;
        usage = (usage > 4) ? 4 : usage;

        if (total == 0) {
            initPowerOut();
        }
    }

    void drainConstant() {
        int divisor = 0;
        switch (usage) {
            case 1: divisor = 2; break;
            case 2: divisor = 4; break;
            case 3: divisor = 6; break;
            case 4: divisor = 8; break;
        }

        if (divisor && (drainTime == 600 || drainTime == 540 || drainTime == 480 || drainTime == 420)) {
            drainTime /= divisor;
        }

        if (drainTime <= 0) {
            total -= 1;
            ones -= 1;

            if (ones < 0 && tenths > 0) {
                ones = 9;
                tenths -= 1;
            }

            setDrainTime();
        } else {
            drainTime -= 1;
        }
    }

    void setDrainTime() {
        static const int drainTimes[8] = {
            600, // unused index 0
            600, // night 1
            540, // night 2
            540, // night 3
            480, // night 4
            420, // night 5
            420, // night 6
            420  // night 7
        };
        drainTime = drainTimes[save::whichNight];
    }


        void initPowerOut(){
            state::isOffice = false;

            //sprite::UI::office::unloadPowerInfo();
            //sprite::office::unloadDoors();
            //sprite::office::unloadButtons();
            sprite::UI::office::unloadCamFlip();
            //sprite::UI::office::unloadCamUi();
            sprite::UI::office::unloadCams();

            sfx::office::unloadSfx();
            ambience::office::unloadAmbience();
            ambience::office::unloadFanSound();

            image::n_noPower::loadNoPower();

            music::n_ending::loadEndingSong();
            music::n_ending::playEndingSong();

            state::isPowerOut = true;
        }
    }
}