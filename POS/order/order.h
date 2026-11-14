#pragma once

#include <QString>
#include <QtGlobal>

namespace Pos
{
struct Order
{
    QString id;
    QString date;
    QString time;
    QString customerName;
    QString customerContact;
    QString status;
    double paymentAmount = 0;
    QString paymentMode;
    QString merchant;
};
}
