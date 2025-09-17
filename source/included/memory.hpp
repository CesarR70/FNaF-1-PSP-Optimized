#ifndef MEMORY_HPP
#define MEMORY_HPP

#include "global.hpp"

namespace memory {
    // PSP Memory Budget Analysis
    // Total available: ~60MB (varies by PSP model)
    // Current usage tracking for optimization
    
    struct MemoryUsage {
        size_t audioPreCached = 0;      // Our critical audio pre-caching
        size_t audioOffice = 0;         // All office SFX
        size_t audioAmbience = 0;       // Ambience audio
        size_t audioMusic = 0;          // Menu/ending music
        size_t graphicsCamera = 0;      // Camera images
        size_t graphicsUI = 0;          // UI elements
        size_t graphicsAnimatronic = 0; // Animatronic sprites
        size_t graphicsJumpscare = 0;   // Jumpscare images
        
        size_t getTotal() const {
            return audioPreCached + audioOffice + audioAmbience + audioMusic +
                   graphicsCamera + graphicsUI + graphicsAnimatronic + graphicsJumpscare;
        }
        
        float getPercentageUsed() const {
            const size_t BUDGET_MB = 60 * 1024 * 1024; // 60MB budget
            return (float)getTotal() / BUDGET_MB * 100.0f;
        }
    };
    
    extern MemoryUsage currentUsage;
    
    // Memory tracking functions
    void trackAudioPreCached(size_t bytes);
    void trackAudioOffice(size_t bytes);
    void trackAudioAmbience(size_t bytes);
    void trackAudioMusic(size_t bytes);
    void trackGraphicsCamera(size_t bytes);
    void trackGraphicsUI(size_t bytes);
    void trackGraphicsAnimatronic(size_t bytes);
    void trackGraphicsJumpscare(size_t bytes);
    
    // Utility functions
    void printMemoryReport();
    bool isWithinBudget();
    size_t getAvailableBudget();
    
    // Pre-caching recommendations
    bool canPreCacheAdditionalAudio(size_t additionalBytes);
    bool shouldPreCacheMoreAssets();
}

#endif // MEMORY_HPP
