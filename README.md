# DukaanOS

A barcode based Point of Sale (POS) system built with C++ and Qt. Products are identified by camera scanning Code 39 barcodes, using an encoder and decoder written from scratch.

## Features

- Login with Cashier and Manager roles
- Camera based barcode scanning to view product details
- Order recording with stock check, hold and payment (cash, card, wallet)
- Bill generation with order, tax and discount totals
- Text file storage for products, orders, order details and bills
- Code 39 barcode generator and scanner (six-digit numeric barcodes)

## Pages

| Page | Access | Purpose |
|---|---|---|
| Login | Everyone | Sign in with username and password |
| Dashboard | Cashier, Manager | Role based actions and today's summary |
| Scan Product | Cashier, Manager | Scan a barcode and view product details |
| New Order | Cashier | Scan items, set payment, hold the order or generate the bill |
| Bill | Cashier | View the generated bill |
| Held Orders | Cashier | Resume on-hold orders |
| Orders History | Cashier, Manager | Search and view orders; Manager can archive |
| Products | Manager | View, add and edit products |

## Team

| Name | Responsibility |
|---|---|
| Shoaib Ul Hassan (Team Leader) | Shared interfaces, models, encoder, ID generation, integration |
| Umar Khan | Camera input, image preprocessing, scan-line extraction |
| Zain Ul Abideen | Barcode generation, file repositories |
| Anas | Barcode decoding, services and payment modes |
| Abdul Ahad | User interface and navigation |

## Getting Started (Windows)

**Requirements:** Git, CMake, Ninja, and Qt 6 (Widgets and Multimedia) with the MinGW 64-bit kit.

1. Clone the repository.
2. Run `run.bat`. It configures, builds, deploys the Qt runtime and starts the app.

If Qt is not in `C:\Qt`, set `QT_ROOT` (Qt folder) or `QT_PREFIX_PATH` (the kit's `mingw_64` folder). If several MinGW compilers are installed, set `MINGW_ROOT` to the matching one.

To build an installer, run `run.bat --package`, then compile `DukaanOS.iss` in Inno Setup to create `dist\DukaanOS-Setup.exe`.

## Project Structure

```
DukaanOS/
├── main.cpp
├── common/          # Shared Image class
├── encoder/         # Digits to Code 39 patterns
├── generator/       # Patterns to barcode image
├── scanner/         # Image to run widths
├── camera/          # Live camera capture
├── decoder/         # Run widths to digits
├── models/          # Product, Order, OrderDetail, Bill, User
├── repositories/    # Text file read/write
├── services/        # Product, order, billing, auth, payment
├── ui/              # Pages and widgets
├── data/            # Product.txt, Order.txt, OrderDetail.txt, Bill.txt, Users.txt
├── barcodes/        # Product barcode images
└── tests/
```

Pages only handle display and input, services hold the business rules, and repositories only read and write files. Each member works only inside their own folder, and shared `.h` files are changed only by the Team Leader.

## Data Files

One record per line, fields separated by a space, each line ending with `#`. Names use `_` instead of spaces, and an empty optional field is written as `-`.

| File | Fields |
|---|---|
| Product.txt | id, barcode, acronym (max 8), name, unit, unit price, tax (PKR), discount per unit, available quantity, barcode image path |
| Order.txt | order id (8 digits), date, time, customer name, contact, status, payment amount, payment mode, merchant |
| OrderDetail.txt | order id, product id, barcode, quantity |
| Bill.txt | bill id (9 digits), customer name, order id, acronym, unit price, discount per unit, quantity, total without tax, total tax, total discount, total payable, status |
| Users.txt | username, password, role |

Order status: `in-progress`, `on-hold`, `paid`, `archived`. Payment mode: `cash`, `card`, `wallet`.

## Workflow

Work on your own `feature/<name>` branch, merge the latest `main` into it, push, and open a pull request. Do not commit directly to `main`.

```
git switch main && git pull origin main
git switch feature/<name>
git merge main
git add <files> && git commit -m "message"
git push -u origin feature/<name>
```

## License

Released under the terms of the `LICENSE` file.
