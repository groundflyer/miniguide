// -*- C++ -*-

#include "mainwindow.hpp"
#include "parser.hpp"

#include <QApplication>
#include <QDir>
#include <QFile>
#include <QFileDialog>
#include <QMessageBox>
#include <QSettings>
#include <QWidget>

static const QString app_name("MinIGuide");

// settings names
namespace st
{
static const QString data("data");

static const QString winsize("Window/winsize");
static const QString split1("Window/split1");
static const QString split2("Window/split2");
static const QString winstate("Window/state");

static const QString search("Session/search");
static const QString techs("Session/technologies");
static const QString cats("Session/categories");
static const QString cpuids("Session/cpuids");
static const QString intrs("Session/intrinsics");
} // namespace st

int
main(int argc, char* argv[])
{
    QApplication app(argc, argv);
    QApplication::setApplicationName(app_name);
    QApplication::setOrganizationName("Roman Saldygashev");

    QString data_path = QApplication::applicationDirPath() + "/data-3.5.2.xml";
    QSettings settings;
    QVariant  data_path_v = settings.value(st::data);
    if(data_path_v.canConvert<QString>()) data_path = data_path_v.toString();

    MainWindow window;

    auto settings_saver = [&settings, &window]()
    {
        const auto [split1, split2] = window.saveSplittersState();

        settings.setValue(st::winsize, window.size());
        settings.setValue(st::split1, split1);
        settings.setValue(st::split2, split2);
        settings.setValue(st::winstate, window.saveState());
        settings.setValue(st::search, window.searchText());
        settings.setValue(st::techs,
                          QStringList(window.selectedTechs().values()));
        settings.setValue(st::cats,
                          QStringList(window.selectedCategories().values()));
        settings.setValue(st::cpuids,
                          QStringList(window.selectedCPUIDs().values()));
        settings.setValue(st::intrs, window.shownIntrinsics());
    };

    QObject::connect(&app, &QApplication::aboutToQuit, settings_saver);

    QFile data_file(data_path);
    if(!data_file.exists())
    {
        data_path = QFileDialog::getOpenFileName(&window,
                                                 "Open Intrinsics Data",
                                                 QDir::homePath(),
                                                 "XML documents (data-*.xml)");
        data_file.setFileName(data_path);
        settings.setValue(st::data, data_path);
    }

    try
    {
        const ParseData data = parse_doc(&data_file);

        window.addIntrinsics(data.intrinsics);
    }
    catch(const ParsingError& ex)
    {
        QMessageBox msg(&window);
        msg.setWindowTitle("Error");
        msg.setIcon(QMessageBox::Critical);
        msg.setText("Failed to parse data");
        switch(ex.reason)
        {
        case ParsingError::NOT_OPEN:
        {
            msg.setDetailedText("Could not open file.");
            break;
        }
        case ParsingError::NOT_IIDATA:
        {
            msg.setDetailedText("Incorrect data format.");
            break;
        }
        }
        msg.exec();
    }

    // loading settings
    {
        window.resize(settings.value(st::winsize, QSize(640, 480)).toSize());

        const QVariant s1 = settings.value(st::split1);
        const QVariant s2 = settings.value(st::split2);
        if(s1.isValid() && s2.isValid())
            window.restoreSplittersState(s1.toByteArray(), s2.toByteArray());

        const QVariant ws = settings.value(st::winstate);
        if(ws.isValid()) window.restoreState(ws.toByteArray());

        // session
        window.setSearch(settings.value(st::search, "").toString());
        window.selectTechs(
            settings.value(st::techs, QStringList()).toStringList());
        window.selectCategories(
            settings.value(st::cats, QStringList()).toStringList());
        window.selectCPUIDs(
            settings.value(st::cpuids, QStringList()).toStringList());
        window.showIntrinsics(
            settings.value(st::intrs, QStringList()).toStringList());
    }

    window.setWindowTitle(app_name);
    window.show();

    return app.exec();
}
