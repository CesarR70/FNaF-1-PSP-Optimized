#include "included/ending.hpp"

namespace ending {

    int waitTime = 3660;

    // Transition flags for deferred menu switch
    static bool requestMenu = false;     // set when timer expires or initMenu() is called
    static bool transitioning = false;   // prevents double-run within the same frame

    void reset() {
        waitTime = 3660;
        requestMenu = false;
        transitioning = false;
    }

    namespace render {
        void renderEnding() {
            // Guard in case the texture has been unloaded/nulled
            auto* tex = image::n_ending::ending;
            if (tex) {
                drawSpriteAlpha(0, 0, 480, 272, tex, 0, 0, 0);
            } else {
                // Optionally draw a solid black fallback if needed
                // drawFillRect(0,0,480,272,0xFF000000);
            }
        }
    }

    namespace wait {
        void waitForFrames() {
            if (requestMenu) return; // already scheduled
            if (--waitTime <= 0) {
                requestMenu = true;  // defer actual work to postFrame()
            }
        }

        // Keep this for external callers; now behaves as a scheduler
        void initMenu() {
            requestMenu = true;
        }
    }

    // Call this ONCE per frame from your main loop AFTER:
    // - sceGuFinish();
    // - sceGuSync(GU_SYNC_FINISH, GU_SYNC_WHAT_DONE);
    // - sceDisplayWaitVblankStart();
    // - sceGuSwapBuffers();
    void postFrame() {
        if (!requestMenu || transitioning) return;
        transitioning = true;

        // Now it's safe to unload and change scenes.

        // Stop/unload ending music safely (idempotent expected)
        if (!music::n_ending::stopped) {
            // music::n_ending::stop(); // if available
            music::n_ending::unloadEndingSong();
            music::n_ending::stopped = true;
        } else {
            music::n_ending::unloadEndingSong(); // ok if unload is idempotent
        }

        // Free ending texture (ensure unloadEnding() nulls the pointer).
        image::n_ending::unloadEnding();

        // Switch state away from ending before we load menu assets.
        state::isEnding = false;

        // CRITICAL: Unload pre-cached assets to prevent interference with menu system
        text::preload::unloadCameraAssets();
        text::preload::unloadJumpscareAssets();
        sfx::preload::unloadCriticalAudio();

        // Load menu assets
        image::menu::loadMenuBackground();
        image::menu::loadLogo();
        image::menu::loadCopyright();
        image::menu::loadTextAndCursor();
        sprite::menu::loadStar();

        menu::menuCursor::moveCursor();
        save::readData();

        music::menu::loadMenuMusic();
        music::menu::playMenuMusic();

        state::isMenu = true;

        // Reset for next time
        waitTime = 3660;
        requestMenu = false;
        transitioning = false;
    }
}