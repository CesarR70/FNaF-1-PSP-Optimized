#include "included/animatronic.hpp"
#include "included/state.hpp"

// Small, branch prediction hints (PSPSDK uses GCC)
#if defined(__GNUC__)
#  define LIKELY(x)   (__builtin_expect(!!(x), 1))
#  define UNLIKELY(x) (__builtin_expect(!!(x), 0))
#else
#  define LIKELY(x)   (x)
#  define UNLIKELY(x) (x)
#endif

namespace animatronic {

    // ------------------------------
    // Global state (kept same names)
    // ------------------------------
    volatile bool isMoving = false; // volatile for thread safety
    volatile bool reloaded = true; // volatile for thread safety
    volatile bool usingCams = false; // volatile for thread safety
    volatile bool leftClosed = false; // volatile for thread safety
    volatile bool rightClosed = false; // volatile for thread safety

    volatile bool jumpscaring = false; // volatile for thread safety
    bool locked = false;

    int ThreadIdR = -1;
    SceUID ReloadSemaphore = -1;  // exposed in header
    SceUID FoxyStateSemaphore = -1;  // for Foxy attack state synchronization
    int PspThreadStatus = 1;

    bool unloaded = false;

    int waitBeforeForceReset = 450; // int, not float

    // ------------------------------
    // Internal reload worker state
    // ------------------------------
    static SceUID sReloadThread = -1;
    // Coalesce (un)load requests across a frame to avoid repeated heavy I/O
    static volatile int sPendingReloads = 0;

    // Forward decls
    static void ensureReloadWorker();
    static inline void queueReloadOnce();

    // ------------------------------
    // Fast RNG (drop-in replacement for rand()%N)
    // ------------------------------
#if ANIMATRONIC_USE_STD_RAND
    static inline int fastRandN(int n) { return rand() % n; }
#else
    static inline uint32_t xorshift32() {
        static uint32_t s = 0xA3C59AC3u; // non-zero seed
        s ^= s << 13;
        s ^= s >> 17;
        s ^= s << 5;
        return s;
    }
    static inline int fastRandN(int n) {
        // Map 32-bit to [0,n) with good distribution
        return static_cast<int>((static_cast<uint64_t>(xorshift32()) * static_cast<uint32_t>(n)) >> 32);
    }
#endif

    // ==============================
    // Reset & utilities
    // ==============================
    void reset() {
        freddy::levelOnes = 0;
        freddy::levelTenths = 0;
        freddy::totalLevel = 0;
        freddy::atDoor = false;

        bonnie::levelOnes = 0;
        bonnie::levelTenths = 0;
        bonnie::totalLevel = 0;
        bonnie::atDoor = false;

        chika::levelOnes = 0;
        chika::levelTenths = 0;
        chika::totalLevel = 0;
        chika::atDoor = false;

        foxy::levelOnes = 0;
        foxy::levelTenths = 0;
        foxy::totalLevel = 0;
        foxy::atDoor = false;

        freddy::position = 0;
        bonnie::position = 0;
        chika::position = 0;
        foxy::position = 0;

        isMoving = false;
        reloaded = true;
        usingCams = false;
        leftClosed = false;
        rightClosed = false;

        jumpscaring = false;
        unloaded = false;
        locked = false;

        waitBeforeForceReset = 450;
        sprite::n_jumpscare::whichJumpscare = 0;

        // CRITICAL: Reset reload worker state to prevent state persistence between nights
        // This fixes the issue where Night 4→5 transitions inherit worker thread problems
        // Also fixes death→menu→restart deadlock issues
        sPendingReloads = 0;
        
        // Drain any pending semaphore tokens to ensure clean state
        if (ReloadSemaphore >= 0) {
            while (sceKernelPollSema(ReloadSemaphore, 1) >= 0) { /* drain */ }
        }
        
        // Initialize Foxy state semaphore for thread-safe attack state management
        if (FoxyStateSemaphore < 0) {
            FoxyStateSemaphore = sceKernelCreateSema("foxy_state", 0, 1, 1, nullptr);
        }
        
        // CRITICAL: Ensure worker thread state is completely cleared
        // This prevents race conditions where the worker thread is still processing when we restart
        
        ensureReloadWorker();
    }

