// -*- C++ -*-

#include "layout.hpp"

#include <QApplication>
#include <QWidget>
#include <QSettings>
#include <QFile>
#include <QFileDialog>
#include <QDir>

static const QString app_name("Intrinsics Guide");

int main(int argc, char* argv[])
{
    QApplication app(argc, argv);
    QApplication::setApplicationName(app_name);
    QApplication::setOrganizationName("Roman Saldygashev");

    QString data_path = QApplication::applicationDirPath() + "/data-3.5.2.xml";
    QSettings settings;
    QVariant data_path_v = settings.value("data");
    if (data_path_v.canConvert<QString>())
        data_path = data_path_v.toString();

    QFile data_file(data_path);
    if (!data_file.exists())
    {
        data_path = QFileDialog::getOpenFileName(nullptr, "Open Intrinsics Data",
                                                 QDir::homePath(),
                                                 "XML documents (data-*.xml)");
        data_file.setFileName(data_path);
        settings.setValue("data", data_path);
    }

    QWidget window;
    window.setLayout(make_layout());
    window.setWindowTitle(app_name);
    window.show();

    return app.exec();
}
