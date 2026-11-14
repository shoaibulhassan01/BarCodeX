#pragma once

#include "user.h"

namespace Pos
{
// Owner: Shoaib. Check the username and password and return the user's role.
bool login(QString filePath, QString username, QString password,
    User &loggedInUser, QString &error);
}
