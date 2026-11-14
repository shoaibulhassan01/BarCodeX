#include "pos_ui.h"

#include <QLabel>
#include <QTabWidget>
#include <QVBoxLayout>
#include <QWidget>

namespace
{
// Owner: Abdul Ahad. Use this temporary page until each screen is implemented.
QWidget *createPlaceholderPage(const QString &title, const QString &ownerTask)
{
    QWidget *page = new QWidget;
    auto *layout = new QVBoxLayout(page);
    auto *heading = new QLabel(title, page);
    QFont headingFont = heading->font();
    headingFont.setBold(true);
    heading->setFont(headingFont);
    layout->addWidget(heading);
    layout->addWidget(new QLabel(
        QStringLiteral("Starter screen. TODO: %1").arg(ownerTask), page));
    layout->addStretch();
    return page;
}
}

namespace PosUi
{
// Owner: Abdul Ahad. Add real page widgets and role-aware navigation here.
QWidget *createPosWindow()
{
    auto *tabs = new QTabWidget;
    tabs->setWindowTitle(QStringLiteral("DukaanOS POS"));
    tabs->addTab(createPlaceholderPage(QStringLiteral("Login"),
        QStringLiteral("Build login form and show authentication errors.")),
        QStringLiteral("Login"));
    tabs->addTab(createPlaceholderPage(QStringLiteral("Dashboard"),
        QStringLiteral("Show role-specific actions and today's sales summary.")),
        QStringLiteral("Dashboard"));
    tabs->addTab(createPlaceholderPage(QStringLiteral("Scan Product"),
        QStringLiteral("Show camera preview, scan result and product details.")),
        QStringLiteral("Scan Product"));
    tabs->addTab(createPlaceholderPage(QStringLiteral("New Order"),
        QStringLiteral("Build cart, quantity controls, totals and payment actions.")),
        QStringLiteral("New Order"));
    tabs->addTab(createPlaceholderPage(QStringLiteral("Bill"),
        QStringLiteral("Display and print the completed order bill.")),
        QStringLiteral("Bill"));
    tabs->addTab(createPlaceholderPage(QStringLiteral("Held Orders"),
        QStringLiteral("List held orders and resume a selected order.")),
        QStringLiteral("Held Orders"));
    tabs->addTab(createPlaceholderPage(QStringLiteral("Orders History"),
        QStringLiteral("Search, filter and view authorized order history.")),
        QStringLiteral("Orders History"));
    tabs->addTab(createPlaceholderPage(QStringLiteral("Products"),
        QStringLiteral("Add/edit products and show low-stock products.")),
        QStringLiteral("Products"));
    return tabs;
}
}
