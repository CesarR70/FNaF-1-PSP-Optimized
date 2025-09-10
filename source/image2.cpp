#include "included/image2.hpp"
#include <string>

// Helpers: safe free + array free
static inline void freeImageSafe(Image*& img) {
    if (img) {
        freeImage(img);
        img = nullptr;
    }
}

template <size_t N>
static inline void freeImageArray(Image* (&arr)[N]) {
    for (size_t i = 0; i < N; ++i) {
        if (arr[i]) {
            freeImage(arr[i]);
            arr[i] = nullptr;
        }
    }
}

namespace image {
    namespace menu {
        Image* menuBackground[4] = {nullptr, nullptr, nullptr, nullptr};
        Image* selectionText[4]  = {nullptr, nullptr, nullptr, nullptr};
        Image* selectionCursor   = nullptr;

        Image* logo = nullptr;
        Image* copyright = nullptr;

        void loadMenuBackground() {
            menuBackground[0] = loadPng("romfs/gfx/menu/frame_1.png");
            menuBackground[1] = loadPng("romfs/gfx/menu/frame_2.png");
            menuBackground[2] = loadPng("romfs/gfx/menu/frame_3.png");
            menuBackground[3] = loadPng("romfs/gfx/menu/frame_4.png");
        }
        void unloadMenuBackground() {
            freeImageArray(menuBackground);
        }

        void loadLogo() {
            logo = loadPng("romfs/gfx/menu/logo.png");
        }
        void unloadLogo() {
            freeImageSafe(logo);
        }

        void loadCopyright() {
            copyright = loadPng("romfs/gfx/menu/copyright.png");
        }
        void unloadCopyright() {
            freeImageSafe(copyright);
        }

        void loadTextAndCursor() {
            selectionText[0] = loadPng("romfs/gfx/menu/selection/continue.png");
            selectionText[1] = loadPng("romfs/gfx/menu/selection/newGame.png");
            selectionText[2] = loadPng("romfs/gfx/menu/selection/6thNight.png");
            selectionText[3] = loadPng("romfs/gfx/menu/selection/customNight.png");
            selectionCursor  = loadPng("romfs/gfx/menu/selection/arrow.png");
        }
        void unloadTextAndCursor() {
            freeImageArray(selectionText);
            freeImageSafe(selectionCursor);
        }
    }

    namespace global {
        namespace n_static {
            Image* staticFrames[4] = {nullptr, nullptr, nullptr, nullptr};

            void loadStatic() {
                staticFrames[0] = loadPng("romfs/gfx/menu/static/image1_480x272.png");
                staticFrames[1] = loadPng("romfs/gfx/menu/static/image2_480x272.png");
                staticFrames[2] = loadPng("romfs/gfx/menu/static/image3_480x272.png");
                staticFrames[3] = loadPng("romfs/gfx/menu/static/image4_480x272.png");
            }
            void unloadStatic() {
                freeImageArray(staticFrames);
            }
        }
    }

    namespace n_newspaper {
        Image* newspaper = nullptr;
        bool loaded = false;

        void loadNewsPaper() {
            freeImageSafe(newspaper);
            newspaper = loadPng("romfs/gfx/newspaper/paper.png");
            loaded = true;
        }
        void unloadNewsPaper() {
            freeImageSafe(newspaper);
            loaded = false;
        }
    }

    namespace n_ending {
        Image* ending = nullptr;

        void loadEnding() {
            freeImageSafe(ending);
            if (save::whichNight == 5) {
                ending = loadPng("romfs/gfx/ending/good.png");
            } else if (save::whichNight == 7) {
                ending = loadPng("romfs/gfx/ending/bad.png");
            } else {
                // Fallback if needed; keep empty if no graphic for other nights
                ending = loadPng("romfs/gfx/ending/good.png");
            }
        }
        void unloadEnding() {
            freeImageSafe(ending);
        }
    }

    namespace n_noPower {
        Image* noPower[2] = {nullptr, nullptr};

        void loadNoPower() {
            noPower[0] = loadPng("romfs/gfx/powerout/freddy1.png");
            noPower[1] = loadPng("romfs/gfx/powerout/freddy2.png");
        }
        void unloadNoPower() {
            freeImageArray(noPower);
        }
    }
}

namespace sprite {

    namespace menu {
        Image* star = nullptr;