    // CRITICAL: Enhanced reset for death transitions to prevent deadlock inheritance
    void resetForDeath() {
        // First, ensure any ongoing reload operations are stopped
        isMoving = false;
        reloaded = true;
        jumpscaring = false;
        usingCams = false; // CRITICAL: Reset camera usage state
        
        // Clear all pending reload requests
        sPendingReloads = 0;
        
        // Drain any pending semaphore tokens to ensure clean state
        if (ReloadSemaphore >= 0) {
            while (sceKernelPollSema(ReloadSemaphore, 1) >= 0) { /* drain */ }
        }
        
        // Reset animatronic positions and levels (but don't call full reset to avoid double-reset)
        freddy::levelOnes = 0;
        freddy::levelTenths = 0;
        freddy::totalLevel = 0;
        freddy::atDoor = false;
        freddy::position = 0;

        bonnie::levelOnes = 0;
        bonnie::levelTenths = 0;
        bonnie::totalLevel = 0;
        bonnie::atDoor = false;
        bonnie::position = 0;

        chika::levelOnes = 0;
        chika::levelTenths = 0;
        chika::totalLevel = 0;
        chika::atDoor = false;
        chika::position = 0;

        foxy::levelOnes = 0;
        foxy::levelTenths = 0;
        foxy::totalLevel = 0;
        foxy::atDoor = false;
        foxy::position = 0;

        leftClosed = false;
        rightClosed = false;
        unloaded = false;
        locked = false;
        waitBeforeForceReset = 450;
        sprite::n_jumpscare::whichJumpscare = 0;
        
        // Ensure worker thread state is completely cleared
        ensureReloadWorker();
    }

    void forceAnimatronicAiReset() {
        if (waitBeforeForceReset <= 0 && !jumpscaring) {

            if (reloaded && isMoving) {
                isMoving = false;
            }
            if (isMoving && reloaded && freddy::delay > 0 && bonnie::delay > 0 && chika::delay > 0 && foxy::delay > 0) {
                isMoving = false;
            }

            if (freddy::position > 6) freddy::position = 6;
            if (bonnie::position > 6 && bonnie::position != 7) bonnie::position = 6;
            if (chika::position > 6 && chika::position != 7 && chika::position != 8 && chika::position != 9) chika::position = 6;
            if (foxy::position > 4) foxy::position = 4;

            if (foxy::position == 0) foxy::position = 0;

            if (bonnie::inOtherRoom && bonnie::position != 7) bonnie::inOtherRoom = false;
            if (chika::inOtherRoom && chika::position != 7 && chika::position != 8 && chika::position != 9) chika::inOtherRoom = false;

            if (freddy::atDoor && freddy::position < 6) freddy::atDoor = false;
            if (bonnie::atDoor && bonnie::position < 6) bonnie::atDoor = false;
            if (chika::atDoor && chika::position < 6) chika::atDoor = false;
            if (foxy::atDoor && foxy::position < 4) {
                foxy::atDoor = false;
                foxy::resetAttackState();
            }

            waitBeforeForceReset = 450;
        } else {
            waitBeforeForceReset -= 1;
        }
    }

    // ==============================
    // Persistent reload worker
    // ==============================
    static inline void drainAllSemaTokens() {
        // Drain pending tokens so one heavy reload satisfies a burst of requests
        while (sceKernelPollSema(ReloadSemaphore, 1) >= 0) { /* drain */ }
    }

    int reloadCams(SceSize /*args*/, void* /*argp*/) {
        for (;;) {
            // Block until a reload request arrives
            sceKernelWaitSema(ReloadSemaphore, 1, nullptr);

            // Drain extras; do one heavy reload per burst
            drainAllSemaTokens();
            sPendingReloads = 0;

            if (UNLIKELY(jumpscaring)) {
                // If jumpscare in progress, skip to keep main thread smooth
                continue;
            }

            reloaded = false;
            isMoving = true;

            // Smart incremental update: only update cameras that have changed
            // This keeps all cameras visible while updating only what's needed
            // Replaces the old system that unloaded all cameras and caused black screens
            sprite::UI::office::updateChangedCams();

            // CRITICAL: Don't play audio from background thread to prevent race conditions
            // Audio should only be played from the main thread
            // if (usingCams) {
            //     sfx::office::playMove();
            // }

            reloaded = true;
            isMoving = false;
        }

        // Not reached; keep old behavior-compatible:
        ThreadIdR = sceKernelExitThread(PspThreadStatus);
        return 0;
    }

