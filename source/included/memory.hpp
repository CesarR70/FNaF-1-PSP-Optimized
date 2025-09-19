#pragma once

#include <cstddef>

namespace memory {
    
    // Memory budget tracking for PSP optimization
    // PSP has ~60MB total RAM, we need to track usage carefully
    
    // Memory tracking variables are internal to memory.cpp
    // (declared as static to avoid race conditions)
    
    // Tracking functions
    void trackGraphicsCamera(size_t bytes);
    void trackGraphicsJumpscare(size_t bytes);
    void trackAudio(size_t bytes);
    void trackSystem(size_t bytes);
    
    // CRITICAL: Untracking functions to prevent memory accumulation
    void untrackGraphicsCamera(size_t bytes);
    void untrackGraphicsJumpscare(size_t bytes);
    void untrackAudio(size_t bytes);
    void untrackSystem(size_t bytes);
    
    // Utility functions
    bool isMemoryBudgetOK();
    void printMemoryReport();
}