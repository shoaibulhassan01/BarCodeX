# BarCodeX

---

## 📋 Project Details

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

## 👥 Team Members

| # | Name | Main Responsibility |
|---|---|---|
| 1 | Shoaib Ul Hassan (Team Leader) | Project setup and shared interfaces; Code 39 lookup table and encoder; coordination, review, merging, and final integration. |
| 2 | Umar Khan | Camera input and image preprocessing; barcode scan-line extraction and black/white run-width measurement. |
| 3 | Zain Ul Abideen | Barcode generation; rendering Code 39 patterns as images with element widths and quiet zones, and exporting them. |
| 4 | Anas | Barcode decoding; classifying run widths, matching Code 39 patterns, and recovering the decoded numeric value. |
| 5 | Abdul Ahad | User interface; generation and scanning screens, navigation, input/output display, and connecting the UI to backend modules. |

---

## 🚀 Quick Start

### Prerequisites

Make sure GCC/G++, Git, and the Qt development kit are installed. Qt is the
desktop UI framework; use a Qt kit compatible with the selected C++ compiler.

Check their versions:

```bash
g++ --version
git --version
```

The repository currently contains the project folder scaffold only. A Qt-aware
build configuration and application entry point still need to be added; the
plain GCC command does not configure or link a Qt application.

Build and run instructions will be added with the Qt application entry point
and Qt-aware build configuration.

---

## ✨ Features

### Generator

- **Code 39 Encoding:** Converts numeric digits (0–9) into Code 39 barcode patterns.
- **Six-Digit Validation:** Accepts exactly six numeric digits.
- **Start/Stop Handling:** Adds the required `*` start/stop symbols.
- **Input Validation:** Rejects empty input and unsupported characters.
- **Barcode Image Generation:** Renders the encoded pattern as a barcode image.
- **Image Export:** Saves the generated barcode image.

### Scanner

- **Live Camera Scanning:** Captures barcode images using a laptop camera.
- **Image Upload:** Allows users to upload an existing barcode image.
- **Image Preprocessing:** Processes captured or uploaded images for barcode recognition.
- **Run-Width Extraction:** Measures black and white element widths.
- **Pattern Recognition:** Classifies narrow and wide elements and matches Code 39 patterns.
- **Numeric Decoding:** Retrieves the original six-digit numeric value.

---

## 🛠️ Technologies Used

| Field | Details |
|---|---|
| **Programming Language** | C++ |
| **Compiler** | GCC/G++ |
| **UI Framework** | Qt |
| **Barcode Standard** | Code 39 |
| **Core Concepts** | Pattern Matching, Character Encoding, Image Processing |
| **Version Control** | Git & GitHub |

---

## 📁 Project Structure

```text
BarCodeX/
├── include/
│   └── barcodex/
│       ├── barcode/        # Encoder and decoder interfaces
│       ├── image/          # Image processing interfaces
│       └── ui/             # Qt UI interfaces
├── src/
│   ├── barcode/            # Code 39 encoding and decoding
│   ├── image/              # Uploaded/camera image processing
│   └── ui/                 # Qt windows and UI behavior
├── resources/              # Application resources
├── tests/                  # Encoder and decoder tests
├── README.md
└── LICENSE
```

The directories are scaffolded; source files and the Qt build configuration
will be added as development progresses.

---

## 🔀 Team Branch and Commit Workflow

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

## 🎯 Applications

- Inventory Management
- Product Identification
- Library Management Systems
- Asset Tracking

---

## 📄 License

This project is released under the terms of the [LICENSE](LICENSE) file.
