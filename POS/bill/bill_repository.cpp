#include "bill_repository.h"

namespace Pos
{
// Owner: Anas. Rows with the same bill ID belong to one bill.
bool readBill(QString filePath, QString billId, QVector<Bill> &rows,
    QString &error)
{
    Q_UNUSED(filePath);
    Q_UNUSED(billId);
    Q_UNUSED(rows);
    error = QStringLiteral("TODO: Read bill rows from Bill.txt.");
    return false;
}

// Owner: Anas. Add one line to Bill.txt for every product.
bool saveBill(QString filePath, QVector<Bill> rows, QString &error)
{
    Q_UNUSED(filePath);
    Q_UNUSED(rows);
    error = QStringLiteral("TODO: Save bill rows to Bill.txt.");
    return false;
}
}