    static void ensureReloadWorker() {
        if (ReloadSemaphore < 0) {
            // Small cap to absorb bursts; we also drain in the worker
            ReloadSemaphore = sceKernelCreateSema("reload_sema", 0, 0, 8, nullptr);
        }
        if (sReloadThread < 0) {
            const int prio  = 0x18;    // lower than main to minimize contention
            const int stack = 0x1000;  // modest stack is enough
            sReloadThread = sceKernelCreateThread("reload_thread", reloadCams, prio, stack, 0, NULL);
            if (sReloadThread >= 0) {
                sceKernelStartThread(sReloadThread, 0, NULL);
                ThreadIdR = sReloadThread; // keep compatibility with old global
            }
        }
    }

    static inline void queueReloadOnce() {
        ensureReloadWorker();
        if (sPendingReloads == 0) {
            sPendingReloads = 1;
            sceKernelSignalSema(ReloadSemaphore, 1);
        }
    }

    void setReload() {
        // CRITICAL: Fix deadlock issue - allow queuing reloads even during active reloads
        // The semaphore system handles batching multiple requests automatically
        // Only prevent queuing during jumpscares to maintain stability
        
        // Use atomic semaphore operations to prevent race conditions
        if (LIKELY(!jumpscaring)) {
            ensureReloadWorker();
            // Try to acquire semaphore non-blocking to prevent races
            if (ReloadSemaphore >= 0 && sceKernelPollSema(ReloadSemaphore, 1) >= 0) {
                // We got the semaphore, now queue the reload and signal back
                queueReloadOnce();
            } else {
                // Semaphore busy or unavailable, queue anyway as system handles batching
                queueReloadOnce();
            }
        }
    }

    // ==============================
    // Game flow
    // ==============================
    void runAiLoop() {
        // Handle Foxy attack every frame (not just every AI turn)
        if (save::whichNight > 1) {
            foxy::handleAttackFrame();
        }
        
        // Pause AI movements during Foxy attack
        // CRITICAL: Read pause state atomically to prevent race conditions
        bool foxyPaused = false;
        if (FoxyStateSemaphore >= 0) {
            sceKernelWaitSema(FoxyStateSemaphore, 1, nullptr);
            foxyPaused = state::isFoxyAttackPaused;
            sceKernelSignalSema(FoxyStateSemaphore, 1);
        } else {
            foxyPaused = state::isFoxyAttackPaused; // Fallback
        }
        if (!foxyPaused) {
            freddy::wait();
            if (save::whichNight == 2) {
                freddy::incrementDifficulty();
            }

            bonnie::wait();
            chika::wait();
            if (save::whichNight > 1) {
                foxy::wait();
            }
            
            // CRITICAL: Play move sound from main thread only to prevent race conditions
            if (usingCams && isMoving) {
                sfx::office::playMove();
            }
        }
    }

    void unloadMain() {
        // locked = true; // kept commented as in original

        state::isOffice = false;

        // Clean up office audio (comprehensive cleanup matching powerout.cpp)
        sfx::office::unloadSfx();
        ambience::office::unloadAmbience();
        ambience::office::unloadFanSound();
        call::unloadPhoneCalls();

        // Clean up office sprites and UI (comprehensive cleanup matching powerout.cpp)
        sprite::UI::office::unloadCamUi();
        sprite::UI::office::unloadPowerInfo();
        sprite::UI::office::unloadTimeInfo();
        sprite::office::unloadDoors();
        sprite::office::unloadButtons();
        sprite::UI::office::unloadCams();
        sprite::UI::office::unloadCamFlip();

        // Clean up office background sprites (comprehensive cleanup matching powerout.cpp)
        officeImage::unloadOffice1Sprites();
        officeImage::unloadOffice2Sprites();

        // CRITICAL: Clean up pre-cached assets to prevent camera system conflicts
        // This matches the comprehensive cleanup in powerout.cpp and dead.cpp
        text::preload::unloadCameraAssets();
        text::preload::unloadJumpscareAssets();
        sfx::preload::unloadCriticalAudio();

        sfx::jumpscare::loadJumpscareSound();

        initJumpscare();
    }

