#pragma once

#include "bill.h"

#include "../order/order.h"
#include "../product/product.h"

namespace Pos
{
// Owner: Anas. TODO: Calculate and fill one product row for the bill.
bool calculateBillLine(QString billId, Order order, Product product,
    int quantity, Bill &bill, QString &error);
}
