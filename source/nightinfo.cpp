#include "included/nightinfo.hpp"

namespace nightinfo {

    int countdown = 400;
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
        countdown = 400;
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
        // Note: sprite::UI::office::loadCams() still loads several PNGs at once.
        // If you see a hitch on that step, we can split it per-camera later.
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

                case 8: sprite::UI::office::loadCams();     loadStep++; break;
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
                case 14: call::loadPhoneCalls();           loadStep++; break;

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
            if (countdown <= 0 && officeObjectsLoaded && !deactivated) {
                // Schedule transition; real work happens in postFrame()
                requestOffice = true;
                // Optional: prevent re-scheduling if called again
                deactivated = true;
            } else {
                countdown -= 1;
            }
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