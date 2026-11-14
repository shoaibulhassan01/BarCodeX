#include "auth_service.h"
#include "user_repository.h"

namespace Pos
{
// Owner: Shoaib. Read Users.txt, find the name, then compare the password.
bool login(QString filePath, QString username, QString password,
    User &loggedInUser, QString &error)
{
    QVector<User> users;
    if (!readUsers(filePath, users, error))
        return false;
    if (!findUser(users, username, loggedInUser) ||
        loggedInUser.password != password)
    {
        error = QStringLiteral("Username or password is incorrect.");
        return false;
    }
    error.clear();
    return true;
}
}
