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

            // Prepare jumpscare
            sfx::jumpscare::loadJumpscareSound();
            sprite::n_jumpscare::whichJumpscare = 1;
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