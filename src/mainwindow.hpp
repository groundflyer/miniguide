// -*- C++ -*-
// mainwindow.hpp
//

#pragma once

#include "parser.hpp"

#include <QColor>
#include <QDockWidget>
#include <QHash>
#include <QLineEdit>
#include <QListWidget>
#include <QListWidgetItem>
#include <QMainWindow>
#include <QSet>
#include <QSplitter>
#include <QStringList>
#include <QTreeWidget>
#include <QTreeWidgetItem>
#include <QVector>

#include <utility>

class MainWindow : public QMainWindow
{
    Q_OBJECT

    static const inline QStringList technologies = {"MMX",
                                                    "SSE",
                                                    "SSE2",
                                                    "SSE3",
                                                    "SSSE3",
                                                    "SSE4.1",
                                                    "SSE4.2",
                                                    "AVX",
                                                    "AVX2",
                                                    "FMA",
                                                    "AVX-512",
                                                    "KNC",
                                                    "SVML",
                                                    "Other"};

    QLineEdit*                   p_search_edit = new QLineEdit;
    QTreeWidget*                 p_tech_tree   = new QTreeWidget;
    QListWidget*                 p_cat_list    = new QListWidget;
    QListWidget*                 p_name_list   = new QListWidget;
    QSplitter*                   p_left_split  = new QSplitter(Qt::Vertical);
    QSplitter*                   p_top_split   = new QSplitter(Qt::Horizontal);
    QVector<QListWidgetItem*>    m_intrinsics_widgets;
    QVector<QListWidgetItem*>    m_category_widgets;
    QVector<QTreeWidgetItem*>    m_tech_widgets;
    QVector<QTreeWidgetItem*>    m_cpuid_widgets;
    QHash<QString, Intrinsic>    m_intrinsics_map;
    QHash<QString, QDockWidget*> m_dock_widgets;
    QHash<QString, QColor>       m_colormap{
              {  "KNC", {255, 142, 0}},
              { "SVML", {0, 175, 239}},
              {"Other",      Qt::gray}
    };

    QBrush
    techBrush(const QString& tech, const int alpha = 255) const;

    void
    fillTechTree(const QSet<QString>& cpuids);

    void
    fillCategoriesList(const QSet<QString>& categories);

    void
    filter();

    void
    showIntrinsic(const Intrinsic& i);

  private slots:
    void
    selectParent(QTreeWidgetItem* child, int column);

  public:
    MainWindow(QWidget* parent = nullptr);

    void
    addIntrinsics(const Intrinsics&);

    QString
    searchText() const;

    void
    setSearch(const QString&);

    QSet<QString>
    selectedTechs() const;

    void
    selectTechs(const QStringList&);

    QSet<QString>
    selectedCategories() const;

    void
    selectCategories(const QStringList&);

    QSet<QString>
    selectedCPUIDs() const;

    void
    selectCPUIDs(const QStringList&);

    QStringList
    shownIntrinsics() const;

    void
    showIntrinsics(const QStringList&);

    std::pair<QByteArray, QByteArray>
    saveSplittersState() const;

    void
    restoreSplittersState(const QByteArray&, const QByteArray&);
};
