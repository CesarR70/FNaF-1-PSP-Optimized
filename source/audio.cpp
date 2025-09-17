#include "included/audio.hpp"
#include "included/memory.hpp"

// Safety helper: pause before delete, then null the pointer
// CRITICAL: Add thread safety to prevent race conditions during high activity
static inline void safeDelete(OSL_SOUND*& s) {
    if (s) {
        // CRITICAL: Check if sound is still valid before operations
        // This prevents crashes when sound is accessed from multiple threads
        int channel = oslGetSoundChannel(s);
        if (channel != -1) {
            oslPauseSound(s, 1); // pause this sound (not -1)
        }
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
        
        void preloadAmbience() {
            // ULTRA-AGGRESSIVE: Pre-cache ambience audio for zero-latency
            // This ensures smooth atmospheric audio without any loading stutter
            if (ambience) {
                // Pre-warm ambience audio (1.26 MB) - force audio system processing
                oslGetSoundChannel(ambience);
                memory::trackAudioAmbience(1323150); // ambience_mix.wav size
            }
            if (fan) {
                // Pre-warm fan sound (103 KB) - force audio system processing
                oslGetSoundChannel(fan);
                memory::trackAudioAmbience(105934); // fan.wav size
            }
            
            #ifdef DEBUG_MEMORY
            printf("Pre-cached ambience audio: %.2f MB\n", 1429084 / (1024.0f * 1024.0f));
            #endif
        }
        
        void preloadEndingMusic() {
            // ULTRA-AGGRESSIVE: Pre-cache ending music for zero-latency
            // This eliminates loading stutter during the ending sequence
            // Critical for maintaining the emotional impact of the ending!
            
            if (music::n_ending::endingSong) {
                // Pre-warm ending music (1.30 MB) - force audio system processing
                oslGetSoundChannel(music::n_ending::endingSong);
                memory::trackAudioMusic(1359304); // music.wav size
            }
            
            #ifdef DEBUG_MEMORY
            printf("Pre-cached ending music: %.2f MB\n", 
                   1359304 / (1024.0f * 1024.0f));
            #endif
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
    
    void loadAndPreloadPhoneCall() {
        // OPTIMIZED: Load and pre-cache phone call in one efficient step
        // This eliminates the need for separate load + preload calls
        int nightIndex = save::whichNight - 1;
        if (nightIndex >= 0 && nightIndex < 5) {
            // Load the phone call file
            safeDelete(phoneCalls[nightIndex]);
            std::string filePath = "romfs/ambience/office/call/call" + toString(nightIndex + 1) + ".wav";
            phoneCalls[nightIndex] = oslLoadSoundFileWAV(filePath.c_str(), OSL_FMT_STREAM);
            stopped = false;
            
            // Immediately pre-cache for zero-latency playback
            if (phoneCalls[nightIndex]) {
                oslGetSoundChannel(phoneCalls[nightIndex]);
                
                // Track memory usage based on night
                size_t phoneCallSize = 0;
                switch (nightIndex) {
                    case 0: phoneCallSize = 4494284; break; // call1.wav - 4.3 MB
                    case 1: phoneCallSize = 2214644; break; // call2.wav - 2.1 MB  
                    case 2: phoneCallSize = 1602498; break; // call3.wav - 1.5 MB
                    case 3: phoneCallSize = 1347648; break; // call4.wav - 1.3 MB
                    case 4: phoneCallSize = 796570;  break; // call5.wav - 0.8 MB
                }
                
                memory::trackAudioMusic(phoneCallSize);
                
                #ifdef DEBUG_MEMORY
                printf("Loaded and pre-cached phone call %d: %zu bytes (%.2f MB)\n", 
                       nightIndex + 1, phoneCallSize, phoneCallSize / (1024.0f * 1024.0f));
                #endif
            }
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
        // OSL_SOUND* kitchen = nullptr; // DISABLED: Not currently used
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
            // safeDelete(kitchen); // DISABLED
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
            // kitchen   = oslLoadSoundFileWAV("romfs/sfx/office/kitchen.wav", OSL_FMT_STREAM); // DISABLED
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
            // if (kitchen)   oslPauseSound(kitchen, 1); // DISABLED
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
            // safeDelete(kitchen); // DISABLED
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
        // void playKitchen()  {
        //     // DISABLED: Not currently used
        //     // if (kitchen && oslGetSoundChannel(kitchen) == -1) oslPlaySound(kitchen, 6); 
        // }
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

        void preloadCriticalAudio() {
            // ULTRA-AGGRESSIVE PRE-CACHING: Maximum performance mode!
            // Pre-cache ALL frequently used audio for zero-latency playback
            // This eliminates ANY potential loading stutter during gameplay
            
            size_t preCachedBytes = 0;
            
            // CORE AUDIO (All nights) - Essential for smooth gameplay
            if (move) {
                oslGetSoundChannel(move); // Pre-warm audio system
                preCachedBytes += 8288; // move.wav - AI movement (updated: shortened file)
            }
            if (scare) {
                oslGetSoundChannel(scare); // Pre-warm audio system
                preCachedBytes += 150788; // scare.wav - Jumpscare sound
            }
            if (buzz) {
                oslGetSoundChannel(buzz); // Pre-warm audio system
                preCachedBytes += 95390; // buzz.wav - Door buzz
            }
            if (door) {
                oslGetSoundChannel(door); // Pre-warm audio system
                preCachedBytes += 24974; // door.wav - Door close
            }
            if (laugh) {
                oslGetSoundChannel(laugh); // Pre-warm audio system
                preCachedBytes += 230998; // laugh.wav - Animatronic laugh
            }
            if (walk) {
                oslGetSoundChannel(walk); // Pre-warm audio system
                preCachedBytes += 172582; // walk.wav - Animatronic movement
            }
            // if (kitchen) {
            //     oslGetSoundChannel(kitchen); // Pre-warm audio system
            //     preCachedBytes += 168970; // kitchen.wav - Kitchen ambience
            // } // DISABLED
            if (camera[0]) {
                oslGetSoundChannel(camera[0]); // Pre-warm audio system
                preCachedBytes += 111610; // openCam.wav - Camera open
            }
            if (camera[1]) {
                oslGetSoundChannel(camera[1]); // Pre-warm audio system
                preCachedBytes += 17932; // closeCam.wav - Camera close
            }
            if (switchCam) {
                oslGetSoundChannel(switchCam); // Pre-warm audio system
                preCachedBytes += 2382; // switch.wav - Camera switch
            }
            
            // JUMPScare AUDIO - Critical for maximum tension!
            // These are the most important sounds for the horror experience
            if (sfx::jumpscare::jumpscare) {
                oslGetSoundChannel(sfx::jumpscare::jumpscare); // Pre-warm audio system
                preCachedBytes += 115360; // jumpscare.wav - Main jumpscare sound
            }
            if (sfx::jumpscare::jumpscare2) {
                oslGetSoundChannel(sfx::jumpscare::jumpscare2); // Pre-warm audio system
                preCachedBytes += 349522; // jumpscare2.wav - Alternative jumpscare
            }
            if (sfx::jumpscare::dead) {
                oslGetSoundChannel(sfx::jumpscare::dead); // Pre-warm audio system
                preCachedBytes += 230388; // dead.wav - Death sound
            }
            
            // FOXY AUDIO (Nights 2+) - High-tension moments
            if (save::whichNight > 1) {
                if (run) {
                    oslGetSoundChannel(run); // Pre-warm audio system
                    preCachedBytes += 56902; // run.wav - Foxy attack warning
                }
                if (knock) {
                    oslGetSoundChannel(knock); // Pre-warm audio system
                    preCachedBytes += 129526; // knock.wav - Foxy blocked
                }
            }
            
            // Track memory usage for optimization
            memory::trackAudioPreCached(preCachedBytes);
            
            // Debug: Print memory usage (remove in release)
          // #ifdef DEBUG_MEMORY
            // printf("ULTRA-AGGRESSIVE: Pre-cached %zu bytes (%.2f MB) of audio\n", 
                   // preCachedBytes, preCachedBytes / (1024.0f * 1024.0f));
            // printf("Memory usage: %.1f%% of 60MB budget\n", 
                   // (float)preCachedBytes / (60 * 1024 * 1024) * 100.0f);
            // #endif
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
        
        void preloadSixAmAudio() {
            // ULTRA-AGGRESSIVE: Pre-cache Six AM audio for zero-latency
            // This eliminates loading stutter during the Six AM sequence
            // Critical for maintaining the relief and satisfaction of surviving the night!
            
            if (chimes) {
                // Pre-warm Six AM chimes (0.59 MB) - force audio system processing
                oslGetSoundChannel(chimes);
                memory::trackAudioMusic(617080); // chimes.wav size
            }
            
            #ifdef DEBUG_MEMORY
            printf("Pre-cached Six AM audio: %.2f MB\n", 
                   617080 / (1024.0f * 1024.0f));
            #endif
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
}