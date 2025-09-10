#include "included/global.hpp"

//globally used
#include "included/image2.hpp"
#include "included/audio.hpp"
#include "included/state.hpp"
#include "included/save.hpp"
#include "included/power.hpp"
#include "included/time.hpp"
#include "included/camera.hpp"

//only for certain areas
#include "included/menu.hpp"
#include "included/newspaper.hpp"
#include "included/nightinfo.hpp"
#include "included/office.hpp"
#include "included/customnight.hpp"
#include "included/sixam.hpp"
#include "included/dead.hpp"
#include "included/ending.hpp"
#include "included/jumpscare.hpp"
#include "included/powerout.hpp"


#include <cstdlib>

using namespace std;

#define BUF_WIDTH  512
#define SCR_WIDTH  480
#define SCR_HEIGHT 272

#include <pspkernel.h>

PSP_MODULE_INFO("FNaF 1 PSP v1.5", 0, 1, 0);
PSP_MAIN_THREAD_ATTR(THREAD_ATTR_USER | THREAD_ATTR_VFPU);
// Choose one heap macro (pick only one):
PSP_HEAP_SIZE_KB(-1024); // maximize heap (reserve ~1MB for system)
// PSP_HEAP_SIZE_MAX();   // alternative macro in newer SDKs

void* fbp0 = nullptr;
void* fbp1 = nullptr;

static unsigned int __attribute__((aligned(16))) DisplayList[64 * 1024]; // 256 KB
//static unsigned int __attribute__((aligned(16))) DisplayList[262144]; //1MB Display


static int exit_callback(int /*arg1*/, int /*arg2*/, void* /*common*/) {
    sceKernelExitGame();
    return 0;
}

static int callbackThread(SceSize /*args*/, void* /*argp*/) {
    int cbid = sceKernelCreateCallback("Exit Callback", exit_callback, nullptr);
    if (cbid >= 0) {
        sceKernelRegisterExitCallback(cbid);
    }
    sceKernelSleepThreadCB();
    return 0;
}

static void setupCallbacks() {
    int thid = sceKernelCreateThread("exit_callback_thread",
                                     callbackThread,
                                     0x11,        // priority
                                     0xFA0,       // stack size
                                     0,           // attributes
                                     nullptr);
    if (thid >= 0) {
        sceKernelStartThread(thid, 0, nullptr);
    }
}

void InitGU() {
    // 16-bit color buffers for VRAM headroom
    fbp0 = getStaticVramBuffer(BUF_WIDTH, SCR_HEIGHT, GU_PSM_5650);
    fbp1 = getStaticVramBuffer(BUF_WIDTH, SCR_HEIGHT, GU_PSM_5650);

    sceGuInit();
    sceGuStart(GU_DIRECT, DisplayList);
    sceGuDrawBuffer(GU_PSM_5650, fbp0, BUF_WIDTH);
    sceGuDispBuffer(SCR_WIDTH, SCR_HEIGHT, fbp1, BUF_WIDTH);

    // 2D-friendly viewport
    sceGuOffset(2048 - (SCR_WIDTH / 2), 2048 - (SCR_HEIGHT / 2));
    sceGuViewport(2048, 2048, SCR_WIDTH, SCR_HEIGHT);

    // Clear only color (no depth buffer used)
    sceGuClearColor(0);
    sceGuClear(GU_COLOR_BUFFER_BIT);

    // Scissor to screen
    sceGuScissor(0, 0, SCR_WIDTH, SCR_HEIGHT);
    sceGuEnable(GU_SCISSOR_TEST);

    // Blending and alpha test
    sceGuEnable(GU_BLEND);
    sceGuBlendFunc(GU_ADD, GU_SRC_ALPHA, GU_ONE_MINUS_SRC_ALPHA, 0, 0);
    sceGuEnable(GU_ALPHA_TEST);
    sceGuAlphaFunc(GU_GREATER, 0, 0xff);

    // 2D: textures yes, no lighting/fog/depth/culling/dither
    sceGuEnable(GU_TEXTURE_2D);
    sceGuTexFilter(GU_NEAREST, GU_NEAREST); // keep crisp; set LINEAR if you prefer softening

    sceGuDisable(GU_LIGHTING);
    sceGuDisable(GU_LIGHT0);
    sceGuDisable(GU_LIGHT1);
    sceGuDisable(GU_LIGHT2);
    sceGuDisable(GU_LIGHT3);
    sceGuDisable(GU_FOG);
    sceGuDisable(GU_DEPTH_TEST);
    sceGuDisable(GU_CLIP_PLANES);
    sceGuDisable(GU_CULL_FACE);
    sceGuDisable(GU_DITHER);

    sceGuFinish();
    sceGuSync(GU_SYNC_FINISH, GU_SYNC_WHAT_DONE);

    sceDisplayWaitVblankStartCB();
    sceGuDisplay(GU_TRUE);
}
void initEngine(){
    setupCallbacks();
    //pspDebugScreenInit();

    sceCtrlSetSamplingCycle(0);
    sceCtrlSetSamplingMode(PSP_CTRL_MODE_ANALOG);

    InitGU();
    
    //oslInit();
    VirtualFileInit();
    oslInitAudio();
}

