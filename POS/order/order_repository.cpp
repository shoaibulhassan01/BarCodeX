#include "order_repository.h"

namespace Pos
{
// Owner: Anas. Open the file and read its order records.
bool readOrders(QString filePath, QVector<Order> &orders, QString &error)
{
    Q_UNUSED(filePath);
    Q_UNUSED(orders);
    error = QStringLiteral("TODO: Read orders from Order.txt.");
    return false;
}

// Owner: Anas. Return only details whose order ID matches.
bool readOrderDetails(QString filePath, QString orderId,
    QVector<OrderDetail> &details, QString &error)
{
    Q_UNUSED(filePath);
    Q_UNUSED(orderId);
    Q_UNUSED(details);
    error = QStringLiteral("TODO: Read order details from OrderDetail.txt.");
    return false;
}

// Owner: Anas. Save the order record and then its product detail records.
bool saveOrder(QString orderFile, QString detailFile, Order order,
    QVector<OrderDetail> details, QString &error)
{
    Q_UNUSED(orderFile);
    Q_UNUSED(detailFile);
    Q_UNUSED(order);
    Q_UNUSED(details);
    error = QStringLiteral("TODO: Save order and order details.");
    return false;
}

// Owner: Anas. Keep all other order fields unchanged.
bool changeOrderStatus(QString filePath, QString orderId, QString status,
    QString &error)
{
    Q_UNUSED(filePath);
    Q_UNUSED(orderId);
    Q_UNUSED(status);
    error = QStringLiteral("TODO: Update the order status.");
    return false;
}
}
