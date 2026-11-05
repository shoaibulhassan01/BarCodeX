# BarCodeX

---

## 📋 Project Details

| Field | Details |
|---|---|
| **Project** | BarCodeX: Barcode Generator & Scanner |
| **Language** | C++ |
| **Barcode Standard** | Code 39 |
| **Goal** | Desktop application for generating and scanning six-digit barcodes |
| **Domain** | Pattern Matching, Character Encoding, Image Processing |

The project brief requires a desktop application that accepts exactly six digits,
generates and saves a barcode image, and scans it using a laptop camera. The
team's encoder and decoder must be implemented from scratch; camera and general
image-processing libraries require instructor approval. The Day 1 encoder
accepts the basic Code 39 character set so its shared algorithm can be tested
with the agreed `ABC`, `123`, and `FAST` examples. The eventual application
workflow must still validate six-digit numeric input.

---

## 👥 Team Members

| # | Name | Main responsibility |
|---|---|---|
| 1 | Shoaib Ul Hassan (Team Leader) | Project setup and shared interfaces; Code 39 lookup table and encoder; coordination, review, merging, and final integration. |
| 2 | Umar Khan | Camera/image input and preprocessing; barcode scan-line extraction and black/white run-width measurement. |
| 3 | Zain Ul Abideen | Barcode generation; render Code 39 patterns as images with element widths and quiet zones, and export them. |
| 4 | Anas | Barcode decoding; classify run widths, match Code 39 patterns, and recover the decoded text. |
| 5 | Abdul Ahad | User interface; application screens and navigation, input/output display, and connecting the UI to backend modules. |

---

## Quick Start

Build and run the sample encoder using CMake:

```bash
cmake -S . -B build
cmake --build build
./build/barcodex
```

Run the tests:

```bash
ctest --test-dir build --output-on-failure
```

---

## 🚀 Features

### Generator
- **Code 39 Encoding:** Converts basic supported characters into nine-element patterns.
- **Start/Stop Handling:** Adds the required `*` start/stop symbols.
- **Input Validation:** Rejects empty input and unsupported characters.
- **Image Generation and Export:** Planned; the shared interface is defined.

### Scanner
- **Pattern and Run-Width Interfaces:** Shared decoder signatures are defined.
- **Camera scanning and decoding:** Planned; implementation follows after library
  approval.

---

## 🛠️ Technologies Used

| Field | Details |
|---|---|
| **Language** | C++ |
| **Barcode Standard** | Code 39 |
| **Core Concepts** | Pattern Matching, Character Encoding, Image Processing |
| **Version Control** | Git & GitHub |

---

## Project Structure

```text
BarCodeX/
├── include/barcodex/        # Shared public interfaces
├── src/                     # Application entry point and encoder
├── tests/                   # Encoder and Code 39 table tests
├── CMakeLists.txt
├── README.md
└── LICENSE
```

## Team Branch and Commit Workflow

`main` is the default branch. Each member should work on their assigned
`feature/<name>` branch, starting from the latest `main`:

```bash
git switch main
git pull origin main
git switch feature/<name>
git merge main
```

Commit focused, working changes with a concise message, then push the feature
branch and open a pull request into `main`. Review and resolve conflicts before
merging; do not commit directly to `main`.

```bash
git add <changed-files>
git commit -m "Add Code 39 encoder"
git push -u origin feature/<name>
```

The repository already has the five team feature branches. Repository access
for team members is managed in GitHub Settings → Collaborators; only a
repository owner/admin can invite accounts.

---

## 🎯 Applications

- Inventory Management
- Product Identification
- Library Management Systems
- Asset Tracking

---

## 📄 License

This project is released under the terms of the [LICENSE](LICENSE) file.