void initGame(){
    //state::isMenu = false;

    save::file();
    //save::readData();

    image::global::n_static::loadStatic();

    image::menu::loadMenuBackground();
    image::menu::loadLogo();
    sprite::menu::loadStar();
    image::menu::loadTextAndCursor();
    image::menu::loadCopyright();
    text::global::loadNightText();

    menu::menuCursor::moveCursor();

    music::menu::loadMenuMusic();
    music::menu::playMenuMusic();

    //state::isMenu = true;
}

bool reseted = true;

void resetMain(){

    if (reseted == false){
        sprite::UI::office::freddyPosition = 0;
        sprite::UI::office::bonniePosition = 0;
        sprite::UI::office::chicaPosition = 0;
        sprite::UI::office::foxyPosition = 0;
                
        timegame::reset();
        office::reset();
        camera::reset();
        power::reset();
        animatronic::reset();
        nightinfo::reset();
        sixam::reset();
        jumpscare::reset();
        powerout::reset();
        newspaper::reset();
        dead::reset();
        customnight::reset();
        ending::reset();

        reseted = true;
    }
}

int cursorMoveTime = 5;

void handleMenuState(SceCtrlData ctrlData){
    menu::render::renderBackground();
    menu::render::animateBackground();
    menu::render::renderLogo();
    menu::render::renderCopyright();
    menu::render::renderStars();
    menu::menuCursor::renderCursor();

    menu::n_static::renderStatic();
    menu::n_static::animateStatic();

if (cursorMoveTime <= 0) {
    if (ctrlData.Buttons & PSP_CTRL_CROSS) { menu::menuCursor::select(); cursorMoveTime = 7; }
    if (ctrlData.Buttons & PSP_CTRL_UP)    { menu::menuCursor::cursorPos--; menu::menuCursor::moveCursor(); cursorMoveTime = 7; }
    if (ctrlData.Buttons & PSP_CTRL_DOWN)  { menu::menuCursor::cursorPos++; menu::menuCursor::moveCursor(); cursorMoveTime = 7; }
} else {
    cursorMoveTime -= 1;
}

    resetMain();
}

void handleNewspaperState(){
    newspaper::render::renderNewspaper();
    newspaper::next::initNightinfo();

    resetMain();
}

void handleNightInfoState(){
    nightinfo::render::renderNightinfo();

    nightinfo::next::preloadOffice();
    nightinfo::next::initOffice();

    resetMain();
}

