#include "global.hpp"
#include "audio.hpp"
#include "image2.hpp"
#include "animatronic.hpp"

namespace office{
    extern int wichOfficeFrame;
    extern std::string dir;
    extern std::string state;
    extern std::string buttonState;
    extern std::string doorButtonState;

    extern volatile int usageCountdown; // volatile for thread safety
    extern volatile bool leftEdge; // volatile for thread safety
    extern volatile bool rightEdge; // volatile for thread safety

    extern volatile bool leftOn; // volatile for thread safety
    extern volatile bool rightOn; // volatile for thread safety
    extern volatile bool leftClosed; // volatile for thread safety
    extern volatile bool rightClosed; // volatile for thread safety

    extern volatile bool closingLeft; // volatile for thread safety
    extern volatile bool openingLeft; // volatile for thread safety
    extern volatile bool closingRight; // volatile for thread safety
    extern volatile bool openingRight; // volatile for thread safety

    void reset();


    namespace render{
        void renderOffice();
        void renderButtons();
        void renderDoors();
    }

    namespace buttons{
        void setButtonFrame();
    }
    
    namespace main{
        void setX();
        void moveOffice();
    }

    namespace lights{
        void lights();
    }

    namespace doors{
        void doors();
        void closeLeft();
        void openLeft();
        void closeRight();
        void openRight();
    }

    //void animateLeft();
}