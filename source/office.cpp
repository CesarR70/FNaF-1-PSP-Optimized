#include "included/office.hpp"

namespace office {

    int wichOfficeFrame = 0; // 3 = bonnie, 4 = chica at door
    int xPos[6];
    int speed = 1;
    int speedMultiplier = 5;
    std::string dir = "none";

    volatile int usageCountdown = 10; // volatile for thread safety
    volatile bool leftEdge = false; // volatile for thread safety
    volatile bool rightEdge = true; // volatile for thread safety

    volatile bool leftOn = false; // volatile for thread safety
    volatile bool rightOn = false; // volatile for thread safety
    volatile bool leftClosed = false; // volatile for thread safety
    volatile bool rightClosed = false; // volatile for thread safety

    volatile bool closingLeft = false; // volatile for thread safety
    volatile bool openingLeft = false; // volatile for thread safety
    volatile bool closingRight = false; // volatile for thread safety
    volatile bool openingRight = false; // volatile for thread safety

    std::string state = "none";
    std::string buttonState = "up";
    std::string doorButtonState = "up";

    int leftButtonFrame = 0;
    int rightButtonFrame = 0;

    // Original shared timer (kept for compatibility, no longer used)
    int doorAnimTime = 1;

    // New: independent door timers to allow both doors to animate smoothly
    int leftDoorTimer = 0;
    int rightDoorTimer = 0;

    int leftDoorFrame = 0;
    int rightDoorFrame = 0;

    // New: prevent scare spam while holding light
    bool scareLeftPlayed = false;
    bool scareRightPlayed = false;

    static inline char firstCharLower(const std::string& s, char def = 'n') {
        if (s.empty()) return def;
        char c = s[0];
        if (c >= 'A' && c <= 'Z') c = char(c + ('a' - 'A'));
        return c;
    }

    void reset() {
        wichOfficeFrame = 0;

        main::setX();

        leftOn = false;
        rightOn = false;
        leftClosed = false;
        rightClosed = false;
        closingLeft = false;
        openingLeft = false;
        closingRight = false;
        openingRight = false;

        state = "none";
        buttonState = "up";
        doorButtonState = "up";

        leftButtonFrame = 0;
        rightButtonFrame = 0;

        doorAnimTime = 1;
        leftDoorTimer = 0;
        rightDoorTimer = 0;
        leftDoorFrame = 0;
        rightDoorFrame = 0;

        scareLeftPlayed = false;
        scareRightPlayed = false;

        leftEdge = false;
        rightEdge = true;
    }

    namespace render {
        void renderOffice() {
            drawSpriteAlpha(0, 0, 480, 272, officeImage::office1Sprites[wichOfficeFrame], xPos[0], 0, 0);
            drawSpriteAlpha(0, 0, 124, 272, officeImage::office2Sprites[wichOfficeFrame], xPos[1], 0, 0);
        }
        void renderButtons() {
            drawSpriteAlpha(0, 0, 50, 90, sprite::office::buttonsLeft[leftButtonFrame], xPos[2], 105, 0);
            drawSpriteAlpha(0, 0, 50, 90, sprite::office::buttonsRight[rightButtonFrame], xPos[3], 100, 0);
        }
        void renderDoors() {
            drawSpriteAlpha(0, 0, 84, 272, sprite::office::doorLeft[leftDoorFrame], xPos[4], 0, 0);
            drawSpriteAlpha(0, 0, 84, 272, sprite::office::doorRight[rightDoorFrame], xPos[5], 0, 0);
        }
    }

    namespace buttons {
        void setButtonFrame() {
            leftButtonFrame = (leftOn ? 2 : 0) + (leftClosed ? 1 : 0);
            rightButtonFrame = (rightOn ? 2 : 0) + (rightClosed ? 1 : 0);
        }
    }

    namespace main {
        void setX() {
            xPos[0] = 0;
            xPos[1] = 480;
            xPos[2] = -5;
            xPos[3] = 550;
            xPos[4] = 30;
            xPos[5] = 480;
        }

