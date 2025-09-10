#include "included/sixam.hpp"

namespace sixam {

    int whichNumber = 5;
    int delayChange = 150;
    int delayReload = 500;

    // Deferred transition
    enum class Pending { None, Nightinfo, Ending, Menu };
    static Pending pending = Pending::None;
    static bool transitioning = false;

    void reset() {
        whichNumber = 5;
        delayChange = 150;
        delayReload = 500;

        pending = Pending::None;
        transitioning = false;
    }

    namespace saveIt {
        void saveTheData() {
            save::saveData();
            save::file();
        }
    }

    namespace render {
        void renderSixAm() {
            // Clamp index and guard pointers
            int idx = whichNumber;
            if (idx < 0) idx = 0;
            if (idx > 9) idx = 9; // nightNumbersPixel typically 0..9

            auto* digit = text::global::nightNumbersPixel[idx];
            if (digit) {
                drawSpriteAlpha(0, 0, 20, 20, digit, 215, 123, 0);
            }

            if (sprite::UI::office::AM) {
                drawSpriteAlpha(0, 0, 27, 20, sprite::UI::office::AM, 240, 126, 0);
            }
        }
    }

    namespace animate {
        void wait() {
            if (delayChange <= 0) {
                whichNumber = 6; // show 6
            } else {
                delayChange -= 1;
            }
        }
    }

    namespace next {
        // Keep clearRest here; we’ll call it from postFrame.
        void clearRest() {
            officeImage::unloadOffice2Sprites();

            sprite::UI::office::unloadPowerInfo();
            sprite::UI::office::unloadTimeInfo();
            sprite::office::unloadDoors();
            sprite::office::unloadButtons();
        }

        // These now just schedule transitions; actual work happens in sixam::postFrame()
        void initNightinfo() {
            pending = Pending::Nightinfo;
        }
        void initEnding() {
            pending = Pending::Ending;
        }
        void initMenu() {
            pending = Pending::Menu;
        }

        void wait() {
            if (delayReload <= 0) {
                if (pending == Pending::None) {
                    switch (save::whichNight) {
                        case 5: case 7:
                            initEnding();
                            break;
                        case 6:
                            initMenu();
                            break;
                        default:
                            initNightinfo();
                            break;
                    }
                }
            } else {
                delayReload -= 1;
            }
        }
    }

    // Call this after swap in your main loop (like ending::postFrame)
    void postFrame() {
        if (pending == Pending::None || transitioning) return;
        transitioning = true;

        // Leave sixam state first
        state::isSixAm = false;

        // Free office assets safely now
        next::clearRest();

        // Perform the scheduled transition
        switch (pending) {
            case Pending::Nightinfo: {
                save::readData();
                sprite::nightinfo::loadNightInfoSprite();
                sfx::sixam::unloadSixAm();
                state::isNightinfo = true;
            } break;

            case Pending::Ending: {
                image::n_ending::loadEnding();
                music::n_ending::loadEndingSong();
                music::n_ending::playEndingSong();
                state::isEnding = true;
            } break;

            case Pending::Menu: {
                save::readData();

                image::menu::loadMenuBackground();
                image::menu::loadLogo();
                sprite::menu::loadStar();
                image::menu::loadCopyright();
                text::global::loadNightText();

                menu::menuCursor::moveCursor();

                music::menu::loadMenuMusic();
                music::menu::playMenuMusic();

                state::isMenu = true;
            } break;

            case Pending::None:
            default:
                break;
        }

        pending = Pending::None;
        transitioning = false;
    }
}