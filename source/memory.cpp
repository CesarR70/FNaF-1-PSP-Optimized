#include "included/memory.hpp"
#include "included/global.hpp"
#include <cstdio>

namespace memory {
    
    // Memory budget tracking for PSP optimization
    // PSP has ~60MB total RAM, we need to track usage carefully
    
    // CRITICAL: Make these static to avoid race conditions
    static size_t totalGraphicsMemory = 0;
    static size_t totalAudioMemory = 0;
    static size_t totalSystemMemory = 0;
    
    // OPTIMIZED: Increased limits for better performance while maintaining stability
    const size_t MAX_GRAPHICS_MEMORY = 45 * 1024 * 1024; // 45MB for graphics (increased for more pre-caching)
    const size_t MAX_AUDIO_MEMORY = 8 * 1024 * 1024;     // 8MB for audio (kept same)
    const size_t MAX_SYSTEM_MEMORY = 5 * 1024 * 1024;    // 5MB for system (increased for stability)
    const size_t MAX_TOTAL_MEMORY = 58 * 1024 * 1024;    // 58MB total (6MB OS headroom - still safe)
    
    void trackGraphicsCamera(size_t bytes) {
        totalGraphicsMemory += bytes;
        DEBUG_PRINTF("Memory: Graphics Camera +%zu bytes (Total: %zu MB)\n", 
               bytes, totalGraphicsMemory / (1024 * 1024));
        
        // Warning if approaching limit
        if (totalGraphicsMemory > MAX_GRAPHICS_MEMORY * 0.9) {
            DEBUG_PRINTF("⚠️  WARNING: Graphics memory at %.1f%% of limit!\n", 
                   (float)totalGraphicsMemory / MAX_GRAPHICS_MEMORY * 100.0f);
        }
    }
    
    void trackGraphicsJumpscare(size_t bytes) {
        totalGraphicsMemory += bytes;
        DEBUG_PRINTF("Memory: Graphics Jumpscare +%zu bytes (Total: %zu MB)\n", 
               bytes, totalGraphicsMemory / (1024 * 1024));
        
        // Warning if approaching limit
        if (totalGraphicsMemory > MAX_GRAPHICS_MEMORY * 0.9) {
            DEBUG_PRINTF("⚠️  WARNING: Graphics memory at %.1f%% of limit!\n", 
                   (float)totalGraphicsMemory / MAX_GRAPHICS_MEMORY * 100.0f);
        }
    }
    
    void trackAudio(size_t bytes) {
        totalAudioMemory += bytes;
        DEBUG_PRINTF("Memory: Audio +%zu bytes (Total: %zu MB)\n", 
               bytes, totalAudioMemory / (1024 * 1024));
        
        // Warning if approaching limit
        if (totalAudioMemory > MAX_AUDIO_MEMORY * 0.9) {
            DEBUG_PRINTF("⚠️  WARNING: Audio memory at %.1f%% of limit!\n", 
                   (float)totalAudioMemory / MAX_AUDIO_MEMORY * 100.0f);
        }
    }
    
    void trackSystem(size_t bytes) {
        totalSystemMemory += bytes;
        DEBUG_PRINTF("Memory: System +%zu bytes (Total: %zu MB)\n", 
               bytes, totalSystemMemory / (1024 * 1024));
        
        // Warning if approaching limit
        if (totalSystemMemory > MAX_SYSTEM_MEMORY * 0.9) {
            DEBUG_PRINTF("⚠️  WARNING: System memory at %.1f%% of limit!\n", 
                   (float)totalSystemMemory / MAX_SYSTEM_MEMORY * 100.0f);
        }
    }
    
    // CRITICAL: Add untracking functions to prevent memory accumulation
    void untrackGraphicsCamera(size_t bytes) {
        if (totalGraphicsMemory >= bytes) {
            totalGraphicsMemory -= bytes;
            DEBUG_PRINTF("Memory: Graphics Camera -%zu bytes (Total: %zu MB)\n", 
                   bytes, totalGraphicsMemory / (1024 * 1024));
        } else {
            DEBUG_PRINTF("⚠️  ERROR: Attempted to untrack %zu bytes but only %zu bytes allocated!\n",
                   bytes, totalGraphicsMemory);
            DEBUG_PRINTF("⚠️  This indicates a memory tracking bug - investigate immediately!\n");
            // Don't reset to 0 - keep the incorrect value visible for debugging
            // totalGraphicsMemory = 0; // REMOVED: This was masking real bugs
        }
    }
    
