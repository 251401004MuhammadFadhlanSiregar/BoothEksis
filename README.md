# 🚀 Space Adventure Photobooth
**Bootheksis** — A kawaii space-themed photo booth app built with Qt5 C++.

---

## Features
- **Take Photos** — Live camera preview with a 3-second countdown per shot
- **Upload Photos** — Select 2 images from disk to use as your photo strip
- **Sticker Editor** — Drag-and-drop 5 kawaii space stickers onto your strip
- **Download** — Save your completed strip as a hi-res PNG (2× upscaled)
- **Share** — Copy strip to clipboard and paste anywhere

---

## Requirements
| Requirement | Version |
|---|---|
| Qt | ≥ 5.12 |
| C++ | C++17 |
| Modules | `core gui widgets multimedia multimediawidgets` |
| Compiler | GCC / Clang / MSVC (any with C++17) |

---

## Build Instructions

### Linux / macOS
```bash
cd photobooth
qmake photobooth.pro
make -j$(nproc)
./SpaceAdventurePhotobooth
```

### Windows (Qt Creator)
1. Open `photobooth.pro` in Qt Creator
2. Select a Qt 5.x kit with Multimedia support
3. Press **Ctrl+R** to build & run

### CMake (optional)
If you prefer CMake, a `CMakeLists.txt` stub:
```cmake
cmake_minimum_required(VERSION 3.16)
project(SpaceAdventurePhotobooth)
set(CMAKE_CXX_STANDARD 17)
find_package(Qt5 REQUIRED COMPONENTS Widgets Multimedia MultimediaWidgets)
file(GLOB SRC src/*.cpp)
add_executable(${PROJECT_NAME} ${SRC})
target_link_libraries(${PROJECT_NAME} Qt5::Widgets Qt5::Multimedia Qt5::MultimediaWidgets)
```

---

## Project Structure
```
photobooth/
├── photobooth.pro      ← qmake build file
├── README.md
└── src/
    ├── main.cpp            ← Entry point
    ├── mainwindow.h/.cpp   ← Page navigation (QStackedWidget)
    ├── menupage.h/.cpp     ← Welcome screen with space theme
    ├── camerapage.h/.cpp   ← Camera preview + countdown capture
    └── stickerpage.h/.cpp  ← Sticker canvas + export
```

---

## Screens
| Screen | Description |
|---|---|
| **Menu** | Purple lavender bg, "SPACE ADVENTURE" title, Take/Upload buttons |
| **Camera** | Live viewfinder inside the photo strip frame, 3 s countdown per slot |
| **Sticker** | Draggable space stickers (alien, meteor, earth, saturn, rocket) |

---

## Notes
- If **no camera is detected**, the app falls back to a colored placeholder and
  shows a "No Camera – Upload instead" button automatically.
- Stickers are **drawn with QPainter** — no external image assets needed.
- The rendered export is **2× the canvas size** (1000×1280 px) for crisp prints.
- **Share** copies the pixmap to the system clipboard; paste with Ctrl+V.
