#include "included/nightinfo.hpp"

namespace nightinfo {

    // OPTIMIZATION: Remove countdown timer for better loading reliability
    // Transition now purely based on loading completion, not arbitrary time
    bool officeObjectsLoaded = false;
    bool isLoading = false;
    bool deactivated = false;
    bool loadedMain = false;

    // Incremental loader
    static int  loadStep = 0;
    static int  loadsPerFrame = 2; // tune: do a couple of steps per frame
    // Deferred transition to Office
    static bool requestOffice = false;
    static bool transitioning = false;

    void reset() {
        // OPTIMIZATION: Removed countdown timer - transition based on loading completion only
        officeObjectsLoaded = false;
        isLoading = false;
        deactivated = false;
        loadedMain = false;

        loadStep = 0;
        requestOffice = false;
        transitioning = false;
    }

    namespace render {
        void renderNightinfo() {
            // Info
            if (sprite::nightinfo::info) {
                drawSpriteAlpha(0, 0, 75, 48, sprite::nightinfo::info, 200, 100, 0);
            }
            // Clock
            if (sprite::nightinfo::clock) {
                drawSpriteAlpha(0, 0, 40, 40, sprite::nightinfo::clock, 435, 227, 0);
            }
            // Night number
            int n = save::whichNight;
            if (n < 0) n = 0;
            if (n > 9) n = 9;
            auto* num = text::global::nightNumbersPixel[n];
            if (num) {
                drawSpriteAlpha(0, 0, 20, 20, num, 265, 128, 0);
            }
        }
    }

    namespace next {

        // One tiny task per step to avoid frame spikes.
        // Note: sprite::UI::office::loadAllCams() loads all camera images at once for initial setup.
        // This ensures all cameras are visible from the start of early levels.
        static void tickLoader() {
            switch (loadStep) {
                case 0:
                    if (image::n_newspaper::loaded) {
                        image::n_newspaper::unloadNewsPaper();
                    }
                    loadStep++;
                    break;

                case 1: officeImage::loadOffice1Sprites(); loadStep++; break;
                case 2: officeImage::loadOffice2Sprites(); loadStep++; break;

                case 3: sprite::office::loadButtons();     loadStep++; break;
                case 4: sprite::office::loadDoors();       loadStep++; break;

                case 5: sprite::UI::office::loadCamFlip();  loadStep++; break;
                case 6: sprite::UI::office::loadPowerInfo();loadStep++; break;
                case 7: sprite::UI::office::loadTimeInfo(); loadStep++; break;

                case 8: sprite::UI::office::loadAllCams();  loadStep++; break;
                case 9: sprite::UI::office::loadCamUi();    loadStep++; break;

                case 10:
                    power::update::setDrainTime();
                    office::main::setX();
                    animatronic::setDefault();
                    loadStep++;
                    break;

                case 11: ambience::office::loadAmbience(); loadStep++; break;
                case 12: ambience::office::loadFanSound(); loadStep++; break;
                case 13: sfx::office::loadSfx();           loadStep++; break;
                case 14: call::loadAndPreloadPhoneCall(); loadStep++; break; // Load and pre-cache phone call in one step
                case 15: sfx::office::preloadCriticalAudio(); loadStep++; break; // ULTRA-AGGRESSIVE: Pre-cache ALL audio for max performance
                case 16: ambience::office::preloadAmbience(); loadStep++; break; // Pre-cache ambience for zero-latency
                case 17: sprite::UI::office::preloadCameraAssets(); loadStep++; break; // Pre-cache ALL camera graphics for zero-latency
                case 18: sprite::UI::office::preloadJumpscareAssets(); loadStep++; break; // Pre-cache ALL jumpscare graphics for zero-latency
                case 19: ambience::office::preloadEndingMusic(); loadStep++; break; // Pre-cache ending music for zero-latency
                case 20: sfx::sixam::preloadSixAmAudio(); loadStep++; break; // Pre-cache Six AM audio for zero-latency

                default:
                    officeObjectsLoaded = true;
                    isLoading = false;
                    loadedMain = true;
                    break;
            }
        }

        void preloadOffice() {
            if (officeObjectsLoaded || deactivated) return;

            // Start incremental loading
            if (!isLoading) {
                isLoading = true;
                loadStep = 0;
            }

            // Do a few steps per frame to keep things smooth
            int steps = loadsPerFrame;
            while (isLoading && steps-- > 0) {
                tickLoader();
            }
        }

        void initOffice() {
            // OPTIMIZATION: Transition purely based on loading completion
            // No more arbitrary countdown timer - ensures everything is loaded before proceeding
            if (officeObjectsLoaded && !deactivated) {
                // Schedule transition; real work happens in postFrame()
                requestOffice = true;
                // Optional: prevent re-scheduling if called again
                deactivated = true;
            }
            // Removed countdown decrement - transition happens when ready
        }
    }

    // Call once per frame AFTER buffer swap (like ending/powerout/sixam/dead)
    void postFrame() {
        if (!requestOffice || transitioning) return;
        transitioning = true;

        // Leave nightinfo state
        state::isNightinfo = false;

        // Free nightinfo textures now that the last frame using them has been presented
        sprite::nightinfo::unloadNightInfoSprite();

        // Start ambience/fan/call playback (assets already loaded during preload)
        ambience::office::playAmbience();
        ambience::office::playFanSound();
        call::playPhoneCalls();

        state::isOffice = true;

        requestOffice = false;
        transitioning = false;
    }
}