# 🛠️ FNaF 1 PSP — Source Code  

This repository contains the **source code** for the *Five Nights at Freddy’s PSP port*.  
Unlike the release build, this version is **not playable out of the box** and must be compiled using a PSP toolchain/SDK.  

---

## 📌 Overview
- Written in C++ and optimized for PSP hardware.  
- Requires compilation with a **PSP SDK** (e.g., [PSPSDK](https://github.com/pspdev/pspsdk) or [psptoolchain](https://github.com/pspdev/psptoolchain)).  
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
   git clone https://github.com/CesarR70/FNaF-1-PSP-Optimized
   cd FNaF-1-PSP-Optimized
 2. Make sure your PSP toolchain is set in your PATH.
 3. Run make to build the game:  
make

