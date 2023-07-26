// -*- C++ -*-

#include "layout.hpp"
#include "parser.hpp"

#include <QApplication>
#include <QWidget>
#include <QSettings>
#include <QFile>
#include <QFileDialog>
#include <QDir>
#include <QMessageBox>


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

    QWidget window;
    MainLayout* main_layout = new MainLayout;

    window.resize(settings.value("winsize", QSize(640, 480)).toSize());
    QObject::connect(&app, &QApplication::aboutToQuit, [&settings, &window](){ settings.setValue("winsize", window.size()); });

    window.setLayout(main_layout);
    window.setWindowTitle(app_name);
    window.show();

    QFile data_file(data_path);
    if (!data_file.exists())
    {
        data_path = QFileDialog::getOpenFileName(&window, "Open Intrinsics Data",
                                                 QDir::homePath(),
                                                 "XML documents (data-*.xml)");
        data_file.setFileName(data_path);
        settings.setValue("data", data_path);
    }

    try
    {
        const ParseData data = parse_doc(&data_file);

        main_layout->addIntrinsics(data.intrinsics);
    }
    catch (const ParsingError& ex)
    {
        QMessageBox msg(&window);
        msg.setWindowTitle("Error");
        msg.setIcon(QMessageBox::Critical);
        msg.setText("Failed to parse data");
        switch (ex.reason)
        {
        case ParsingError::NOT_OPEN: {
          msg.setDetailedText("Could not open file.");
          break;
        }
        case ParsingError::NOT_IIDATA: {
          msg.setDetailedText("Incorrect data format.");
          break;
        }
        }
        msg.exec();
    }

    return app.exec();
}
