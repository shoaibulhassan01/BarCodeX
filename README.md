# BarCodeX

---

## Project Details

| Field | Details |
|---|---|
| **Project** | BarCodeX: Barcode Generator & Scanner |
| **Language** | C++ |
| **Barcode Standard** | Code 39 |
| **Goal** | Desktop application for generating and scanning six-digit numeric barcodes |
| **Domain** | Pattern Matching, Character Encoding, Image Processing |

BarCodeX is a C++ desktop application designed to generate and scan Code 39 barcodes containing numeric digits (0–9).

The application accepts exactly six numeric digits, generates and saves a barcode image, and supports scanning existing barcode images as well as live barcode scanning through a laptop camera.

The barcode encoder and decoder algorithms will be implemented from scratch. Camera and general image-processing libraries require instructor approval.

**Supported input:** Numeric digits (0–9).

---

## Team Members

| # | Name | Main Responsibility |
|---|---|---|
| 1 | Shoaib Ul Hassan (Team Leader) | Project setup and shared interfaces; Code 39 lookup table and encoder; coordination, review, merging, and final integration. |
| 2 | Umar Khan | Camera input and image preprocessing; barcode scan-line extraction and black/white run-width measurement. |
| 3 | Zain Ul Abideen | Barcode generation; rendering Code 39 patterns as images with element widths and quiet zones, and exporting them. |
| 4 | Anas | Barcode decoding; classifying run widths, matching Code 39 patterns, and recovering the decoded numeric value. |
| 5 | Abdul Ahad | User interface; generation and scanning screens, navigation, input/output display, and connecting the UI to backend modules. |

---

## Quick Start (Windows)

### Prerequisites

- Git
- CMake and Ninja (or the copies installed with Qt)
- Qt 6 with the **MinGW 64-bit** kit and the matching MinGW compiler

Install Qt with its Qt Widgets and Qt Multimedia components. The default Qt installer location
(`C:\Qt`) is detected automatically. For a custom location, set `QT_ROOT` to
the Qt installation folder or `QT_PREFIX_PATH` to the kit's `mingw_64` folder.
If more than one MinGW compiler is installed, set `MINGW_ROOT` to the matching
compiler folder (the one containing `bin\g++.exe`).

Clone the repository and double-click `run.bat`, or run it from Command Prompt.
The script configures and builds the app, deploys the Qt runtime beside the
executable, and starts the window. The first run requires the Qt development
kit and build tools; they are not included in the repository.

The equivalent CMake preset can be used after setting `CMAKE_PREFIX_PATH`,
`CMAKE_CXX_COMPILER`, and `CMAKE_MAKE_PROGRAM` to paths for your own Qt and
MinGW installation.

---

## Features

### Generator

- **Code 39 Encoding:** Converts numeric digits (0–9) into Code 39 barcode patterns.
- **Six-Digit Validation:** Accepts exactly six numeric digits.
- **Start/Stop Handling:** Adds the required `*` start/stop symbols.
- **Input Validation:** Rejects empty input and unsupported characters.
- **Barcode Image Generation:** Renders the encoded pattern as a barcode image.
- **Image Export:** Saves the generated barcode image.
- **Refresh:** Clears the entered number and generated barcode.

### Scanner

- **Live Camera Scanning:** Captures barcode images using a laptop camera.
- **Image Scanning:** Open an image file or paste a screenshot from the clipboard
  with **Ctrl+V** on the Scan tab to preview and decode it.
- **Refresh:** Clears the current image and scan result.
- **Image Preprocessing:** Processes captured or uploaded images for barcode recognition.
- **Run-Width Extraction:** Measures black and white element widths.
- **Pattern Recognition:** Classifies narrow and wide elements and matches Code 39 patterns.
- **Numeric Decoding:** Retrieves the original six-digit numeric value.

---

## Technologies Used

| Field | Details |
|---|---|
| **Programming Language** | C++ |
| **Compiler** | GCC/G++ |
| **UI Framework** | Qt 6 Widgets and Qt Multimedia |
| **Camera Capture** | `QCamera`, `QMediaCaptureSession`, and `QVideoSink` |
| **Barcode Standard** | Code 39 |
| **Core Concepts** | Pattern Matching, Character Encoding, Image Processing |
| **Version Control** | Git & GitHub |

---

## Project Structure

```text
BarCodeX/
├── main.cpp                # UI and main program (Ahad)
├── common/
│   └── common.h            # Shared Image class (Leader)
├── encoder/
│   ├── encoder.h           # Digits to Code 39 patterns (Leader)
│   └── encoder.cpp
├── generator/
│   ├── generator.h         # Patterns to barcode image (Zain)
│   └── generator.cpp
├── scanner/
│   ├── scanner.h           # Image to black/white run widths (Umar)
│   └── scanner.cpp
├── camera/
│   ├── camera_scanner.h    # Live camera capture and frame decoding
│   └── camera_scanner.cpp
├── decoder/
│   ├── decoder.h           # Run widths to digits (Anas)
│   └── decoder.cpp
├── tests/                  # Camera decoding regression test and barcode images
├── README.md
├── .gitignore
└── LICENSE
```

**Data flow**

- Generate: digits → `encoder` → patterns → `generator` → barcode image
- Scan: image → `scanner` → run widths → `decoder` → digits

**Shared formats**

- Pattern: 9 letters, `n` = narrow, `w` = wide (example: `nwnnwnwnn`)
- Run widths: first value is always a bar (black); narrow = 2 px, wide = 6 px
- Pixels: black = 0, white = 255

Each member works only inside their own folder. The `.h` files (function names, inputs and outputs) are changed only by the Team Leader.

The application is a Qt Widgets desktop program. Image scanning and live camera scanning use separate processing paths; the camera path captures frames with Qt Multimedia and applies adaptive thresholding before Code 39 decoding.

---

## Team Branch and Commit Workflow

`main` is the default branch. Each team member should work on their assigned `feature/<name>` branch, starting from the latest `main` branch.

### 1. Get the Latest Main Branch

```bash
git switch main
git pull origin main
```

### 2. Switch to Your Feature Branch

```bash
git switch feature/<name>
```

### 3. Merge Latest Main into Your Branch

```bash
git merge main
```

### 4. Commit Your Changes

```bash
git add <changed-files>
git commit -m "Add Code 39 encoder"
```

### 5. Push Your Feature Branch

```bash
git push -u origin feature/<name>
```

After pushing, open a pull request into `main`.

Review and resolve conflicts before merging. Do not commit directly to `main`.

The repository already has the five team feature branches. Repository access for team members is managed in GitHub Settings → Collaborators; only a repository owner/admin can invite accounts.

---

## Applications

- Inventory Management
- Product Identification
- Library Management Systems
- Asset Tracking

---

## License

This project is released under the terms of the [LICENSE](LICENSE) file.
