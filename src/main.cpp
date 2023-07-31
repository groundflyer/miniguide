// -*- C++ -*-

#include "mainwindow.hpp"
#include "parser.hpp"

#include <QApplication>
#include <QWidget>
#include <QSettings>
#include <QFile>
#include <QFileDialog>
#include <QDir>
#include <QMessageBox>


static const QString app_name("Intrinsics Guide");
static const QString st_data("data");
static const QString st_winsize("window/winsize");
static const QString st_split1("window/split1");
static const QString st_split2("window/split2");

int main(int argc, char* argv[])
{
    QApplication app(argc, argv);
    QApplication::setApplicationName(app_name);
    QApplication::setOrganizationName("Roman Saldygashev");

    QString data_path = QApplication::applicationDirPath() + "/data-3.5.2.xml";
    QSettings settings;
    QVariant data_path_v = settings.value(st_data);
    if (data_path_v.canConvert<QString>())
        data_path = data_path_v.toString();

    MainWindow window;

    auto settings_saver = [&settings, &window]()
    {
        settings.setValue(st_winsize, window.size());

        const auto [split1, split2] = window.saveSplittersState();
        settings.setValue(st_split1, split1);
        settings.setValue(st_split2, split2);
    };

    // loading settings
    {
        window.resize(settings.value(st_winsize, QSize(640, 480)).toSize());
        const QVariant s1 = settings.value(st_split1);
        const QVariant s2 = settings.value(st_split2);
        if (s1.isValid() && s2.isValid())
            window.restoreSplittersState(s1.toByteArray(), s2.toByteArray());
    }

    QObject::connect(&app, &QApplication::aboutToQuit, settings_saver);

    window.setWindowTitle(app_name);
    window.show();

    QFile data_file(data_path);
    if (!data_file.exists())
    {
        data_path = QFileDialog::getOpenFileName(&window, "Open Intrinsics Data",
                                                 QDir::homePath(),
                                                 "XML documents (data-*.xml)");
        data_file.setFileName(data_path);
        settings.setValue(st_data, data_path);
    }

    try
    {
        const ParseData data = parse_doc(&data_file);

        window.addIntrinsics(data.intrinsics);
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
