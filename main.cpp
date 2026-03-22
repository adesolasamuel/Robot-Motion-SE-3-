#include <QApplication>
#include <QPalette>
#include <QStyleFactory>
#include "gui/mainwindow.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    // Dark Fusion theme
    app.setStyle(QStyleFactory::create("Fusion"));

    QPalette dark;
    dark.setColor(QPalette::Window,          QColor(43, 43, 43));
    dark.setColor(QPalette::WindowText,      Qt::white);
    dark.setColor(QPalette::Base,            QColor(30, 30, 30));
    dark.setColor(QPalette::AlternateBase,   QColor(51, 51, 51));
    dark.setColor(QPalette::ToolTipBase,     Qt::white);
    dark.setColor(QPalette::ToolTipText,     Qt::white);
    dark.setColor(QPalette::Text,            Qt::white);
    dark.setColor(QPalette::Button,          QColor(51, 51, 51));
    dark.setColor(QPalette::ButtonText,      Qt::white);
    dark.setColor(QPalette::BrightText,      Qt::red);
    dark.setColor(QPalette::Link,            QColor(0, 188, 212));
    dark.setColor(QPalette::Highlight,       QColor(0, 188, 212));
    dark.setColor(QPalette::HighlightedText, Qt::black);
    app.setPalette(dark);

    MainWindow window;
    window.show();

    return app.exec();
}