void handleOfficeState(SceCtrlData ctrlData) {
    // Render the office when the camera is not in use or closing
    if (!camera::isUsing || camera::closing) {
        office::render::renderOffice();
        office::main::moveOffice();
        office::lights::lights();
        office::buttons::setButtonFrame();
        office::render::renderButtons();
        office::render::renderDoors();
    }

    // Run AI logic
    animatronic::runAiLoop();
    animatronic::forceAnimatronicAiReset();

    // Render camera flipping and UI
    camera::render::renderCamFlip();
    if (camera::isUsing && animatronic::reloaded && sprite::UI::office::loaded &&
        animatronic::foxy::position != 3 && animatronic::foxy::position != 4) {
        camera::render::renderCamera();
    }


    // Render UI and static effects
    camera::render::renderUi();
    camera::n_static::renderStatic();
    camera::n_static::animateStatic();

    // Handle power updates
    power::render::renderPowerLeft();
    power::update::drainConstant();
    power::update::checkDrain();

    // Render and update time
    timegame::render::renderTime();
    timegame::update::updateTime();

    // Door animations
    if (office::closingLeft) {
        office::doors::closeLeft();
    } else if (office::openingLeft) {
        office::doors::openLeft();
    }

    if (office::closingRight) {
        office::doors::closeRight();
    } else if (office::openingRight) {
        office::doors::openRight();
    }

    // Camera animations
    if (camera::opening) {
        camera::animation::openCams();
    } else if (camera::closing) {
        camera::animation::closeCams();
    }

    // Office directional controls
    if (!camera::isUsing) {
        switch (ctrlData.Buttons) {
            case PSP_CTRL_LEFT:
                office::dir = "right";
                break;
            case PSP_CTRL_RIGHT:
                office::dir = "left";
                break;
            default:
                office::dir = "none";
                break;
        }

        office::buttonState = (ctrlData.Buttons & PSP_CTRL_SQUARE) ? "down" : "up";

        if (ctrlData.Buttons & PSP_CTRL_CROSS) {
            if (office::doorButtonState == "up") {
                office::doors::doors();
            }
            office::doorButtonState = "held";
        } else {
            office::doorButtonState = "up";
        }
    }

    // Handle phone call stop
    if (ctrlData.Buttons & PSP_CTRL_CIRCLE) {
        if (!call::stopped) {
            call::unloadPhoneCalls();
        }
    }

    // Camera directional and button controls
    switch (ctrlData.Buttons) {
        case PSP_CTRL_TRIANGLE:
            if (camera::buttonState == "up") {
                camera::animation::camera();
            }
            camera::buttonState = "held";
            break;

        case PSP_CTRL_UP:
            if (camera::buttonState == "up") {
                camera::system::up();
            }
            camera::buttonState = "held";
            break;

        case PSP_CTRL_DOWN:
            if (camera::buttonState == "up") {
                camera::system::down();
            }
            camera::buttonState = "held";
            break;

        case PSP_CTRL_LEFT:
            if (camera::buttonState == "up") {
                camera::system::left();
            }
            camera::buttonState = "held";
            break;

        case PSP_CTRL_RIGHT:
            if (camera::buttonState == "up") {
                camera::system::right();
            }
            camera::buttonState = "held";
            break;

        default:
            camera::buttonState = "up";
            break;
    }

    reseted = false;
}

