#include "included/camera.hpp"

namespace camera {

static constexpr int kCamCount = 11;
static constexpr int kFlipFrameCount = 4;
static constexpr int kStaticFrames = 4;

static const int kButtonPosX[kCamCount] = { 353,348,333,352,352,323,395,395,307,435,440 };
static const int kButtonPosY[kCamCount] = { 115,140,172,215,232,210,215,232,150,202,150 };

static const int kReticlePosX[kCamCount] = { 353,348,333,352,352,323,395,395,307,435,440 };
static const int kReticlePosY[kCamCount] = { 115,140,172,215,232,210,215,232,150,202,150 };

int whichCamera;
bool isUsing = false;

bool opening = false;
bool closing = false;

int whichFrame;
int waitFrames = 1;
int delay = 3;

int reticleX = 353;
int reticleY = 115;

std::string buttonState = "up";

// Note: Camera switching throttling removed - relying on existing kCamReloadBudget system
// in image2.cpp plus double-check safety in render functions

static inline int clamp(int v, int lo, int hi) {
    return (v < lo) ? lo : (v > hi ? hi : v);
}

void reset() {
    whichCamera = 0;

    isUsing = false;
    animatronic::usingCams = false;
    opening = false;
    closing = false;

    whichFrame = 0;
    waitFrames = 1;
    delay = 3;

    reticleX = 353;
    reticleY = 115;

    buttonState = "up";
}

namespace render {

    void renderCamFlip() {
        if (!(closing || opening)) return;

        int idx = clamp(whichFrame, 0, kFlipFrameCount - 1);
        auto* tex = sprite::UI::office::camFlip[idx];
        if (tex) {
            drawSpriteAlpha(0, 0, 480, 272, tex, 0, 0, 0);
        }
    }

    void renderCamera() {
        // Extra guards so we don't try to draw while cams are being (re)loaded
        if (!isUsing || closing) return;
        if (!sprite::UI::office::loaded || !animatronic::reloaded) return;
        
        // CRITICAL: Additional safety check to prevent crashes during high activity
        // Don't render if animatronics are moving (reloading sprites) - EXCEPT during Foxy attacks
        if (animatronic::isMoving && !animatronic::foxy::atDoor) return;

        int cam = clamp(whichCamera, 0, kCamCount - 1);
        
        // Normal camera rendering - let the AI system handle everything
        auto* tex = sprite::UI::office::cams[cam];
        
        // CRITICAL: Double-check the texture is still valid after array access
        // This prevents crashes during rapid camera switching + animatronic movement
        if (tex && tex->data) {
            drawSpriteAlpha(0, 0, 480, 272, tex, 0, 0, 0);
        }
    }

    void renderUi() {
        if (!isUsing || closing) return;

        if (sprite::UI::office::camBorder) {
            drawSpriteAlpha(0, 0, 480, 272, sprite::UI::office::camBorder, 0, 0, 0);
        }
        if (sprite::UI::office::camMap) {
            drawSpriteAlpha(0, 0, 160, 160, sprite::UI::office::camMap, 310, 108, 0);
        }
        if (sprite::UI::office::recording) {
            drawSpriteAlpha(0, 0, 20, 20, sprite::UI::office::recording, 20, 20, 0);
        }

        int cam = clamp(whichCamera, 0, kCamCount - 1);
        auto* name = sprite::UI::office::camNames[cam];
        
        // CRITICAL: Double-check the texture is still valid after array access
        if (name && name->data) {
            drawSpriteAlpha(0, 0, 121, 13, name, 310, 95, 0);
        }

        for (int i = 0; i < kCamCount; ++i) {
            auto* tex = sprite::UI::office::camButtons[i];
            // CRITICAL: Double-check the texture is still valid after array access
            if (tex && tex->data) {
                drawSpriteAlpha(0, 0, 20, 15, tex, kButtonPosX[i], kButtonPosY[i], 0);
            }
        }

        if (sprite::UI::office::reticle) {
            drawSpriteAlpha(0, 0, 20, 15, sprite::UI::office::reticle,
                            kReticlePosX[cam], kReticlePosY[cam], 0);
        }
    }
}

namespace n_static {
    int whichFrameStatic = 0;
    int waitFrames2 = 5;