        void moveOffice() {
            // Use first char checks to avoid heavy string comparisons
            const char d = firstCharLower(dir, 'n'); // 'l' 'r' 'n'
            const int step = speed * speedMultiplier;

            if (d == 'l' && xPos[1] > 364) {
                for (int i = 0; i < 6; ++i) xPos[i] -= step;
            } else if (d == 'r' && xPos[0] < 0) {
                for (int i = 0; i < 6; ++i) xPos[i] += step;
            }

            // Snap/clamp to edges so we never "miss" the exact value
            int shift = 0;
            if (xPos[0] < -120) {
                shift = -120 - xPos[0];
            } else if (xPos[0] > 0) {
                shift = 0 - xPos[0];
            }
            if (shift) {
                for (int i = 0; i < 6; ++i) xPos[i] += shift;
            }

            // Update edges based on clamped position
            if (xPos[0] == -120) {
                leftEdge = true;
                rightEdge = false;
            } else if (xPos[0] == 0) {
                rightEdge = true;
                leftEdge = false;
            } else {
                leftEdge = false;
                rightEdge = false;
            }
        }
    }

    namespace lights {
        void lights() {
            const char btn = firstCharLower(buttonState, 'u'); // 'd' or 'u'

            if (btn == 'd') {
                // Right side (triggered when at left edge)
                if (leftEdge) {
                    wichOfficeFrame = (animatronic::chika::position == 6) ? 4 : 1;
                    if (!rightOn) {
                        rightOn = true;
                        sfx::office::playLightOn(); // transition ON
                    }
                    // Scare once while light is held, if applicable
                    if (animatronic::chika::position == 6 && !rightClosed) {
                        if (!scareRightPlayed) {
                            sfx::office::playScare();
                            scareRightPlayed = true;
                        }
                    } else {
                        scareRightPlayed = false;
                    }
                }

                // Left side (triggered when at right edge)
                if (rightEdge) {
                    wichOfficeFrame = (animatronic::bonnie::position == 6) ? 3 : 2;
                    if (!leftOn) {
                        leftOn = true;
                        sfx::office::playLightOn(); // transition ON
                    }
                    if (animatronic::bonnie::position == 6 && !leftClosed) {
                        if (!scareLeftPlayed) {
                            sfx::office::playScare();
                            scareLeftPlayed = true;
                        }
                    } else {
                        scareLeftPlayed = false;
                    }
                }
            } else {
                // Button up: lights off, reset frame and scare guards
                if (leftOn) {
                    leftOn = false;
                    sfx::office::playLightOff();
                }
                if (rightOn) {
                    rightOn = false;
                    sfx::office::playLightOff();
                }
                wichOfficeFrame = 0;
                scareLeftPlayed = false;
                scareRightPlayed = false;
            }
        }
    }

    namespace doors {

        void doors() {
            // Toggle left door when at right edge
            if (rightEdge) {
                if (!closingLeft && !leftClosed) {
                    closingLeft = true;
                    openingLeft = false;
                    leftDoorTimer = 1;
                    sfx::office::playDoor();
                } else if (!openingLeft && leftClosed) {
                    openingLeft = true;
                    closingLeft = false;
                    leftDoorTimer = 1;
                    sfx::office::playDoor();
                }
            }
            // Toggle right door when at left edge
            else if (leftEdge) {
                if (!closingRight && !rightClosed) {
                    closingRight = true;
                    openingRight = false;
                    rightDoorTimer = 1;
                    sfx::office::playDoor();
                } else if (!openingRight && rightClosed) {
                    openingRight = true;
                    closingRight = false;
                    rightDoorTimer = 1;
                    sfx::office::playDoor();
                }
            }
        }

        void closeLeft() {
            if (--leftDoorTimer <= 0) {
                if (leftDoorFrame < 6) {
                    ++leftDoorFrame;
                    leftDoorTimer = 1;
                } else {
                    closingLeft = false;
                    leftClosed = true;
                    animatronic::leftClosed = true;
                }
            }
        }
        void openLeft() {
            if (--leftDoorTimer <= 0) {
                if (leftDoorFrame > 0) {
                    --leftDoorFrame;
                    leftDoorTimer = 1;
                } else {
                    openingLeft = false;
                    leftClosed = false;
                    animatronic::leftClosed = false;
                }
            }
        }

        void closeRight() {
            if (--rightDoorTimer <= 0) {
                if (rightDoorFrame < 6) {
                    ++rightDoorFrame;
                    rightDoorTimer = 1;
                } else {
                    closingRight = false;
                    rightClosed = true;
                    animatronic::rightClosed = true;
                }
            }
        }
        void openRight() {
            if (--rightDoorTimer <= 0) {
                if (rightDoorFrame > 0) {
                    --rightDoorFrame;
                    rightDoorTimer = 1;
                } else {
                    openingRight = false;
                    rightClosed = false;
                    animatronic::rightClosed = false;
                }
            }
        }
    }
}