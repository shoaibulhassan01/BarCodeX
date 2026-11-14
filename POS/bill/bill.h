#pragma once

#include <QString>
#include <QtGlobal>

namespace Pos
{
struct Bill
{
    QString id;
    QString customerName;
    QString orderId;
    QString productAcronym;
    double unitPrice = 0;
    double discountPerUnit = 0;
    int quantity = 0;
    double totalWithoutTax = 0;
    double totalTax = 0;
    double totalDiscount = 0;
    double totalPayable = 0;
    QString paymentStatus;
};
}
