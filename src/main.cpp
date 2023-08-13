// -*- C++ -*-
//
//    MinIGuide - minimalistic intrinsics guide
//    Copyright (C) 2023  Roman Saldygashev
//
//    This program is free software: you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation, either version 3 of the License, or
//    (at your option) any later version.
//
//    This program is distributed in the hope that it will be useful,
//    but WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//    GNU General Public License for more details.
//
//    You should have received a copy of the GNU General Public License
//    along with this program.  If not, see <https://www.gnu.org/licenses/>.

#include "mainwindow.hpp"
#include "parser.hpp"

#include <QApplication>
#include <QDir>
#include <QElapsedTimer>
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
static const QString ret("Session/ret");
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
    QApplication::setOrganizationName("MinIGuide Project");

    QString data_path = QApplication::applicationDirPath() + "/data-3-6-6.xml";
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
        settings.setValue(st::ret, window.selectedRet());
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

    QString data_v;
    QString data_d;

    try
    {
        QElapsedTimer timer;
        timer.start();

        const ParseData data = parse_doc(&data_file);
        data_v               = data.version;
        data_d               = data.date;

        qInfo("Parsed data in %.03f seconds",
              static_cast<float>(timer.restart()) / 1000.f);

        window.fillTechTree(data.technologies);
        window.fillCategoriesList(data.categories);
        window.fillRetCombo(data.rets);
        window.addIntrinsics(data.intrinsics);

        qInfo("Initialized GUI in %.03f seconds",
              static_cast<float>(timer.elapsed()) / 1000.f);
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
        window.selectRet(settings.value(st::ret, "*").toString());
        window.selectTechs(
            settings.value(st::techs, QStringList()).toStringList());
        window.selectCategories(
            settings.value(st::cats, QStringList()).toStringList());
        window.selectCPUIDs(
            settings.value(st::cpuids, QStringList()).toStringList());
        window.showIntrinsics(
            settings.value(st::intrs, QStringList()).toStringList());
    }

    if(!data_v.isEmpty())
        window.setWindowTitle(
            QString("%1 [data v%2 (%3)]").arg(app_name, data_v, data_d));
    else
        window.setWindowTitle(app_name);

    window.show();
    window.connectSignals();
    window.initialFilter();

    return app.exec();
}
