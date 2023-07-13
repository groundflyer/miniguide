#include <QApplication>
#include <QWidget>

#include "layout.hpp"

int main(int argc, char* argv[])
{
    QApplication app(argc, argv);

    QWidget window;
    window.setLayout(make_layout());
    window.setWindowTitle("IntelⓇ Intrinsics Guide");
    window.show();

    return app.exec();
}
