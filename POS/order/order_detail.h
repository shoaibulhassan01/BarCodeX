#pragma once

#include <QString>

namespace Pos
{
struct OrderDetail
{
    QString orderId;
    QString productId;
    QString barcode;
    int quantity = 0;
};
}