        void loadStar() {
            star = loadPng("romfs/gfx/menu/star.png");
        }
        void unloadStar() {
            freeImageSafe(star);
        }
    }

    namespace nightinfo {
        Image* info = nullptr;
        Image* clock = nullptr;

        void loadNightInfoSprite() {
            info  = loadPng("romfs/gfx/nightinfo/info.png");
            clock = loadPng("romfs/gfx/nightinfo/clock.png");
        }
        void unloadNightInfoSprite() {
            freeImageSafe(info);
            freeImageSafe(clock);
        }
    }

    namespace office {
        Image* buttonsLeft[4]  = {nullptr, nullptr, nullptr, nullptr};
        Image* buttonsRight[4] = {nullptr, nullptr, nullptr, nullptr};

        void loadButtons() {
            buttonsLeft[0] = loadPng("romfs/gfx/office/buttons/left/left_0.png");
            buttonsLeft[1] = loadPng("romfs/gfx/office/buttons/left/left_1.png");
            buttonsLeft[2] = loadPng("romfs/gfx/office/buttons/left/left_2.png");
            buttonsLeft[3] = loadPng("romfs/gfx/office/buttons/left/left_3.png");

            buttonsRight[0] = loadPng("romfs/gfx/office/buttons/right/right_0.png");
            buttonsRight[1] = loadPng("romfs/gfx/office/buttons/right/right_1.png");
            buttonsRight[2] = loadPng("romfs/gfx/office/buttons/right/right_2.png");
            buttonsRight[3] = loadPng("romfs/gfx/office/buttons/right/right_3.png");
        }
        void unloadButtons() {
            freeImageArray(buttonsLeft);
            freeImageArray(buttonsRight);
        }

        Image* doorLeft[7]  = {nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr};
        Image* doorRight[7] = {nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr};

        void loadDoors() {
            doorLeft[0] = loadPng("romfs/gfx/office/doors/left/door_1.png");
            doorLeft[1] = loadPng("romfs/gfx/office/doors/left/door_2.png");
            doorLeft[2] = loadPng("romfs/gfx/office/doors/left/door_3.png");
            doorLeft[3] = loadPng("romfs/gfx/office/doors/left/door_4.png");
            doorLeft[4] = loadPng("romfs/gfx/office/doors/left/door_5.png");
            doorLeft[5] = loadPng("romfs/gfx/office/doors/left/door_6.png");
            doorLeft[6] = loadPng("romfs/gfx/office/doors/left/door_7.png");

            doorRight[0] = loadPng("romfs/gfx/office/doors/right/door_1.png");
            doorRight[1] = loadPng("romfs/gfx/office/doors/right/door_2.png");
            doorRight[2] = loadPng("romfs/gfx/office/doors/right/door_3.png");
            doorRight[3] = loadPng("romfs/gfx/office/doors/right/door_4.png");
            doorRight[4] = loadPng("romfs/gfx/office/doors/right/door_5.png");
            doorRight[5] = loadPng("romfs/gfx/office/doors/right/door_6.png");
            doorRight[6] = loadPng("romfs/gfx/office/doors/right/door_7.png");
        }
        void unloadDoors() {
            freeImageArray(doorLeft);
            freeImageArray(doorRight);
        }
    }

    namespace UI {
        namespace office {
            Image* powerBar[5] = {nullptr, nullptr, nullptr, nullptr, nullptr};
            Image* usageFrame = nullptr;
            Image* powerLeft = nullptr;

            void loadPowerInfo() {
                powerBar[0] = loadPng("romfs/gfx/office/ui/bar_1.png");
                powerBar[1] = loadPng("romfs/gfx/office/ui/bar_2.png");
                powerBar[2] = loadPng("romfs/gfx/office/ui/bar_3.png");
                powerBar[3] = loadPng("romfs/gfx/office/ui/bar_4.png");
                powerBar[4] = loadPng("romfs/gfx/office/ui/bar_5.png");

                usageFrame = loadPng("romfs/gfx/office/ui/usage.png");
                powerLeft  = loadPng("romfs/gfx/office/ui/powerLeft.png");
            }
            void unloadPowerInfo() {
                freeImageArray(powerBar);
                freeImageSafe(usageFrame);
                freeImageSafe(powerLeft);
            }

            Image* AM = nullptr;
            Image* Night = nullptr;

