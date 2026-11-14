#pragma once

#include "product.h"

#include <QString>
#include <QVector>

namespace Pos
{
// Owner: Umar Khan. TODO: Read Product.txt and fill the list.
bool readProducts(QString filePath, QVector<Product> &products, QString &error);

// Owner: Umar Khan. TODO: Save all products back to Product.txt.
bool saveProducts(QString filePath, QVector<Product> products, QString &error);
}
