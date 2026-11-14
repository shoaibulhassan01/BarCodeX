#include "bill_service.h"

namespace Pos
{
// Owner: Anas. Tax and discount are multiplied by the number of units.
bool calculateBillLine(QString billId, Order order, Product product,
    int quantity, Bill &bill, QString &error)
{
    if (quantity <= 0)
    {
        error = QStringLiteral("Quantity must be greater than zero.");
        return false;
    }

    double subtotal = product.unitPrice * quantity;
    double tax = product.tax * quantity;
    double discount = product.discountPerUnit * quantity;
    if (discount > subtotal)
    {
        error = QStringLiteral("Discount cannot be greater than the price.");
        return false;
    }

    bill.id = billId;
    bill.customerName = order.customerName;
    bill.orderId = order.id;
    bill.productAcronym = product.acronym;
    bill.unitPrice = product.unitPrice;
    bill.discountPerUnit = product.discountPerUnit;
    bill.quantity = quantity;
    bill.totalWithoutTax = subtotal;
    bill.totalTax = tax;
    bill.totalDiscount = discount;
    bill.totalPayable = subtotal + tax - discount;
    bill.paymentStatus = order.status;
    error.clear();
    return true;
}
}
