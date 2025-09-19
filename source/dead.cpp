#include "included/dead.hpp"

// External reference to the reset flag in main.cpp
extern bool reseted;

namespace dead {

    int waitFrames = 600;

    // Deferred transition (safer on PSP)
    static bool requestMenu = false;
    static bool transitioning = false;

    void reset() {
        waitFrames = 600;
        requestMenu = false;
        transitioning = false;
    }

    namespace n_static {
        int whichFrameStatic = 0;
        int waitFrames2 = 5;

        static inline int clamp(int v, int lo, int hi) {
            return (v < lo) ? lo : (v > hi ? hi : v);
        }

        void renderStatic() {
            int idx = clamp(whichFrameStatic, 0, 3);
            auto* tex = image::global::n_static::staticFrames[idx];
            if (tex) {
                drawSpriteAlpha(0, 0, 480, 272, tex, 0, 0, 0);
            }
        }
        void animateStatic() {
            if (waitFrames2 <= 0) {
                whichFrameStatic += 1;
                if (whichFrameStatic > 3) whichFrameStatic = 0;
                waitFrames2 = 5;
            } else {
                waitFrames2 -= 1;
            }
        }
    }

    namespace wait {
        void waitForFrames() {
            if (requestMenu) return; // already scheduled
            if (waitFrames <= 0) {
                initMenu();          // schedule transition
                waitFrames = 600;    // reset for next time
            } else {
                waitFrames -= 1;
            }
        }

        // Now schedules; actual work happens in dead::postFrame()
        void initMenu() {
            requestMenu = true;
        }
    }

    // Call this after swap in your main loop (like ending/powerout):
    // - sceGuFinish();
    // - sceGuSync(...);
    // - sceDisplayWaitVblankStartCB();
    // - sceGuSwapBuffers();
    // - dead::postFrame();
    void postFrame() {
        if (!requestMenu || transitioning) return;
        transitioning = true;

        // Leave dead state first
        state::isDead = false;

        // Stop and free related audio (safe even if already done)
        sfx::jumpscare::unloadDeadSound();

        // Free office UI resources (idempotent unloads recommended)
        sprite::UI::office::unloadCamUi();
        sprite::UI::office::unloadPowerInfo();
        sprite::office::unloadDoors();
        sprite::office::unloadButtons();
        
        // CRITICAL: Unload camera system to ensure clean state for next game
        // This prevents camera deadlock by ensuring cameras are properly reinitialized
        sprite::UI::office::unloadCams();
        
        // CRITICAL: Unload pre-cached assets to prevent interference with camera system
        text::preload::unloadCameraAssets();
        text::preload::unloadJumpscareAssets();
        sfx::preload::unloadCriticalAudio();

        // Load menu assets
        image::menu::loadMenuBackground();
        image::menu::loadLogo();
        image::menu::loadCopyright();
        image::menu::loadTextAndCursor();
        sprite::menu::loadStar();

        music::menu::loadMenuMusic();
        music::menu::playMenuMusic();

        state::isMenu = true;

        // CRITICAL: Reset all game systems when returning to menu
        // This ensures clean state for the next game session
        // Note: We don't call resetMain() here because it would cause double-reset
        // The resetForDeath() already handled the animatronic reset
        // But we need to ensure the reset flag is set for the next game
        reseted = false; // This will trigger resetMain() when starting a new game

        // Done
        requestMenu = false;
        transitioning = false;
    }
}