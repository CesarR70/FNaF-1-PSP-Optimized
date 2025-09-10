#include "included/audio.hpp"

// Safety helper: pause before delete, then null the pointer
static inline void safeDelete(OSL_SOUND*& s) {
    if (s) {
        oslPauseSound(s, 1); // pause this sound (not -1)
        oslDeleteSound(s);
        s = nullptr;
    }
}

namespace music {
    namespace menu {
        OSL_SOUND* menuMusic = nullptr;

        void loadMenuMusic() {
            safeDelete(menuMusic);
            // Keep original format choice
            menuMusic = oslLoadSoundFileWAV("romfs/music/menu/music.wav", OSL_FMT_NONE);
        }
        void playMenuMusic() {
            if (!menuMusic) return;
            // Set loop first, then ensure it’s playing
            oslSetSoundLoop(menuMusic, 1);
            if (oslGetSoundChannel(menuMusic) == -1) {
                oslPlaySound(menuMusic, 0);
            }
        }
        void unloadMenuMusic() {
            safeDelete(menuMusic);
        }
    }

    namespace n_ending {
        OSL_SOUND* endingSong = nullptr;
        bool stopped = false;

        void loadEndingSong() {
            safeDelete(endingSong);
            endingSong = oslLoadSoundFileWAV("romfs/music/ending/music.wav", OSL_FMT_STREAM);
            stopped = false;
        }
        void playEndingSong() {
            if (!endingSong) return;
            if (oslGetSoundChannel(endingSong) == -1) {
                oslPlaySound(endingSong, 0);
            }
        }
        void stopEndingSong() {
            safeDelete(endingSong);
            stopped = true;
        }
        void unloadEndingSong() {
            safeDelete(endingSong);
            stopped = true;
        }
    }
}

namespace ambience {
    namespace office {
        OSL_SOUND* ambience = nullptr;
        OSL_SOUND* fan = nullptr;

        void loadAmbience() {
            safeDelete(ambience);
            ambience = oslLoadSoundFileWAV("romfs/ambience/office/ambience_mix.wav", OSL_FMT_STREAM);
        }
        void playAmbience() {
            if (!ambience) return;
            oslSetSoundLoop(ambience, 1);
            if (oslGetSoundChannel(ambience) == -1) {
                oslPlaySound(ambience, 0);
            }
        }
        void unloadAmbience() {
            safeDelete(ambience);
        }

        void loadFanSound() {
            safeDelete(fan);
            fan = oslLoadSoundFileWAV("romfs/ambience/office/fan.wav", OSL_FMT_STREAM);
        }
        void playFanSound() {
            if (!fan) return;
            oslSetSoundLoop(fan, 1);
            if (oslGetSoundChannel(fan) == -1) {
                oslPlaySound(fan, 2);
            }
        }
        void unloadFanSound() {
            safeDelete(fan);
        }
    }
}

namespace call {
    OSL_SOUND* phoneCalls[5] = {nullptr, nullptr, nullptr, nullptr, nullptr};
    bool stopped = false;

    void loadPhoneCalls() {
        int nightIndex = save::whichNight - 1;
        if (nightIndex >= 0 && nightIndex < 5) {
            safeDelete(phoneCalls[nightIndex]);
            std::string filePath = "romfs/ambience/office/call/call" + toString(nightIndex + 1) + ".wav";
            phoneCalls[nightIndex] = oslLoadSoundFileWAV(filePath.c_str(), OSL_FMT_STREAM);
            stopped = false;
        }
    }

    void playPhoneCalls() {
        int nightIndex = save::whichNight - 1;
        if (nightIndex >= 0 && nightIndex < 5 && phoneCalls[nightIndex]) {
            oslPlaySound(phoneCalls[nightIndex], 1);
            if (oslGetSoundChannel(phoneCalls[nightIndex]) == -1) {
                unloadPhoneCalls(); // preserve original behavior
            }
        }
    }

