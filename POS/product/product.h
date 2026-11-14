#pragma once

#include <QString>
#include <QtGlobal>

namespace Pos
{
struct Product
{
    QString id;
    QString barcode;
    QString acronym;
    QString name;
    QString unit;
    double unitPrice = 0;
    double tax = 0;
    double discountPerUnit = 0;
    int quantity = 0;
    QString barcodeImage;
};
}