    void initJumpscare() {
        // Load sprites immediately (benefits from pre-caching system)
        // This ensures sprites are ready before state transition
        sprite::n_jumpscare::loadJumpscare();
        
        // Only set state after loading is complete
        state::isJumpscare = true;
    }

    // ==============================
    // Freddy
    // ==============================
    namespace freddy {
        int   levelOnes = 0;
        int   levelTenths = 0;
        float totalLevel = 0;
        int   randomNumber = 0;

        float position = 0;
        int   delay = 650; // int, not float

        bool  atDoor = false;

        inline void reloadPosition() {
            sprite::UI::office::freddyPosition = position;
            setReload();
        }

        inline void triggerJumpscare() {
            jumpscaring = true;
            sprite::n_jumpscare::whichJumpscare = 1;
            unloadMain();
        }

        inline void blockAttack() {
            atDoor = false;
            position = 6;
            reloadPosition();
        }

        inline void moveForward() {
            position += 1;
            sfx::office::playLaugh();
            reloadPosition();
        }

        inline bool shouldMoveOrAttack() {
            return (randomNumber < totalLevel || position == 6);
        }

        inline void handleAtDoor() {
            atDoor = true;
            if (!jumpscaring && !rightClosed && usingCams) {
                triggerJumpscare();
            } else if (!jumpscaring && rightClosed) {
                blockAttack();
            }
        }

        inline void decideNextMove() {
            if (position >= 6) {
                handleAtDoor();
            } else if (!usingCams && !::animatronic::isMoving) {
                moveForward();
            }
        }

        inline void evaluateMovement() {
            if (shouldMoveOrAttack() && !atDoor) {
                decideNextMove();
            }
        }

        void generateRandom() {
            randomNumber = fastRandN(20);
            evaluateMovement();
        }

        void wait() {
            if (delay > 0) {
                --delay;
            } else {
                generateRandom();
                delay = 650;
            }
        }

        void incrementDifficulty() {
            if (save::whichNight == 3) {
                totalLevel = static_cast<float>(fastRandN(3)); // Night 3 variability kept
            }
        }
    }

    // ==============================
    // Bonnie
    // ==============================
    namespace bonnie {
        int   levelOnes = 0;
        int   levelTenths = 0;
        float totalLevel = 0;
        int   randomNumber = 0;

        bool  inOtherRoom = false;
        float position = 0;
        int   delay = 389; // int

        bool  atDoor = false;

        inline void reloadPosition() {
            sprite::UI::office::bonniePosition = position;
            setReload();
        }

        inline void triggerJumpscare() {
            jumpscaring = true;
            sprite::n_jumpscare::whichJumpscare = 2;
            unloadMain();
        }

        inline void blockAttack() {
            atDoor = false;
            position = 1;
            reloadPosition();
        }

        inline void moveForward() {
            position += 1;
            reloadPosition();
        }

        inline bool shouldMoveOrAttack() {
            return (randomNumber < totalLevel || position == 6);
        }

        void moveToRoom(int targetRoom) {
            position = static_cast<float>(targetRoom);
            inOtherRoom = true;
            ::animatronic::isMoving = true;
            reloadPosition();
        }

        inline void handleAtDoor() {
            atDoor = true;
            if (!jumpscaring && !leftClosed) {
                triggerJumpscare();
            } else if (!jumpscaring && leftClosed) {
                blockAttack();
            }
        }

        inline void resetToStart() {
            if (inOtherRoom) {
                position = 2;
                inOtherRoom = false;
                reloadPosition();
            }
        }

        inline void evaluateOpportunity() {
            if (randomNumber == 2 && (position == 2 || position == 3)) {
                moveToRoom(7);
            } else if (position >= 6 && position != 7) {
                handleAtDoor();
            } else if (!::animatronic::isMoving) {
                moveForward();
            }
        }

        void processMovementDecision() {
            if (shouldMoveOrAttack() && !atDoor) {
                if (position == 7) {
                    resetToStart();
                } else {
                    evaluateOpportunity();
                }
            }
        }

        void generateRandom() {
            randomNumber = fastRandN(20);
            processMovementDecision();
        }

        void wait() {
            if (delay > 0) {
                --delay;
            } else {
                generateRandom();
                delay = 389;
            }
        }