            void loadTimeInfo() {
                AM    = loadPng("romfs/gfx/office/ui/AM.png");
                Night = loadPng("romfs/gfx/office/ui/Night.png");
            }
            void unloadTimeInfo() {
                freeImageSafe(AM);
                freeImageSafe(Night);
            }

            Image* camFlip[4] = {nullptr, nullptr, nullptr, nullptr};

            void loadCamFlip() {
                camFlip[0] = loadPng("romfs/gfx/office/camera/animation/flip_0.png");
                camFlip[1] = loadPng("romfs/gfx/office/camera/animation/flip_1.png");
                camFlip[2] = loadPng("romfs/gfx/office/camera/animation/flip_2.png");
                camFlip[3] = loadPng("romfs/gfx/office/camera/animation/flip_3.png");
            }
            void unloadCamFlip() {
                freeImageArray(camFlip);
            }

            Image* cams[11] = {nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr};
 
static std::string lastPath[11];
    // Retire queue (fixed-size, no heap)
        static Image* retireQueue[32];
        static int retireCount = 0;

        static inline void queueRetire(Image* img) {
            if (!img) return;
            if (retireCount < (int)(sizeof(retireQueue)/sizeof(retireQueue[0]))) {
        retireQueue[retireCount++] = img;
         } else {
        // Fallback if queue overflows (rare): free now
        freeImage(img);
    }
}

void postFrame() {
    for (int i = 0; i < retireCount; ++i) {
        freeImage(retireQueue[i]);
    }
    retireCount = 0;
}
            int freddyPosition = 0;
            int bonniePosition = 0;
            int chicaPosition = 0;
            int foxyPosition = 0;

            bool loaded = false;
            
// Build the desired file path for a camera slot based on animatronic positions
static std::string buildCamPath(int idx) {
    // Helper to compose paths
    auto P = [](const char* subdir, const char* name) {
        return std::string("romfs/gfx/office/camera/") + subdir + name + ".png";
    };

    // Shortcuts
    int f = freddyPosition;
    int b = bonniePosition;
    int c = chicaPosition;
    int x = foxyPosition;

    switch (idx) {
        case 0: // Cam 1A
            if (f > 0) f = 0; // keep original behavior
            if (f == 0 && b == 0 && c == 0) return P("main/", "cam1a");
            if (f == 0 && b > 0 && c == 0)  return P("animatronic/cam1a/", "cam1a-freddy&chica");
            if (f == 0 && b == 0 && c > 0)  return P("animatronic/cam1a/", "cam1a-freddy&bonnie");
            if (f == 0 && b > 0 && c > 0)   return (save::whichNight < 4)
                                                ? P("animatronic/cam1a/", "cam1a-freddy")
                                                : P("animatronic/cam1a/", "cam1a-freddystare");
            return P("animatronic/cam1a/", "cam1a-empty");

        case 1: // Cam 1B
            if (b == 1 && c != 1 && f != 1) return P("animatronic/cam1b/", "cam1b-bonnie");
            if (b != 1 && c == 1 && f != 1) return P("animatronic/cam1b/", "cam1b-chica");
            if (f == 1)                     return P("animatronic/cam1b/", "cam1b-freddy");
            return P("main/", "cam1b");

        case 2: // Cam 1C (Foxy)
            switch (x) {
                case 0:  return P("main/", "cam1c");
                case 1:  return P("animatronic/cam1c/", "cam1c-foxy1");
                case 2:  return P("animatronic/cam1c/", "cam1c-foxy2");
                default: return P("animatronic/cam1c/", "cam1c-foxy3");
            }

        case 3: // Cam 2A
            return (b == 3) ? P("animatronic/cam2a/", "cam2a-bonnie") : P("main/", "cam2a");

        case 4: // Cam 2B
            return (b == 5) ? P("animatronic/cam2b/", "cam2b-bonnie") : P("main/", "cam2b");

        case 5: // Cam 3
            return (b == 4) ? P("animatronic/cam3/", "cam3-bonnie") : P("main/", "cam3");

        case 6: // Cam 4A
            if (c == 3 && f != 3) return P("animatronic/cam4a/", "cam4a-chica");
            if (c == 4 && f != 3) return P("animatronic/cam4a/", "cam4a-chicaclose");
            if (f == 3)           return P("animatronic/cam4a/", "cam4a-freddy");
            return P("main/", "cam4a");

        case 7: // Cam 4B
            if (c == 5 && f != 4) return P("animatronic/cam4b/", "cam4b-chica");
            if (f == 4)           return P("animatronic/cam4b/", "cam4b-freddy");
            return P("main/", "cam4b");

        case 8: // Cam 5
            if (b == 7)
                return (save::whichNight > 4)
                    ? P("animatronic/cam5/", "cam5-bonnieclose")
                    : P("animatronic/cam5/", "cam5-bonnie");
            return P("main/", "cam5");

        case 9: // Cam 6
            return P("main/", "cam6");

        case 10: // Cam 7
            if (c == 7 && f != 2) return P("animatronic/cam7/", "cam7-chica");
            if (c == 8 && f != 2) return P("animatronic/cam7/", "cam7-chicaclose");
            if (f == 2)           return P("animatronic/cam7/", "cam7-freddy");
            return P("main/", "cam7");
    }
    // Fallback (shouldn’t happen)
    return P("main/", "cam6");
}
static int nextCamIdx = 0;
static constexpr int kCamReloadBudget = 2; // tune 2..4

void loadCams() {
    if (!loaded) { for (int i = 0; i < 11; ++i) lastPath[i].clear(); }

    int processed = 0;
    int reloaded = 0;
    while (processed < 11 && reloaded < kCamReloadBudget) {
        int i = (nextCamIdx + processed) % 11;
        const std::string newPath = buildCamPath(i);

        if (!(lastPath[i] == newPath && cams[i])) {
            Image* newImg = loadPng(newPath.c_str());
            if (newImg) {
                queueRetire(cams[i]);
                cams[i] = newImg;
                lastPath[i] = newPath;
            }
            reloaded++;
        }
        processed++;
    }
    nextCamIdx = (nextCamIdx + processed) % 11;
    loaded = true;
}

void unloadCams() {
    for (int i = 0; i < 11; ++i) {
        queueRetire(cams[i]);
        cams[i] = nullptr;
        lastPath[i].clear();
    }
    loaded = false;
}
            Image* camNames[11]   = {nullptr};
            Image* camButtons[11] = {nullptr};
            Image* reticle   = nullptr;
            Image* camBorder = nullptr;
            Image* recording = nullptr;
            Image* camMap    = nullptr;

