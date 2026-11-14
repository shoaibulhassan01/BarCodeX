#include "record_codec.h"

namespace Pos
{
// Owner: Umar Khan. Remove #, then split the rest at spaces.
bool splitRecord(QString line, QStringList &fields)
{
    line = line.trimmed();
    if (!line.endsWith('#'))
        return false;

    line.chop(1);
    fields = line.split(' ', Qt::SkipEmptyParts);
    return true;
}

// Owner: Umar Khan. Names with spaces must use underscores before saving.
QString makeRecord(QStringList fields)
{
    return fields.join(' ') + '#';
}
}
