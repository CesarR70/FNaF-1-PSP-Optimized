#ifndef ANIMATRONIC_HPP
#define ANIMATRONIC_HPP

#include "global.hpp"
#include "image2.hpp"
#include "audio.hpp"
#include "save.hpp"
#include "state.hpp"
#include "jumpscare.hpp"

// Optional: set to 1 if you must match std::rand() semantics/seeding.
// Default (0) uses a faster xorshift RNG internally for performance.
#ifndef ANIMATRONIC_USE_STD_RAND
#define ANIMATRONIC_USE_STD_RAND 0
#endif

namespace animatronic {
    extern bool isMoving;
    extern bool reloaded;
    extern bool usingCams;
    extern bool leftClosed;
    extern bool rightClosed;

    extern bool jumpscaring;

    extern bool locked;

    extern int ThreadIdR;
    extern SceUID ReloadSemaphore;   // persistent reload semaphore handle
    extern int PspThreadStatus;

    extern bool unloaded;

    extern int waitBeforeForceReset;

    void reset();
    void forceAnimatronicAiReset();
    int reloadCams(SceSize args, void* argp);
    void setReload();
    void runAiLoop();
    void unloadMain();
    void initJumpscare();

    // Freddy
    namespace freddy {
        extern int levelOnes;
        extern int levelTenths;
        extern float totalLevel;
        extern float position;
        extern int delay;
        extern bool atDoor;

        void wait();
        void incrementDifficulty();
        void generateRandom();
        void evaluateMovement();
        bool shouldMoveOrAttack();
        void decideNextMove();
        void handleAtDoor();
        void triggerJumpscare();
        void blockAttack();
        void moveForward();
        void reloadPosition();
    }

    // Bonnie
    namespace bonnie {
        extern int levelOnes;
        extern int levelTenths;
        extern float totalLevel;
        extern float position;
        extern int delay;
        extern bool atDoor;
        extern bool inOtherRoom;

        void wait();
        void incrementDifficulty();
        void generateRandom();
        void processMovementDecision();
        bool shouldMoveOrAttack();
        void resetToStart();
        void evaluateOpportunity();
        void moveToRoom(int targetRoom);
        void handleAtDoor();
        void triggerJumpscare();
        void blockAttack();
        void moveForward();
        void reloadPosition();
    }

    // Chika
    namespace chika {
        extern int levelOnes;
        extern int levelTenths;
        extern float totalLevel;
        extern float position;
        extern int delay;
        extern bool atDoor;
        extern bool inOtherRoom;

        void wait();
        void incrementDifficulty();
        void generateRandom();
        void processMovementDecision();
        bool shouldMoveOrAttack();
        void resetToStart();
        void evaluateOpportunity();
        void moveToRoom(int targetRoom);
        void handleAtDoor();
        void triggerJumpscare();
        void blockAttack();
        void moveForward();
        void reloadPosition();
    }

    // Foxy
    namespace foxy {
        extern int levelOnes;
        extern int levelTenths;
        extern float totalLevel;
        extern float position;
        extern int delay;
        extern bool atDoor;

        void wait();
        void incrementDifficulty();
        void generateRandomEvent();
        void processRandomEvent();
        bool shouldMoveOrAttack();
        void attemptMoveOrAttack();
        void handleAtDoor();
        void handleMovement();
        void triggerJumpscare();
        void blockAttack();
        void resetToIdle();
        void reloadPosition();

        // Returns the global animatronic::isMoving flag.
        bool isMoving();
    }

    void setDefault();
}

#endif // ANIMATRONIC_HPP