        void incrementDifficulty() { totalLevel += 1; }
    }

    // ==============================
    // Chika
    // ==============================
    namespace chika {
        int   levelOnes = 0;
        int   levelTenths = 0;
        float totalLevel = 0;
        int   randomNumber = 0;

        bool  inOtherRoom = false;
        float position = 0;
        int   delay = 432; // int

        bool  atDoor = false;

        inline void reloadPosition() {
            sprite::UI::office::chicaPosition = position;
            setReload();
        }

        inline void triggerJumpscare() {
            jumpscaring = true;
            sprite::n_jumpscare::whichJumpscare = 3;
            unloadMain();
        }

        inline void blockAttack() {
            atDoor = false;
            position = 1;
            reloadPosition();
        }

        inline void moveForward() {
            position += 1;
            reloadPosition();
        }

        inline bool shouldMoveOrAttack() {
            return (randomNumber < totalLevel || position == 6);
        }

        void moveToRoom(int targetRoom) {
            position = static_cast<float>(targetRoom);
            inOtherRoom = true;
            ::animatronic::isMoving = true;
            reloadPosition();
        }

        inline void handleAtDoor() {
            atDoor = true;

            if (!jumpscaring && !rightClosed) {
                triggerJumpscare();
            } else if (!jumpscaring && rightClosed) {
                blockAttack();
            }
        }

        inline void resetToStart() {
            if (inOtherRoom) {
                position = 1;
                inOtherRoom = false;
                reloadPosition();
            }
        }

        inline void evaluateOpportunity() {
            if (randomNumber == 3 && position == 1) {
                moveToRoom(7);
            } else if (randomNumber == 6 && position == 2) {
                moveToRoom(9);
            } else if (position >= 6 && position != 7 && position != 8 && position != 9) {
                handleAtDoor();
            } else if (!::animatronic::isMoving) {
                moveForward();
            }
        }

        void processMovementDecision() {
            if (shouldMoveOrAttack() && !atDoor) {
                if (position == 8 || position == 9) {
                    resetToStart();
                } else {
                    evaluateOpportunity();
                }
            }
        }

        void generateRandom() {
            randomNumber = fastRandN(20);
            processMovementDecision();
        }

        void wait() {
            if (delay > 0) {
                --delay;
            } else {
                inOtherRoom = false;
                generateRandom();
                delay = 432;
            }
        }

        void incrementDifficulty() { totalLevel += 1; }
    }
    
    // ==============================
    // Foxy
    // ==============================
    namespace foxy {
        int   levelOnes = 0;
        int   levelTenths = 0;
        float totalLevel = 0;
        static int randomNumber = 0; // internal

        float position = 0;
        int   delay = 460; // int

        bool  atDoor = false;
        
        // Foxy attack system
        int   warningTimer = 0;
        bool  foxyAttackStarted = false;
        bool  knockPlayed = false;

        inline void reloadPosition() {
            sprite::UI::office::foxyPosition = position;
            setReload();
        }

        inline void triggerJumpscare() {
            jumpscaring = true;
            sprite::n_jumpscare::whichJumpscare = 4;
            unloadMain();
        }

        inline void resetToIdle() {
            atDoor = false;
            position = 0;
            warningTimer = 0;
            foxyAttackStarted = false;
            knockPlayed = false;
            
            // CRITICAL: Clear pause state atomically
            if (FoxyStateSemaphore >= 0) {
                sceKernelWaitSema(FoxyStateSemaphore, 1, nullptr);
                state::isFoxyAttackPaused = false;
                sceKernelSignalSema(FoxyStateSemaphore, 1);
            } else {
                state::isFoxyAttackPaused = false; // Fallback
            }
            
            reloadPosition();
        }

        inline void resetAttackState() {
            warningTimer = 0;
            foxyAttackStarted = false;
            knockPlayed = false;
            
            // CRITICAL: Clear pause state atomically
            if (FoxyStateSemaphore >= 0) {
                sceKernelWaitSema(FoxyStateSemaphore, 1, nullptr);
                state::isFoxyAttackPaused = false;
                sceKernelSignalSema(FoxyStateSemaphore, 1);
            } else {
                state::isFoxyAttackPaused = false; // Fallback
            }
        }

        inline void blockAttack() {
            // knock.wav is now played in handleAtDoor() when user blocks
            resetToIdle();
        }

