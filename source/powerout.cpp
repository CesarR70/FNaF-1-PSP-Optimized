#include "included/powerout.hpp"

namespace powerout {
    int whichFrame = 0;
    int frameWaitTime = 15;
    int totalWaitTime = 1020;
    bool belowMain = false;

    // Internal flags for transition and deferred unload
    static bool triggeredJumpscare = false;
    static bool pendingUnload = false;

    void reset() {
        whichFrame = 0;
        frameWaitTime = 15;
        totalWaitTime = 1020;
        belowMain = false;
        triggeredJumpscare = false;
        pendingUnload = false;
    }

    namespace render {
        void renderPowerout() {
            // Guard against null or OOB just in case (assumes at least 2 entries exist)
            auto* tex = image::n_noPower::noPower[whichFrame & 1];
            if (tex) {
                drawSpriteAlpha(0, 0, 480, 272, tex, 0, 0, 0);
            }
        }
    }

    // Call this from your main loop AFTER finish/sync/vblank/swap
    void postFrame() {
        if (pendingUnload) {
            image::n_noPower::unloadNoPower(); // make sure this nulls the pointers
            pendingUnload = false;
        }
    }

    namespace animate {

        static inline void startJumpscareOnce() {
            if (triggeredJumpscare) return;
            triggeredJumpscare = true;

            state::isPowerOut = false;

            // Stop/unload ending audio if still active (idempotent expected)
            if (!music::n_ending::stopped) {
                music::n_ending::unloadEndingSong(); // ideally stop() then unload internally
                music::n_ending::stopped = true;
            }

            // CRITICAL: Clean up office assets to prevent memory issues and loading conflicts
            // This matches cleanup patterns from dead.cpp and sixam.cpp
            
            // Clean up office audio
            sfx::office::unloadSfx();
            ambience::office::unloadAmbience();
            ambience::office::unloadFanSound();
            call::unloadPhoneCalls();

            // Clean up office sprites and UI
            sprite::UI::office::unloadCamUi();
            sprite::UI::office::unloadPowerInfo();
            sprite::UI::office::unloadTimeInfo();
            sprite::office::unloadDoors();
            sprite::office::unloadButtons();
            sprite::UI::office::unloadCams();
            sprite::UI::office::unloadCamFlip();

            // Clean up office background sprites
            officeImage::unloadOffice1Sprites();
            officeImage::unloadOffice2Sprites();

            // Clean up pre-cached assets to prevent memory conflicts
            text::preload::unloadCameraAssets();
            text::preload::unloadJumpscareAssets();
            sfx::preload::unloadCriticalAudio();

            // Prepare jumpscare - load BEFORE setting state to prevent black screen
            sfx::jumpscare::loadJumpscareSound();
            sprite::n_jumpscare::whichJumpscare = 1;
            
            // Load sprites immediately (benefits from pre-caching system)
            // This ensures sprites are ready before state transition
            sprite::n_jumpscare::loadJumpscare();
            
            // Only set state after loading is complete
            state::isJumpscare = true;

            // Defer freeing the powerout textures until the GPU has presented
            pendingUnload = true;
        }

        // Kept for compatibility with any existing calls
        void initJumpscare() {
            startJumpscareOnce();
        }

        void animatePowerOut() {
            if (triggeredJumpscare) return; // already transitioning, don’t touch resources

            totalWaitTime -= 1;

            if (totalWaitTime == 310) {
                belowMain = true;
            }

            if (!belowMain) {
                if (--frameWaitTime <= 0) {
                    whichFrame ^= 1; // toggle 0/1
                    frameWaitTime = 15;
                }
            } else {
                whichFrame = 0;

                if (!music::n_ending::stopped) {
                    music::n_ending::unloadEndingSong(); // ideally stop(); see note above
                    music::n_ending::stopped = true;
                }

                if (totalWaitTime <= 0) {
                    startJumpscareOnce();
                    totalWaitTime = 1020; // harmless; prevents underflow if called again
                }
            }
        }
    }
}