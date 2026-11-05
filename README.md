# 📊 BarCodeX

A C++ application that generates and decodes **Code 39 barcodes**. It converts alphanumeric text into a barcode and reads a barcode image back into the original text, with the encoding and decoding logic implemented from scratch.

---

## 📋 Project Details

| Field | Details |
|---|---|
| **Project** | BarCodeX: Code 39 Generator & Scanner |
| **Language** | C++ |
| **Barcode Standard** | Code 39 |
| **Domain** | Pattern Matching, Character Encoding, Image Processing |

---

## 👥 Team Members

| # | Name |
|---|---|
| 1 | Shoaib Ul Hassan |
| 2 | Umar Khan |
| 3 | Zain Ul Abideen |
| 4 | Anas |
| 5 | Abdul Ahad |

---

## ⚡ Quick Start

1. Clone the repository:

   ```bash
   git clone https://github.com/<your-username>/BarCodeX.git
   cd BarCodeX
   ```

2. Build the project:

   ```bash
   g++ -std=c++17 -Iinclude src/*.cpp -o barcodex
   ```

3. Run it:

   ```bash
   ./barcodex
   ```

> 💡 Replace `<your-username>` with your GitHub username. Adjust the build command if you use Visual Studio, CMake, or Make.

---

## 🚀 Features

### 🖨️ Generator
- **Code 39 Encoding:** Converts supported characters into their barcode patterns.
- **Barcode Generation:** Produces a visual barcode using narrow and wide bars.
- **Start/Stop Handling:** Automatically wraps the data in the `*` character.
- **Input Validation:** Rejects characters that Code 39 does not support.

### 🔍 Scanner
- **Barcode Decoding:** Reads a barcode image and recovers the original text.
- **Pattern Matching:** Matches detected patterns against the Code 39 character table.
- **Start/Stop Recognition:** Verifies the `*` markers before accepting a result.

---

## 🛠️ Technologies Used

| Field | Details |
|---|---|
| **Language** | C++ |
| **Barcode Standard** | Code 39 |
| **Core Concepts** | Pattern Matching, Character Encoding, Image Processing |
| **Version Control** | Git & GitHub |

---

## 📁 Project Structure

```text
BarCodeX/
├── src/        # Source files
├── include/    # Header files
├── samples/    # Sample barcode images
├── README.md
└── LICENSE
```

---

## 🎯 Applications

- Inventory Management
- Product Identification
- Library Management Systems
- Asset Tracking

---

## 📄 License

This project is released under the terms of the [LICENSE](LICENSE) file.
