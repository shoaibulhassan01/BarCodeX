#pragma once

#include <QString>

namespace Pos
{
// Owner: Shoaib. TODO: Make a unique 8-digit order ID.
bool generateOrderId(QString &id);

// Owner: Shoaib. TODO: Make a unique 9-digit bill ID.
bool generateBillId(QString &id);
}