            void loadCamUi() {
                camBorder = loadPng("romfs/gfx/office/ui/camera_border.png"); 
                camMap    = loadPng("romfs/gfx/office/ui/camera-map.png");
                recording = loadPng("romfs/gfx/office/ui/recording.png");

                camNames[0] = loadPng("romfs/gfx/office/ui/cam-names/ShowStage.png");
                camNames[1] = loadPng("romfs/gfx/office/ui/cam-names/DiningArea.png");
                camNames[2] = loadPng("romfs/gfx/office/ui/cam-names/PirateCove.png");
                camNames[3] = loadPng("romfs/gfx/office/ui/cam-names/W-Hall.png");
                camNames[4] = loadPng("romfs/gfx/office/ui/cam-names/W-Hall-corner.png");
                camNames[5] = loadPng("romfs/gfx/office/ui/cam-names/Closet.png");
                camNames[6] = loadPng("romfs/gfx/office/ui/cam-names/E-Hall.png");
                camNames[7] = loadPng("romfs/gfx/office/ui/cam-names/E-Hall-corner.png");
                camNames[8] = loadPng("romfs/gfx/office/ui/cam-names/BackStage.png");
                camNames[9] = loadPng("romfs/gfx/office/ui/cam-names/Kitchen.png");
                camNames[10]= loadPng("romfs/gfx/office/ui/cam-names/Restrooms.png");

                camButtons[0]  = loadPng("romfs/gfx/office/camera/main/buttons/cam1a.png");
                camButtons[1]  = loadPng("romfs/gfx/office/camera/main/buttons/cam1b.png");
                camButtons[2]  = loadPng("romfs/gfx/office/camera/main/buttons/cam1c.png");
                camButtons[3]  = loadPng("romfs/gfx/office/camera/main/buttons/cam2a.png");
                camButtons[4]  = loadPng("romfs/gfx/office/camera/main/buttons/cam2b.png");
                camButtons[5]  = loadPng("romfs/gfx/office/camera/main/buttons/cam3.png");
                camButtons[6]  = loadPng("romfs/gfx/office/camera/main/buttons/cam4a.png");
                camButtons[7]  = loadPng("romfs/gfx/office/camera/main/buttons/cam4b.png");
                camButtons[8]  = loadPng("romfs/gfx/office/camera/main/buttons/cam5.png");
                camButtons[9]  = loadPng("romfs/gfx/office/camera/main/buttons/cam6.png");
                camButtons[10] = loadPng("romfs/gfx/office/camera/main/buttons/cam7.png");

                reticle = loadPng("romfs/gfx/office/camera/main/buttons/reticle.png");
            }
            void unloadCamUi() {
                freeImageSafe(camBorder);
                freeImageSafe(camMap);
                freeImageSafe(recording);

                freeImageArray(camNames);
                freeImageArray(camButtons);
                freeImageSafe(reticle);
            }
        }

