#include "included/jumpscare.hpp"

namespace jumpscare {

// Forward declare so reset() can touch it before the variable is defined
namespace load { extern int waitBeforeLoading; }

// Tunables
constexpr int kFrameCount          = 9;   // frames 0..8
constexpr int kDefaultFrameDelay   = 3;   // ticks per frame (was 2)
constexpr int kDefaultHoldFrames   = 15;  // hold on last frame (~0.25s at 60fps)

int whichFrame    = 0;
int frameDelay    = kDefaultFrameDelay;
int frameCounter  = kDefaultFrameDelay;

int holdFrames    = kDefaultHoldFrames;
int holdCounter   = kDefaultHoldFrames;

bool soundPlayed  = false;
bool transitioning = false;

void reset() {
    whichFrame   = 0;
    frameDelay   = kDefaultFrameDelay;
    frameCounter = frameDelay;

    holdFrames   = kDefaultHoldFrames;
    holdCounter  = holdFrames;

    soundPlayed  = false;
    transitioning = false;

    // Reset the deferred loader’s counter
    load::waitBeforeLoading = 2;
}

namespace render {
    void renderJumpscare() {
        if (!sprite::n_jumpscare::loaded) return;

        int idx = whichFrame;
        if (idx < 0) idx = 0;
        if (idx >= kFrameCount) idx = kFrameCount - 1;

        // CRITICAL: Add null check to prevent crashes during high activity
        // This prevents accessing freed or uninitialized sprite data
        if (sprite::n_jumpscare::jumpscareAnim[idx]) {
            // Additional safety: verify the sprite data is valid before rendering
            Image* sprite = sprite::n_jumpscare::jumpscareAnim[idx];
            if (sprite && sprite->data) {
                drawSpriteAlpha(
                    0, 0, 480, 272,
                    sprite,
                    0, 0, 0
                );
            } else {
                DEBUG_PRINTF("⚠️  WARNING: Invalid jumpscare sprite data at index %d\n", idx);
            }
        }
    }
}

namespace animate {
    void initDeathScreen();

    void animateJumpscare() {
        // Don’t advance until assets are ready
        if (!sprite::n_jumpscare::loaded) return;

        // Play scream once, early in the sequence
        if (!soundPlayed && whichFrame >= 1) {
            sfx::jumpscare::playJumpscareSound();
            soundPlayed = true;
        }

        // Advance frames up to the last frame
        if (whichFrame < (kFrameCount - 1)) {
            if (--frameCounter <= 0) {
                ++whichFrame;
                frameCounter = frameDelay;
            }
            return;
        }

        // On last frame: hold for a while
        if (holdCounter > 0) {
            --holdCounter;
            return;
        }

        // Transition once
        if (!transitioning) {
            transitioning = true;
            initDeathScreen();
        }
    }

    void initDeathScreen() {
        state::isJumpscare = false;

        sprite::n_jumpscare::unloadJumpscare();
        sfx::jumpscare::unloadJumpscareSound();

        sfx::jumpscare::loadDeadSound();
        sfx::jumpscare::playDeadSound();

        state::isDead = true;
    }
}

namespace load {
    int waitBeforeLoading = 2; // in frames

    void loadWithDelay() {
        if (!sprite::n_jumpscare::loaded) {
            if (--waitBeforeLoading <= 0) {
                sprite::n_jumpscare::loadJumpscare();
                waitBeforeLoading = 2;
            }
        }
    }
}

} // namespace jumpscare