#include "product_repository.h"

namespace Pos
{
// Owner: Umar Khan. Read one line at a time and parse each product record.
bool readProducts(QString filePath, QVector<Product> &products, QString &error)
{
    Q_UNUSED(products);
    Q_UNUSED(filePath);
    error = QStringLiteral("TODO: Read products from Product.txt.");
    return false;
}

// Owner: Umar Khan. Write each product as one space-separated line ending in #.
bool saveProducts(QString filePath, QVector<Product> products, QString &error)
{
    Q_UNUSED(products);
    Q_UNUSED(filePath);
    error = QStringLiteral("TODO: Save products to Product.txt.");
    return false;
}
}
