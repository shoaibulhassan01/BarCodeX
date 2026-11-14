#include "order_service.h"

namespace Pos
{
// Owner: Anas. Find product by barcode, check stock, then add it to the cart.
bool addProductToOrder(QString barcode, int quantity, QString &error)
{
    Q_UNUSED(barcode);
    Q_UNUSED(quantity);
    error = QStringLiteral("TODO: Add product to order.");
    return false;
}

// Owner: Anas. Recalculate cart totals after removing the item.
bool removeProductFromOrder(QString productId, QString &error)
{
    Q_UNUSED(productId);
    error = QStringLiteral("TODO: Remove product from order.");
    return false;
}

// Owner: Anas. Save the order and reserve its stock while it is on hold.
bool holdOrder(QString &error)
{
    error = QStringLiteral("TODO: Hold the current order.");
    return false;
}

// Owner: Anas. Load the held order without reserving its stock twice.
bool resumeOrder(QString orderId, QString &error)
{
    Q_UNUSED(orderId);
    error = QStringLiteral("TODO: Resume the held order.");
    return false;
}

// Owner: Anas. Check payment, save order files and return bill rows.
bool completePayment(QString paymentMode, QString merchant,
    double amountPaid, QVector<Bill> &billRows, QString &error)
{
    Q_UNUSED(paymentMode);
    Q_UNUSED(merchant);
    Q_UNUSED(amountPaid);
    Q_UNUSED(billRows);
    error = QStringLiteral("TODO: Complete payment and generate bill.");
    return false;
}
}
