# 🛠️ FNaF 1 PSP — Source Code  

This repository contains the **source code** for the *Five Nights at Freddy’s PSP port*.  
Unlike the release build, this version is **not playable out of the box** and must be compiled using a PSP toolchain/SDK.  

---

## 📌 Overview
- Written in C++ and optimized for PSP hardware.  
- Requires compilation with a **PSP SDK** (e.g., [PSPSDK](https://github.com/pspdev/pspsdk) or [psptoolchain](https://github.com/pspdev/psptoolchain)).  
- 128px sprite slices (50% fewer draw calls vs version 1.5.2)
- 16-byte aligned memory allocation
- 333MHz CPU clock enabled
- Enabled Frame skip counter for intensive scenes (helps maintain 60fps)
- Corrected a potential memory leak in vram
- Race conditions removed within Audio, Camera & Jumpscare logic (prevents potential crashes when hardware is being overwhelmed)
- Tweaked some compile flags for smaller & faster C++ code
- Adjustments to audio assets (slightly smaller filesizes with noticeable improvements to audio quality)
- Less I/O hungry for smooth & stable gameplay during later levels or custom night   
- Same asset files as v1.4.5 are used.  

---

## ⚙️ Requirements
Before building, make sure you have:  
- A working **PSP toolchain** (such as `psptoolchain` or `pspsdk`)  
- `make` and a C++ compiler that works with your PSP toolchain  
- PSP assets (not included in this repo — see the release version)  

---

## 🚀 Building the Game
1. Clone this repository:  
   ```bash
   git clone https://github.com/<your-username>/FNaF-1-PSP.git
   cd FNaF-1-PSP
 2. Make sure your PSP toolchain is set in your PATH.
 3. Run make to build the game:  
make