        namespace customnight {
            Image* icons[4]         = {nullptr, nullptr, nullptr, nullptr};
            Image* reticle          = nullptr;
            Image* instructions[2]  = {nullptr, nullptr};
            Image* title            = nullptr;
            Image* arrows[2]        = {nullptr, nullptr};
            Image* levelDesc        = nullptr;
            Image* difficulty       = nullptr;
            Image* names[4]         = {nullptr, nullptr, nullptr, nullptr};
            Image* create           = nullptr;
            Image* exit             = nullptr;
            Image* goldFreddy       = nullptr;

            void loadIcons() {
                icons[0] = loadPng("romfs/gfx/customnight/icons/freddy.png");
                icons[1] = loadPng("romfs/gfx/customnight/icons/bonnie.png");
                icons[2] = loadPng("romfs/gfx/customnight/icons/chika.png");
                icons[3] = loadPng("romfs/gfx/customnight/icons/foxy.png");
            }
            void unloadIcons() {
                freeImageArray(icons);
            }

            void loadReticle() {
                reticle = loadPng("romfs/gfx/customnight/icons/reticle.png");
            }
            void unloadReticle() {
                freeImageSafe(reticle);
            }

            void loadInstructions() {
                instructions[0] = loadPng("romfs/gfx/customnight/ui/L.png");
                instructions[1] = loadPng("romfs/gfx/customnight/ui/R.png");
            }
            void unloadInstructions() {
                freeImageArray(instructions);
            }

            void loadTitle() {
                title = loadPng("romfs/gfx/customnight/ui/title.png");
            }
            void unloadTitle() {
                freeImageSafe(title);
            }

            void loadArrows() {
                arrows[0] = loadPng("romfs/gfx/customnight/ui/left.png");
                arrows[1] = loadPng("romfs/gfx/customnight/ui/right.png");
            }
            void unloadArrows() {
                freeImageArray(arrows);
            }

            void loadText() {
                levelDesc  = loadPng("romfs/gfx/customnight/ui/AI.png");
                difficulty = loadPng("romfs/gfx/customnight/ui/difficulty.png");
            }
            void unloadText() {
                freeImageSafe(levelDesc);
                freeImageSafe(difficulty);
            }

            void loadNames() {
                names[0] = loadPng("romfs/gfx/customnight/ui/freddy.png");
                names[1] = loadPng("romfs/gfx/customnight/ui/bonnie.png");
                names[2] = loadPng("romfs/gfx/customnight/ui/chika.png");
                names[3] = loadPng("romfs/gfx/customnight/ui/foxy.png");
            }
            void unloadNames() {
                freeImageArray(names);
            }

            void loadActions() {
                create = loadPng("romfs/gfx/customnight/ui/create.png");
                exit   = loadPng("romfs/gfx/customnight/ui/exit.png");
            }
            void unloadActions() {
                freeImageSafe(create);
                freeImageSafe(exit);
            }

            void loadGoldenFreddy() {
                goldFreddy = loadPng("romfs/gfx/customnight/ui/gold.png");
            }
            void unloadGoldenFreddy() {
                freeImageSafe(goldFreddy);
            }
        }
    }

    namespace n_jumpscare {
        Image* jumpscareAnim[9] = {nullptr};
        int whichJumpscare = 0;
        bool loaded = false;

        static inline void unloadFrames() {
            freeImageArray(jumpscareAnim);
            loaded = false;
        }

