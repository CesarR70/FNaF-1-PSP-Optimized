#include "included/memory.hpp"
#include <cstdio>

namespace memory {
    
    MemoryUsage currentUsage;
    
    void trackAudioPreCached(size_t bytes) {
        currentUsage.audioPreCached = bytes;
    }
    
    void trackAudioOffice(size_t bytes) {
        currentUsage.audioOffice = bytes;
    }
    
    void trackAudioAmbience(size_t bytes) {
        currentUsage.audioAmbience = bytes;
    }
    
    void trackAudioMusic(size_t bytes) {
        currentUsage.audioMusic = bytes;
    }
    
    void trackGraphicsCamera(size_t bytes) {
        currentUsage.graphicsCamera = bytes;
    }
    
    void trackGraphicsUI(size_t bytes) {
        currentUsage.graphicsUI = bytes;
    }
    
    void trackGraphicsAnimatronic(size_t bytes) {
        currentUsage.graphicsAnimatronic = bytes;
    }
    
    void trackGraphicsJumpscare(size_t bytes) {
        currentUsage.graphicsJumpscare = bytes;
    }
    
    void printMemoryReport() {
        printf("=== PSP MEMORY USAGE REPORT ===\n");
        printf("Audio Pre-cached: %.2f MB (%.1f%%)\n", 
               currentUsage.audioPreCached / (1024.0f * 1024.0f),
               (float)currentUsage.audioPreCached / (60 * 1024 * 1024) * 100.0f);
        printf("Audio Office: %.2f MB (%.1f%%)\n", 
               currentUsage.audioOffice / (1024.0f * 1024.0f),
               (float)currentUsage.audioOffice / (60 * 1024 * 1024) * 100.0f);
        printf("Audio Ambience: %.2f MB (%.1f%%)\n", 
               currentUsage.audioAmbience / (1024.0f * 1024.0f),
               (float)currentUsage.audioAmbience / (60 * 1024 * 1024) * 100.0f);
        printf("Audio Music: %.2f MB (%.1f%%)\n", 
               currentUsage.audioMusic / (1024.0f * 1024.0f),
               (float)currentUsage.audioMusic / (60 * 1024 * 1024) * 100.0f);
        printf("Graphics Camera: %.2f MB (%.1f%%)\n", 
               currentUsage.graphicsCamera / (1024.0f * 1024.0f),
               (float)currentUsage.graphicsCamera / (60 * 1024 * 1024) * 100.0f);
        printf("Graphics UI: %.2f MB (%.1f%%)\n", 
               currentUsage.graphicsUI / (1024.0f * 1024.0f),
               (float)currentUsage.graphicsUI / (60 * 1024 * 1024) * 100.0f);
        printf("Graphics Animatronic: %.2f MB (%.1f%%)\n", 
               currentUsage.graphicsAnimatronic / (1024.0f * 1024.0f),
               (float)currentUsage.graphicsAnimatronic / (60 * 1024 * 1024) * 100.0f);
        printf("Graphics Jumpscare: %.2f MB (%.1f%%)\n", 
               currentUsage.graphicsJumpscare / (1024.0f * 1024.0f),
               (float)currentUsage.graphicsJumpscare / (60 * 1024 * 1024) * 100.0f);
        printf("TOTAL: %.2f MB (%.1f%% of 60MB budget)\n", 
               currentUsage.getTotal() / (1024.0f * 1024.0f),
               currentUsage.getPercentageUsed());
        printf("Available: %.2f MB\n", getAvailableBudget() / (1024.0f * 1024.0f));
    }
    
    bool isWithinBudget() {
        const size_t BUDGET_MB = 60 * 1024 * 1024; // 60MB budget
        return currentUsage.getTotal() < BUDGET_MB;
    }
    
    size_t getAvailableBudget() {
        const size_t BUDGET_MB = 60 * 1024 * 1024; // 60MB budget
        size_t used = currentUsage.getTotal();
        return (used < BUDGET_MB) ? (BUDGET_MB - used) : 0;
    }
    
    bool canPreCacheAdditionalAudio(size_t additionalBytes) {
        return (currentUsage.audioPreCached + additionalBytes) < (5 * 1024 * 1024); // 5MB limit for audio
    }
    
    bool shouldPreCacheMoreAssets() {
        return currentUsage.getPercentageUsed() < 70.0f; // Pre-cache if under 70% usage
    }
}
