#include "global.hpp"
#include "image2.hpp"
#include "audio.hpp"
#include "state.hpp"
#include "jumpscare.hpp"

#include "camera.hpp"
#include "office.hpp"
#include "power.hpp"

//#include "sixam.hpp"

namespace powerout {

    // Call after guSwapBuffers (deferred unload)
    void postFrame();

    // Reset state for this system
    void reset();

    namespace render {
        void renderPowerout();
    }

    namespace animate {
        void animatePowerOut();
        // Kept for compatibility; forwards to the one-shot start in .cpp
        void initJumpscare();
    }
}