    void unloadPhoneCalls() {
        int nightIndex = save::whichNight - 1;
        if (nightIndex >= 0 && nightIndex < 5 && phoneCalls[nightIndex]) {
            safeDelete(phoneCalls[nightIndex]);
            phoneCalls[nightIndex] = nullptr;
        }
        stopped = true;
    }
}

namespace sfx {
    namespace office {
        OSL_SOUND* buzz = nullptr;
        OSL_SOUND* door = nullptr;
        OSL_SOUND* scare = nullptr;
        OSL_SOUND* switchCam = nullptr;
        OSL_SOUND* laugh = nullptr;
        OSL_SOUND* move = nullptr;
        OSL_SOUND* walk = nullptr;
        // OSL_SOUND* kitchen = nullptr; // disabled
        OSL_SOUND* run = nullptr;
        OSL_SOUND* knock = nullptr;
        OSL_SOUND* camera[2] = {nullptr, nullptr};

        void loadSfx() {
            // Clear any previous handles first
            safeDelete(buzz);
            safeDelete(door);
            safeDelete(scare);
            safeDelete(switchCam);
            safeDelete(laugh);
            safeDelete(move);
            safeDelete(walk);
            // safeDelete(kitchen);
            safeDelete(run);
            safeDelete(knock);
            safeDelete(camera[0]);
            safeDelete(camera[1]);

            // Keep your original format choices
            buzz      = oslLoadSoundFileWAV("romfs/sfx/office/buzz.wav",    OSL_FMT_NONE);
            door      = oslLoadSoundFileWAV("romfs/sfx/office/door.wav",    OSL_FMT_NONE);
            scare     = oslLoadSoundFileWAV("romfs/sfx/office/scare.wav",   OSL_FMT_NONE);
            switchCam = oslLoadSoundFileWAV("romfs/sfx/office/switch.wav",  OSL_FMT_NONE);
            laugh     = oslLoadSoundFileWAV("romfs/sfx/office/laugh.wav",   OSL_FMT_STREAM);
            move      = oslLoadSoundFileWAV("romfs/sfx/office/move.wav",    OSL_FMT_NONE);
            walk      = oslLoadSoundFileWAV("romfs/sfx/office/walk.wav",    OSL_FMT_NONE);
            // kitchen = oslLoadSoundFileWAV("romfs/sfx/office/kitchen.wav", OSL_FMT_STREAM);
            run       = oslLoadSoundFileWAV("romfs/sfx/office/run.wav",     OSL_FMT_STREAM);
            knock     = oslLoadSoundFileWAV("romfs/sfx/office/knock.wav",   OSL_FMT_STREAM);

            camera[0] = oslLoadSoundFileWAV("romfs/sfx/office/openCam.wav", OSL_FMT_NONE);
            camera[1] = oslLoadSoundFileWAV("romfs/sfx/office/closeCam.wav",OSL_FMT_NONE);
        }

        void stopSfx() {
            if (buzz)      oslPauseSound(buzz, 1);
            if (door)      oslPauseSound(door, 1);
            if (scare)     oslPauseSound(scare, 1);
            if (switchCam) oslPauseSound(switchCam, 1);
            if (laugh)     oslPauseSound(laugh, 1);
            if (move)      oslPauseSound(move, 1);
            if (walk)      oslPauseSound(walk, 1);
            // if (kitchen) oslPauseSound(kitchen, 1);
            if (run)       oslPauseSound(run, 1);
            if (knock)     oslPauseSound(knock, 1);
            if (camera[0]) oslPauseSound(camera[0], 1);
            if (camera[1]) oslPauseSound(camera[1], 1);
        }

        void unloadSfx() {
            safeDelete(buzz);
            safeDelete(door);
            safeDelete(scare);
            safeDelete(switchCam);
            safeDelete(laugh);
            safeDelete(move);
            safeDelete(walk);
            // safeDelete(kitchen);
            safeDelete(run);
            safeDelete(knock);
            safeDelete(camera[0]);
            safeDelete(camera[1]);
        }