    void untrackGraphicsJumpscare(size_t bytes) {
        if (totalGraphicsMemory >= bytes) {
            totalGraphicsMemory -= bytes;
            DEBUG_PRINTF("Memory: Graphics Jumpscare -%zu bytes (Total: %zu MB)\n", 
                   bytes, totalGraphicsMemory / (1024 * 1024));
        } else {
            DEBUG_PRINTF("⚠️  ERROR: Attempted to untrack %zu bytes but only %zu bytes allocated!\n",
                   bytes, totalGraphicsMemory);
            DEBUG_PRINTF("⚠️  This indicates a memory tracking bug - investigate immediately!\n");
            // Don't reset to 0 - keep the incorrect value visible for debugging
        }
    }
    
    void untrackAudio(size_t bytes) {
        if (totalAudioMemory >= bytes) {
            totalAudioMemory -= bytes;
            DEBUG_PRINTF("Memory: Audio -%zu bytes (Total: %zu MB)\n", 
                   bytes, totalAudioMemory / (1024 * 1024));
        } else {
            DEBUG_PRINTF("⚠️  ERROR: Attempted to untrack %zu bytes but only %zu bytes allocated!\n",
                   bytes, totalAudioMemory);
            DEBUG_PRINTF("⚠️  This indicates a memory tracking bug - investigate immediately!\n");
            // Don't reset to 0 - keep the incorrect value visible for debugging
        }
    }
    
    void untrackSystem(size_t bytes) {
        if (totalSystemMemory >= bytes) {
            totalSystemMemory -= bytes;
            DEBUG_PRINTF("Memory: System -%zu bytes (Total: %zu MB)\n", 
                   bytes, totalSystemMemory / (1024 * 1024));
        } else {
            DEBUG_PRINTF("⚠️  ERROR: Attempted to untrack %zu bytes but only %zu bytes allocated!\n",
                   bytes, totalSystemMemory);
            DEBUG_PRINTF("⚠️  This indicates a memory tracking bug - investigate immediately!\n");
            // Don't reset to 0 - keep the incorrect value visible for debugging
        }
    }
    
    bool isMemoryBudgetOK() {
        size_t totalUsed = totalGraphicsMemory + totalAudioMemory + totalSystemMemory;
        
        // Warning if approaching total limit
        if (totalUsed > MAX_TOTAL_MEMORY * 0.9) {
            DEBUG_PRINTF("⚠️  WARNING: Total memory at %.1f%% of limit! (%zu MB / %zu MB)\n", 
                   (float)totalUsed / MAX_TOTAL_MEMORY * 100.0f,
                   totalUsed / (1024 * 1024),
                   MAX_TOTAL_MEMORY / (1024 * 1024));
        }
        
        return (totalGraphicsMemory < MAX_GRAPHICS_MEMORY && 
                totalAudioMemory < MAX_AUDIO_MEMORY && 
                totalSystemMemory < MAX_SYSTEM_MEMORY &&
                totalUsed < MAX_TOTAL_MEMORY);
    }
    
    void printMemoryReport() {
        DEBUG_PRINTF("\n=== MEMORY BUDGET REPORT ===\n");
        DEBUG_PRINTF("Graphics: %zu MB / %zu MB (%.1f%%)\n", 
               totalGraphicsMemory / (1024 * 1024), 
               MAX_GRAPHICS_MEMORY / (1024 * 1024),
               (float)totalGraphicsMemory / MAX_GRAPHICS_MEMORY * 100.0f);
        DEBUG_PRINTF("Audio: %zu MB / %zu MB (%.1f%%)\n", 
               totalAudioMemory / (1024 * 1024), 
               MAX_AUDIO_MEMORY / (1024 * 1024),
               (float)totalAudioMemory / MAX_AUDIO_MEMORY * 100.0f);
        DEBUG_PRINTF("System: %zu MB / %zu MB (%.1f%%)\n", 
               totalSystemMemory / (1024 * 1024), 
               MAX_SYSTEM_MEMORY / (1024 * 1024),
               (float)totalSystemMemory / MAX_SYSTEM_MEMORY * 100.0f);
        DEBUG_PRINTF("Total: %zu MB / %zu MB (%.1f%%) [64MB PSP Total]\n", 
               (totalGraphicsMemory + totalAudioMemory + totalSystemMemory) / (1024 * 1024),
               MAX_TOTAL_MEMORY / (1024 * 1024),
               (float)(totalGraphicsMemory + totalAudioMemory + totalSystemMemory) / MAX_TOTAL_MEMORY * 100.0f);
        DEBUG_PRINTF("============================\n\n");
    }
}