        void loadFreddy() {
            unloadFrames();
            jumpscareAnim[0] = loadPng("romfs/gfx/jumpscare/freddy/0.png");
            jumpscareAnim[1] = loadPng("romfs/gfx/jumpscare/freddy/1.png");
            jumpscareAnim[2] = loadPng("romfs/gfx/jumpscare/freddy/2.png");
            jumpscareAnim[3] = loadPng("romfs/gfx/jumpscare/freddy/3.png");
            jumpscareAnim[4] = loadPng("romfs/gfx/jumpscare/freddy/4.png");
            jumpscareAnim[5] = loadPng("romfs/gfx/jumpscare/freddy/5.png");
            jumpscareAnim[6] = loadPng("romfs/gfx/jumpscare/freddy/6.png");
            jumpscareAnim[7] = loadPng("romfs/gfx/jumpscare/freddy/7.png");
            jumpscareAnim[8] = loadPng("romfs/gfx/jumpscare/freddy/8.png");
            loaded = true;
        }

        void loadBonnie() {
            unloadFrames();
            jumpscareAnim[0] = loadPng("romfs/gfx/jumpscare/bonnie/0.png");
            jumpscareAnim[1] = loadPng("romfs/gfx/jumpscare/bonnie/1.png");
            jumpscareAnim[2] = loadPng("romfs/gfx/jumpscare/bonnie/2.png");
            jumpscareAnim[3] = loadPng("romfs/gfx/jumpscare/bonnie/3.png");
            jumpscareAnim[4] = loadPng("romfs/gfx/jumpscare/bonnie/4.png");
            jumpscareAnim[5] = loadPng("romfs/gfx/jumpscare/bonnie/5.png");
            jumpscareAnim[6] = loadPng("romfs/gfx/jumpscare/bonnie/6.png");
            jumpscareAnim[7] = loadPng("romfs/gfx/jumpscare/bonnie/7.png");
            jumpscareAnim[8] = loadPng("romfs/gfx/jumpscare/bonnie/8.png");
            loaded = true;
        }

        void loadChica() {
            unloadFrames();
            jumpscareAnim[0] = loadPng("romfs/gfx/jumpscare/chica/0.png");
            jumpscareAnim[1] = loadPng("romfs/gfx/jumpscare/chica/1.png");
            jumpscareAnim[2] = loadPng("romfs/gfx/jumpscare/chica/2.png");
            jumpscareAnim[3] = loadPng("romfs/gfx/jumpscare/chica/3.png");
            jumpscareAnim[4] = loadPng("romfs/gfx/jumpscare/chica/4.png");
            jumpscareAnim[5] = loadPng("romfs/gfx/jumpscare/chica/5.png");
            jumpscareAnim[6] = loadPng("romfs/gfx/jumpscare/chica/6.png");
            jumpscareAnim[7] = loadPng("romfs/gfx/jumpscare/chica/7.png");
            jumpscareAnim[8] = loadPng("romfs/gfx/jumpscare/chica/8.png");
            loaded = true;
        }

        void loadFoxy() {
            unloadFrames();
            jumpscareAnim[0] = loadPng("romfs/gfx/jumpscare/foxy/0.png");
            jumpscareAnim[1] = loadPng("romfs/gfx/jumpscare/foxy/1.png");
            jumpscareAnim[2] = loadPng("romfs/gfx/jumpscare/foxy/2.png");
            jumpscareAnim[3] = loadPng("romfs/gfx/jumpscare/foxy/3.png");
            jumpscareAnim[4] = loadPng("romfs/gfx/jumpscare/foxy/4.png");
            jumpscareAnim[5] = loadPng("romfs/gfx/jumpscare/foxy/5.png");
            jumpscareAnim[6] = loadPng("romfs/gfx/jumpscare/foxy/6.png");
            jumpscareAnim[7] = loadPng("romfs/gfx/jumpscare/foxy/7.png");
            jumpscareAnim[8] = loadPng("romfs/gfx/jumpscare/foxy/8.png");
            loaded = true;
        }

        void loadJumpscare() {
            // Always unload the previous set before loading a new one
            unloadFrames();

            if (whichJumpscare == 1)      loadFreddy();
            else if (whichJumpscare == 2) loadBonnie();
            else if (whichJumpscare == 3) loadChica();
            else if (whichJumpscare == 4) loadFoxy();
        }

