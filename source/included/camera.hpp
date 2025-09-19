#include "global.hpp"
#include "image2.hpp"
#include "audio.hpp"
#include "office.hpp"

namespace camera{
    extern volatile int whichCamera; // volatile for thread safety
    extern volatile bool isUsing; // volatile for thread safety

    extern volatile bool opening; // volatile for thread safety
    extern volatile bool closing; // volatile for thread safety

    extern std::string buttonState; // thread safety handled differently for strings

    void reset();

    namespace render{
        void renderCamFlip();
        void renderCamera();
        void renderCameraPaused();
        void renderUi();
    }

    namespace n_static{
        void renderStatic();
        void animateStatic();
    }

    namespace animation{
        
        void camera();
        void openCams();
        void closeCams();
    }

    namespace system{

        void left();
        void right();
        void up();
        void down();

        //void plus();
        //void minus();
        void setReticle();
    }
}