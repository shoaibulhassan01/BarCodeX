#pragma once

#include <QString>
#include <QStringList>

namespace Pos
{
// Owner: Umar Khan. TODO: Split one record such as "P1 123 APL#".
bool splitRecord(QString line, QStringList &fields);

// Owner: Umar Khan. TODO: Join fields and add the ending #.
QString makeRecord(QStringList fields);
}