     void playLightOn() {
        if (!buzz) return;

        // Always loop the buzz
        oslSetSoundLoop(buzz, 1);

        int ch = oslGetSoundChannel(buzz);
        if (ch == -1) {
            // Not assigned/playing yet: start on the reserved voice
            oslPlaySound(buzz, 3);
        } else {
            // Has a channel already: make sure it’s audible if it was paused
            oslPauseSound(buzz, 0); // unpause just this sound
            // If you want the buzz to always restart from the beginning on each press,
            // uncomment the next line:
            // oslPlaySound(buzz, 3);
        }
    }
    
 void playLightOff() {
        if (buzz) {
            // Pause just this buzz loop; do not stop/unload
            oslPauseSound(buzz, 1);
        }
    }

        void playDoor()     { if (door)      oslPlaySound(door,      4); }
        void playCamOpen()  { if (camera[0]) oslPlaySound(camera[0], 5); }
        void playCamClose() { if (camera[1]) oslPlaySound(camera[1], 5); }
        void playSwitch()   { if (switchCam) oslPlaySound(switchCam, 5); }
        void playMove()     { if (move)      oslPlaySound(move,      5); }

        void playLaugh()    { if (laugh)     oslPlaySound(laugh,     6); }
        void playWalk()     { if (walk)      oslPlaySound(walk,      7); }
        void playKitchen()  {
            // if (kitchen) oslPlaySound(kitchen, 6);
        }
        void playScare()    { if (scare)     oslPlaySound(scare,     7); }

        void playRun()      { if (run)       oslPlaySound(run,       7); }
        void playKnock()    { if (knock)     oslPlaySound(knock,     7); }
    }

    namespace sixam {
        OSL_SOUND* chimes = nullptr;
        // OSL_SOUND* hooray = nullptr;

        void loadSixAm() {
            safeDelete(chimes);
            chimes = oslLoadSoundFileWAV("romfs/sfx/sixam/chimes.wav", OSL_FMT_STREAM);
            // hooray = oslLoadSoundFileWAV("romfs/sfx/sixam/hooray.wav", OSL_FMT_STREAM);
        }
        void unloadSixAm() {
            safeDelete(chimes);
            // safeDelete(hooray);
        }

        void playSixAm() {
            if (chimes) oslPlaySound(chimes, 0);
            // if (hooray) oslPlaySound(hooray, 0);
        }
    }

    namespace jumpscare {
        OSL_SOUND* jumpscare = nullptr;
        OSL_SOUND* jumpscare2 = nullptr; // only used in custom night
        OSL_SOUND* dead = nullptr;

        void loadJumpscareSound() {
            safeDelete(jumpscare);
            jumpscare = oslLoadSoundFileWAV("romfs/sfx/jumpscare/jumpscare.wav", OSL_FMT_STREAM);
        }
        void playJumpscareSound() {
            if (!jumpscare) return;
            oslPlaySound(jumpscare, 0);
            if (oslGetSoundChannel(jumpscare) == -1) {
                unloadJumpscareSound();
            }
        }
        void unloadJumpscareSound() {
            safeDelete(jumpscare);
        }

        void loadJumpscare2Sound() {
            safeDelete(jumpscare2);
            jumpscare2 = oslLoadSoundFileWAV("romfs/sfx/jumpscare/jumpscare2.wav", OSL_FMT_STREAM);
        }
        void playJumpscare2Sound() {
            if (jumpscare2) oslPlaySound(jumpscare2, 0);
        }
        void unloadJumpscare2Sound() {
            safeDelete(jumpscare2);
        }

        void loadDeadSound() {
            safeDelete(dead);
            dead = oslLoadSoundFileWAV("romfs/sfx/jumpscare/dead.wav", OSL_FMT_STREAM);
        }
        void playDeadSound() {
            if (!dead) return;
            oslPlaySound(dead, 0);
            if (oslGetSoundChannel(dead) == -1) {
                unloadDeadSound();
            }
        }
        void unloadDeadSound() {
            safeDelete(dead);
        }
    }
}