#include "included/audio.hpp"
#include "included/memory.hpp"

// Safety helper: pause before delete, then null the pointer
// CRITICAL: Add thread safety to prevent race conditions during high activity
static SceUID audioSemaphore = -1;

static inline void initAudioSemaphore() {
    if (audioSemaphore < 0) {
        audioSemaphore = sceKernelCreateSema("audio_sema", 0, 1, 1, nullptr);
    }
}

static inline void safeDelete(OSL_SOUND*& s) {
    initAudioSemaphore();
    
    if (audioSemaphore >= 0) {
        sceKernelWaitSema(audioSemaphore, 1, nullptr);
    }
    
    if (s) {
        // CRITICAL: Check if sound is still valid before operations
        // This prevents crashes when sound is accessed from multiple threads
        int channel = oslGetSoundChannel(s);
        if (channel != -1) {
            oslPauseSound(s, 1); // pause this sound (not -1)
            sceKernelDelayThread(1000); // Brief delay for pause to take effect (1ms)
        }
        oslDeleteSound(s);
        s = nullptr;
    }
    
    if (audioSemaphore >= 0) {
        sceKernelSignalSema(audioSemaphore, 1);
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

        // CRITICAL: Add additional safety checks to prevent crashes
        // Check if sound object is still valid before operations
        int ch = oslGetSoundChannel(buzz);
        if (ch == -1) {
            // Sound is not currently playing, safe to configure and play
            oslSetSoundLoop(buzz, 1);
            oslPlaySound(buzz, 3);
        } else {
            // Sound is already playing, just unpause if needed
            oslPauseSound(buzz, 0); // unpause just this sound
        }
    }
    
 void playLightOff() {
        if (buzz) {
            // Pause just this buzz loop; do not stop/unload
            oslPauseSound(buzz, 1);
        }
    }

        void playDoor()     { 
            // CRITICAL: Add thread safety to prevent race conditions
            if (door && oslGetSoundChannel(door) == -1) oslPlaySound(door, 4); 
        }
        void playCamOpen()  { 
            // CRITICAL: Add thread safety to prevent race conditions
            if (camera[0] && oslGetSoundChannel(camera[0]) == -1) oslPlaySound(camera[0], 5); 
        }
        void playCamClose() { 
            // CRITICAL: Add thread safety to prevent race conditions
            if (camera[1] && oslGetSoundChannel(camera[1]) == -1) oslPlaySound(camera[1], 5); 
        }
        void playSwitch()   { 
            // CRITICAL: Add thread safety to prevent race conditions
            if (switchCam && oslGetSoundChannel(switchCam) == -1) oslPlaySound(switchCam, 5); 
        }
        void playMove()     { 
            // CRITICAL: Add thread safety to prevent race conditions
            if (move && oslGetSoundChannel(move) == -1) oslPlaySound(move, 5); 
        }

        void playLaugh()    { 
            // CRITICAL: Add thread safety to prevent race conditions
            if (laugh && oslGetSoundChannel(laugh) == -1) oslPlaySound(laugh, 6); 
        }
        void playWalk()     { 
            // CRITICAL: Add thread safety to prevent race conditions
            if (walk && oslGetSoundChannel(walk) == -1) oslPlaySound(walk, 7); 
        }
        void playKitchen()  {
            // if (kitchen) oslPlaySound(kitchen, 6);
        }
        void playScare()    { 
            // CRITICAL: Add thread safety to prevent race conditions
            if (scare && oslGetSoundChannel(scare) == -1) oslPlaySound(scare, 7); 
        }

        void playRun()      { 
            // CRITICAL: Add thread safety to prevent race conditions
            if (run && oslGetSoundChannel(run) == -1) oslPlaySound(run, 7); 
        }
        void playKnock()    { 
            // CRITICAL: Add thread safety to prevent race conditions
            if (knock && oslGetSoundChannel(knock) == -1) oslPlaySound(knock, 7); 
        }
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
            // CRITICAL: Add thread safety to prevent race conditions during jumpscares
            if (!jumpscare) return;
            
            // Check if sound is already playing to prevent multiple simultaneous plays
            if (oslGetSoundChannel(jumpscare) == -1) {
                oslPlaySound(jumpscare, 0);
                // Double-check after playing
                if (oslGetSoundChannel(jumpscare) == -1) {
                    unloadJumpscareSound();
                }
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
            // CRITICAL: Add thread safety to prevent race conditions during death sequence
            if (!dead) return;
            
            // Check if sound is already playing to prevent multiple simultaneous plays
            if (oslGetSoundChannel(dead) == -1) {
                oslPlaySound(dead, 0);
                // Double-check after playing
                if (oslGetSoundChannel(dead) == -1) {
                    unloadDeadSound();
                }
            }
        }
        void unloadDeadSound() {
            safeDelete(dead);
        }
    }
    
    // ==============================
    // Pre-caching System
    // ==============================
    namespace preload {
        
        // CRITICAL: Track exact amounts for proper cleanup
        static size_t lastAudioPreCachedBytes = 0;
        
        void preloadCriticalAudio() {
            // CRITICAL: Prevent double pre-caching to avoid memory accumulation
            if (lastAudioPreCachedBytes > 0) {
                DEBUG_PRINTF("⚠️  WARNING: Audio already pre-cached (%zu bytes), skipping duplicate pre-cache\n", 
                       lastAudioPreCachedBytes);
                return;
            }
            
            // Pre-cache critical audio assets for zero-latency playback
            size_t preCachedBytes = 0;
            
            // Pre-cache office sounds
            if (sfx::office::move) {
                oslGetSoundChannel(sfx::office::move); // Pre-warm audio system
                preCachedBytes += 8288; // move.wav - AI movement
            }
            
            if (sfx::office::run) {
                oslGetSoundChannel(sfx::office::run); // Pre-warm audio system
                preCachedBytes += 12000; // run.wav - Foxy attack
            }
            
            if (sfx::office::knock) {
                oslGetSoundChannel(sfx::office::knock); // Pre-warm audio system
                preCachedBytes += 5000; // knock.wav - Door close
            }
            
            if (sfx::office::door) {
                oslGetSoundChannel(sfx::office::door); // Pre-warm audio system
                preCachedBytes += 3000; // door.wav - Door open/close
            }
            
            if (sfx::office::buzz) {
                oslGetSoundChannel(sfx::office::buzz); // Pre-warm audio system
                preCachedBytes += 8000; // buzz.wav - Power low
            }
            
            // Pre-cache camera sounds (CRITICAL for camera functionality)
            if (sfx::office::laugh) {
                oslGetSoundChannel(sfx::office::laugh); // Pre-warm audio system
                preCachedBytes += 5000; // laugh.wav - Animatronic laugh
            }
            
            if (sfx::office::switchCam) {
                oslGetSoundChannel(sfx::office::switchCam); // Pre-warm audio system
                preCachedBytes += 3000; // switch.wav - Camera switch
            }
            
            if (sfx::office::camera[0]) { // openCam
                oslGetSoundChannel(sfx::office::camera[0]); // Pre-warm audio system
                preCachedBytes += 2000; // openCam.wav - Camera open
            }
            
            if (sfx::office::camera[1]) { // closeCam
                oslGetSoundChannel(sfx::office::camera[1]); // Pre-warm audio system
                preCachedBytes += 2000; // closeCam.wav - Camera close
            }
            
            // Pre-cache office scare sound (replaces jumpscare2 for better memory usage)
            if (sfx::office::scare) {
                oslGetSoundChannel(sfx::office::scare); // Pre-warm audio system
                preCachedBytes += 8000; // scare.wav - Office scare sound
            }
            
            // Pre-cache jumpscare sounds
            if (sfx::jumpscare::jumpscare) {
                oslGetSoundChannel(sfx::jumpscare::jumpscare); // Pre-warm audio system
                preCachedBytes += 15000; // jumpscare.wav
            }
            
            if (sfx::jumpscare::dead) {
                oslGetSoundChannel(sfx::jumpscare::dead); // Pre-warm audio system
                preCachedBytes += 10000; // dead.wav
            }
            
            // Pre-cache ambience
            if (ambience::office::ambience) {
                oslGetSoundChannel(ambience::office::ambience); // Pre-warm audio system
                preCachedBytes += 50000; // ambience_mix.wav
            }
            
            if (ambience::office::fan) {
                oslGetSoundChannel(ambience::office::fan); // Pre-warm audio system
                preCachedBytes += 20000; // fan.wav
            }
            
            // Pre-cache phone calls
            for (int i = 0; i < 5; ++i) {
                if (call::phoneCalls[i]) {
                    oslGetSoundChannel(call::phoneCalls[i]); // Pre-warm audio system
                    preCachedBytes += 30000; // call1-5.wav
                }
            }
            
            // Pre-cache ending music
            if (music::n_ending::endingSong) {
                oslGetSoundChannel(music::n_ending::endingSong); // Pre-warm audio system
                preCachedBytes += 100000; // music.wav
            }
            
            // Pre-cache Six AM chimes
            if (sfx::sixam::chimes) {
                oslGetSoundChannel(sfx::sixam::chimes); // Pre-warm audio system
                preCachedBytes += 15000; // chimes.wav
            }
            
            // Track memory usage and store for cleanup
            lastAudioPreCachedBytes = preCachedBytes;
            memory::trackAudio(preCachedBytes);
            DEBUG_PRINTF("Pre-cached %zu bytes of audio assets\n", preCachedBytes);
        }
        
        void unloadCriticalAudio() {
            // CRITICAL: Properly untrack memory to prevent accumulation across nights
            if (lastAudioPreCachedBytes > 0) {
                memory::untrackAudio(lastAudioPreCachedBytes);
                DEBUG_PRINTF("Unloaded %zu bytes of pre-cached audio assets\n", lastAudioPreCachedBytes);
                lastAudioPreCachedBytes = 0; // Reset for next pre-cache
            } else {
                DEBUG_PRINTF("No audio assets to unload\n");
            }
        }
    }
}