        bool isMoving() { return ::animatronic::isMoving; }

        bool shouldMoveOrAttack() {
            return (randomNumber <= totalLevel || (position >= 3 && !isMoving() && !atDoor));
        }

        void handleAtDoor() {
            if (!atDoor) {
                atDoor = true;
                // Start the warning timer when Foxy reaches the door
                warningTimer = 0;
                foxyAttackStarted = false;
                knockPlayed = false;
                
                // CRITICAL: Set pause state atomically using semaphore synchronization
                if (FoxyStateSemaphore >= 0) {
                    sceKernelWaitSema(FoxyStateSemaphore, 1, nullptr);
                    state::isFoxyAttackPaused = true; // Pause cameras and AI during attack
                    sceKernelSignalSema(FoxyStateSemaphore, 1);
                } else {
                    state::isFoxyAttackPaused = true; // Fallback if semaphore unavailable
                }
            }
        }

        void handleAttackFrame() {
            // Only handle attack logic if Foxy is at the door
            if (atDoor && !jumpscaring) {
                // Play run.wav immediately when Foxy reaches the door
                if (!foxyAttackStarted) {
                    sfx::office::playRun();
                    foxyAttackStarted = true;
                }
                
                // Increment warning timer every frame
                warningTimer++;
                
                // Wait 1 second (60 frames at 60 FPS) before attacking
                if (warningTimer >= 60) {
                    if (!leftClosed) {
                        // User didn't block - trigger jumpscare
                        // CRITICAL: Clear pause state atomically before jumpscare
                        if (FoxyStateSemaphore >= 0) {
                            sceKernelWaitSema(FoxyStateSemaphore, 1, nullptr);
                            state::isFoxyAttackPaused = false;
                            sceKernelSignalSema(FoxyStateSemaphore, 1);
                        } else {
                            state::isFoxyAttackPaused = false; // Fallback
                        }
                        triggerJumpscare();
                    } else {
                        // User blocked - play knock.wav after run.wav
                        if (!knockPlayed) {
                            sfx::office::playKnock();
                            knockPlayed = true;
                        }
                        // Wait a bit for knock sound to play, then reset
                        if (warningTimer >= 90) { // Give knock sound time to play
                            blockAttack();
                        }
                    }
                }
            }
        }

        void handleMovement() {
            if (!isMoving() && ((position < 4 && !usingCams) || (position >= 3 && usingCams))) {
                position += 1;
                reloadPosition();
            }
        }

        void attemptMoveOrAttack() {
            if (position >= 4) {
                handleAtDoor();
            } else {
                handleMovement();
            }
        }

        void processRandomEvent() {
            if (shouldMoveOrAttack()) {
                attemptMoveOrAttack();
            }
        }

        void generateRandomEvent() {
            randomNumber = fastRandN(20);
            processRandomEvent();
        }

        void wait() {
            if (delay > 0) {
                --delay;
            } else {
                generateRandomEvent();
                delay = 460;
            }
        }

        void incrementDifficulty() {
            // No-op for now
        }
    }

    // ==============================
    // Defaults | [0] Freddy, [1] Bonnie, [2] Chica, [3] Foxy
    // ==============================
    void setDefault() {
        static const int levels[6][4] = {
            {0, 0, 0, 0},   // Night 1
            {0, 3, 1, 1},   // Night 2
            {1, 5, 9, 2},   // Night 3
            {1, 5, 9, 6},   // Night 4
            {3, 5, 9, 6},   // Night 5
            {4, 10, 12, 6}  // Night 6
        };

        switch (save::whichNight) {
            case 1: case 2: case 3: case 4: case 5: case 6: {
                const int nightIndex = save::whichNight - 1;
                freddy::totalLevel = static_cast<float>(levels[nightIndex][0]);
                bonnie::totalLevel = static_cast<float>(levels[nightIndex][1]);
                chika::totalLevel  = static_cast<float>(levels[nightIndex][2]);
                foxy::totalLevel   = static_cast<float>(levels[nightIndex][3]);
                break;
            }
            case 7:
                // Custom Night: nothing to set here
                break;
            default:
                // Invalid night: no levels set
                break;
        }

        // Worker must be ready when gameplay starts
        ensureReloadWorker();
    }

} // namespace animatronic