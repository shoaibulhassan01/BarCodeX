#include "user_repository.h"

namespace Pos
{
// Owner: Shoaib. Read and check each line in the user file.
bool readUsers(QString filePath, QVector<User> &users, QString &error)
{
    Q_UNUSED(filePath);
    Q_UNUSED(users);
    error = QStringLiteral("TODO: Read users from Users.txt.");
    return false;
}

// Owner: Shoaib. Search the loaded users for the matching username.
bool findUser(QVector<User> users, QString username, User &user)
{
    for (User savedUser : users)
    {
        if (savedUser.username == username)
        {
            user = savedUser;
            return true;
        }
    }
    return false;
}
}
