// -*- C++ -*-

#include "layout.hpp"
#include "parser.hpp"

#include <QApplication>
#include <QWidget>
#include <QSettings>
#include <QFile>
#include <QFileDialog>
#include <QDir>

#include <QDebug>

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

    const ParseData data = parse_doc(&data_file);

    qDebug() << "version: " << data.version;
    qDebug() << "date: " << data.date;

    for(const Intrinsic& i : data.intrinsics)
    {
        qDebug() << i.name << '\t' << i.tech;
        qDebug() << "\tCategory:" << i.category;
        qDebug() << "\tCPUID Flags: " << i.cpuids.join(" + ");
        qDebug() << "\tDescription: " << i.description;
        qDebug() << "\tOperation: " << i.operation;
        qDebug() << "\tHeader: " << i.header;
    }

    QWidget window;

    window.resize(settings.value("winsize", QSize(640, 480)).toSize());
    QObject::connect(&app, &QApplication::aboutToQuit, [&settings, &window](){ settings.setValue("winsize", window.size()); });

    window.setLayout(make_layout());
    window.setWindowTitle(app_name);
    window.show();

    return app.exec();
}
