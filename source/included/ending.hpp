#include "global.hpp"
#include "image2.hpp"
#include "audio.hpp"
#include "menu.hpp"
#include "save.hpp"

namespace ending {

    // Call after guSwapBuffers (deferred transition)
    void postFrame();

    // Reset the ending state/timer
    void reset();

    namespace render {
        void renderEnding();
    }

    namespace wait {
        void waitForFrames();
        // Now acts as a scheduler; actual work done in postFrame()
        void initMenu();
    }
}