void handleCustomNightState(SceCtrlData ctrlData) {
    // Render all Custom Night elements
    customnight::render::renderHeads();
    customnight::render::renderReticle();
    customnight::render::renderInstructions();
    customnight::render::renderTitle();
    customnight::render::renderNames();
    customnight::render::renderArrows();
    customnight::render::renderText();
    customnight::render::renderLevels();
    customnight::render::renderActions();
    customnight::render::renderGoldenFreddy();

    // Update the position of the reticle
    customnight::reticle::updatePosition();

    // Handle input only when the cursor is ready to move
    if (cursorMoveTime <= 0) {
        switch (ctrlData.Buttons) {
            case PSP_CTRL_RTRIGGER:
                customnight::reticle::moveReticleRight();
                cursorMoveTime = 10;
                break;

            case PSP_CTRL_LTRIGGER:
                customnight::reticle::moveReticleLeft();
                cursorMoveTime = 10;
                break;

            case PSP_CTRL_RIGHT:
                customnight::edit::plus();
                cursorMoveTime = 10;
                break;

            case PSP_CTRL_LEFT:
                customnight::edit::minus();
                cursorMoveTime = 10;
                break;

            case PSP_CTRL_CROSS:
                customnight::actions::create();
                cursorMoveTime = 10;
                break;

            case PSP_CTRL_CIRCLE:
                customnight::actions::exit();
                cursorMoveTime = 10;
                break;

            default:
                break;
        }
    } else {
        cursorMoveTime -= 1; // Decrease the cursor move timer
    }

    resetMain(); // Reset any game-related states
}

void handleSixAmState(){
    sixam::render::renderSixAm();
    sixam::animate::wait();
    sixam::next::wait();

    resetMain();
}

void handlePoweroutState(){
    powerout::render::renderPowerout();
    powerout::animate::animatePowerOut();

    resetMain();
}

void handleJumpscareState(){
    jumpscare::load::loadWithDelay();
    jumpscare::render::renderJumpscare();
    jumpscare::animate::animateJumpscare();

    reseted = false;

}

void handleEndingState(){
    ending::render::renderEnding();
    ending::wait::waitForFrames();

    resetMain();
}

void handleDeadState(){
    dead::n_static::renderStatic();
    dead::n_static::animateStatic();

    dead::wait::waitForFrames();

    resetMain();
}

void handleState(SceCtrlData ctrlData) {
    if (state::isMenu){
        handleMenuState(ctrlData);
    }
    else if (state::isNewspaper){
        handleNewspaperState();
    }
    else if (state::isNightinfo){
        handleNightInfoState();
    }
    else if (state::isOffice){
        handleOfficeState(ctrlData);
    }
    else if (state::isCustomNight){
        handleCustomNightState(ctrlData);
    }
    else if (state::isSixAm){
        handleSixAmState();
    }
    else if (state::isPowerOut){
        handlePoweroutState();
    }
    else if (state::isJumpscare){
        handleJumpscareState();
    }
    else if (state::isEnding){
        handleEndingState();
    }
    else if (state::isDead){
        handleDeadState();
    }
}


auto main() -> int {
    // Optional: boost clock if needed
    // scePowerSetClockFrequency(333, 333, 166);

    SceCtrlData ctrlData{};
    initEngine();
    initGame();

    // For better input handling
    u32 padCurr = 0;

    while (true) {
        // Non-blocking read is fine too: sceCtrlPeekBufferPositive
        sceCtrlReadBufferPositive(&ctrlData, 1);
        padCurr = ctrlData.Buttons;

        sceGuStart(GU_DIRECT, DisplayList);

        // Clear only color (depth not used)
        sceGuClear(GU_COLOR_BUFFER_BIT);

        // If you really need GUM, set orthographic once and avoid per-frame perspective for 2D:
        // sceGumMatrixMode(GU_PROJECTION);
        // sceGumLoadIdentity();
        // sceGumOrtho(0, SCR_WIDTH, SCR_HEIGHT, 0, -1.0f, 1.0f);
        // sceGumMatrixMode(GU_VIEW);
        // sceGumLoadIdentity();

        handleState(ctrlData); // your draw+update

        sceGuFinish();
        sceGuSync(GU_SYNC_FINISH, GU_SYNC_WHAT_DONE);

        // Cap to 60 Hz; wait AFTER finishing the list
        sceDisplayWaitVblankStartCB();

        // Present
        sceGuSwapBuffers();

        // Safe place for deferred load/unload (GPU is done with textures)
        powerout::postFrame();
        ending::postFrame();
        dead::postFrame();
        sixam::postFrame();
        nightinfo::postFrame(); 
        sprite::UI::office::postFrame(); 
    }
}