        void unloadJumpscare() {
            unloadFrames();
        }
    }
}

namespace officeImage {
    Image* office1Sprites[5] = {nullptr, nullptr, nullptr, nullptr, nullptr};
    Image* office2Sprites[5] = {nullptr, nullptr, nullptr, nullptr, nullptr};

    void loadOffice1Sprites() {
        office1Sprites[0] = loadPng("romfs/gfx/office/chuncks/nothing/office_1.png");
        office1Sprites[1] = loadPng("romfs/gfx/office/chuncks/left-empty/office_1.png");
        office1Sprites[2] = loadPng("romfs/gfx/office/chuncks/right-empty/office_1.png");
        office1Sprites[3] = loadPng("romfs/gfx/office/chuncks/left-bonnie/office_1.png");
        office1Sprites[4] = loadPng("romfs/gfx/office/chuncks/right-chika/office_1.png");
    }
    void unloadOffice1Sprites() {
        freeImageArray(office1Sprites);
    }

    void loadOffice2Sprites() {
        office2Sprites[0] = loadPng("romfs/gfx/office/chuncks/nothing/office_2.png");
        office2Sprites[1] = loadPng("romfs/gfx/office/chuncks/left-empty/office_2.png");
        office2Sprites[2] = loadPng("romfs/gfx/office/chuncks/right-empty/office_2.png");
        office2Sprites[3] = loadPng("romfs/gfx/office/chuncks/left-bonnie/office_2.png");
        office2Sprites[4] = loadPng("romfs/gfx/office/chuncks/right-chika/office_2.png");
    }
    void unloadOffice2Sprites() {
        freeImageArray(office2Sprites);
    }
}

namespace text {
    namespace global {
        Image* nightNumbersNormal[10] = {nullptr};
        Image* nightNumbersPixel[10]  = {nullptr};
        Image* symbols = nullptr;

        void loadNightText() {
            nightNumbersNormal[0] = loadPng("romfs/gfx/global/numbers/normal/0-2.png");
            nightNumbersNormal[1] = loadPng("romfs/gfx/global/numbers/normal/1.png");
            nightNumbersNormal[2] = loadPng("romfs/gfx/global/numbers/normal/2.png");
            nightNumbersNormal[3] = loadPng("romfs/gfx/global/numbers/normal/3.png");
            nightNumbersNormal[4] = loadPng("romfs/gfx/global/numbers/normal/4.png");
            nightNumbersNormal[5] = loadPng("romfs/gfx/global/numbers/normal/5.png");
            nightNumbersNormal[6] = loadPng("romfs/gfx/global/numbers/normal/6.png");
            nightNumbersNormal[7] = loadPng("romfs/gfx/global/numbers/normal/7.png");
            nightNumbersNormal[8] = loadPng("romfs/gfx/global/numbers/normal/8.png");
            nightNumbersNormal[9] = loadPng("romfs/gfx/global/numbers/normal/9.png");

            nightNumbersPixel[0] = loadPng("romfs/gfx/global/numbers/pixel/0.png");
            nightNumbersPixel[1] = loadPng("romfs/gfx/global/numbers/pixel/1.png");
            nightNumbersPixel[2] = loadPng("romfs/gfx/global/numbers/pixel/2.png");
            nightNumbersPixel[3] = loadPng("romfs/gfx/global/numbers/pixel/3.png");
            nightNumbersPixel[4] = loadPng("romfs/gfx/global/numbers/pixel/4.png");
            nightNumbersPixel[5] = loadPng("romfs/gfx/global/numbers/pixel/5.png");
            nightNumbersPixel[6] = loadPng("romfs/gfx/global/numbers/pixel/6.png");
            nightNumbersPixel[7] = loadPng("romfs/gfx/global/numbers/pixel/7.png");
            nightNumbersPixel[8] = loadPng("romfs/gfx/global/numbers/pixel/8.png");
            nightNumbersPixel[9] = loadPng("romfs/gfx/global/numbers/pixel/9.png");

            symbols = loadPng("romfs/gfx/global/numbers/symbols/%.png");
        }
        void unloadNightText() {
            freeImageArray(nightNumbersNormal);
            freeImageArray(nightNumbersPixel);
            freeImageSafe(symbols);
        }
    }
}