    void renderStatic() {
        if (!isUsing || closing) return;
        int idx = clamp(whichFrameStatic, 0, kStaticFrames - 1);
        auto* tex = image::global::n_static::staticFrames[idx];
        if (tex) {
            drawSpriteAlpha(0, 0, 480, 272, tex, 0, 0, 0);
        }
    }

    void animateStatic() {
        if (waitFrames2 <= 0) {
            whichFrameStatic++;
            if (whichFrameStatic >= kStaticFrames) whichFrameStatic = 0;
            waitFrames2 = 3;
        } else {
            waitFrames2 -= 1;
        }
    }
}

namespace animation {

    void camera() {
        if (!opening && !isUsing) {
            opening = true;
            sfx::office::playCamOpen();
        }
        else if (!closing && isUsing) {
            closing = true;
            sfx::office::playCamClose();
        }
    }

    void openCams() {
        if (waitFrames <= 0) {
            if (whichFrame < (kFlipFrameCount - 1)) {
                whichFrame += 1;
                waitFrames = 1;
            } else {
                // End of flip: only enter cam view once textures are ready
                if (sprite::UI::office::loaded && animatronic::reloaded) {
                    opening = false;
                    isUsing = true;
                    animatronic::usingCams = true;
                } else {
                    // Hold the last flip frame until assets are ready
                    waitFrames = 1;
                }
            }
        } else {
            waitFrames -= 1;
        }
    }

    void closeCams() {
        if (delay <= 0) {
            if (waitFrames <= 0) {
                if (whichFrame > 0) {
                    whichFrame -= 1;
                    waitFrames = 1;
                } else {
                    closing = false;
                    isUsing = false;
                    animatronic::usingCams = false;
                    delay = 2;
                }
            } else {
                waitFrames -= 1;
            }
        } else {
            delay -= 1;
        }
    }
}

namespace system {

    void setReticle() {
        int cam = clamp(whichCamera, 0, kCamCount - 1);
        reticleX = kReticlePosX[cam];
        reticleY = kReticlePosY[cam];
    }

    void left() {
        if (!isUsing) return;
        
        // PERFORMANCE: Cache previous camera to avoid unnecessary operations
        static int prevCamera = -1;
        if (whichCamera == prevCamera) return; // Skip if already at target
        
        switch (whichCamera) {
            case 1: case 2: whichCamera = 8;  break;
            case 3: case 4: whichCamera = 5;  break;
            case 6: case 7: whichCamera = 3;  break;
            case 9:         whichCamera = 6;  break;
            case 10:        whichCamera = 1;  break;
            default: break;
        }
        whichCamera = clamp(whichCamera, 0, kCamCount - 1);
        prevCamera = whichCamera;
        sfx::office::playSwitch();
        setReticle();
    }

    void right() {
        if (!isUsing) return;
        
        // PERFORMANCE: Cache previous camera to avoid unnecessary operations
        static int prevCamera = -1;
        if (whichCamera == prevCamera) return; // Skip if already at target
        
        switch (whichCamera) {
            case 1: case 2: whichCamera = 10; break;
            case 3: case 4: whichCamera = 6;  break;
            case 6: case 7: whichCamera = 9;  break;
            case 5:         whichCamera = 3;  break;
            case 8:         whichCamera = 1;  break;
            default: break;
        }
        whichCamera = clamp(whichCamera, 0, kCamCount - 1);
        prevCamera = whichCamera;
        sfx::office::playSwitch();
        setReticle();
    }

    void up() {
        if (!isUsing) return;
        
        switch (whichCamera) {
            case 1: whichCamera = 0;  break;
            case 2: whichCamera = 1;  break;
            case 3: whichCamera = 2;  break;
            case 4: whichCamera = 3;  break;
            case 7: whichCamera = 6;  break;
            case 9: whichCamera = 10; break;
            default: break;
        }
        whichCamera = clamp(whichCamera, 0, kCamCount - 1);
        sfx::office::playSwitch();
        setReticle();
    }

    void down() {
        if (!isUsing) return;
        
        switch (whichCamera) {
            case 0: whichCamera = 1; break;
            case 1: whichCamera = 2; break;
            case 2: whichCamera = 3; break;
            case 3: whichCamera = 4; break;
            case 6: whichCamera = 7; break;
            case 10:whichCamera = 9; break;
            default: break;
        }
        whichCamera = clamp(whichCamera, 0, kCamCount - 1);
        sfx::office::playSwitch();
        setReticle();
    }
  }   
}