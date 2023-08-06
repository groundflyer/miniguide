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
              {"Other", Qt::gray}
    };

    QBrush
    techBrush(const QString& tech, const int alpha = 255) const;

    void
    filter();

    void
    showIntrinsic(const Intrinsic& i);

    const Intrinsic&
    intrinsic(QListWidgetItem* item);

  private slots:
    void
    selectParent(QTreeWidgetItem* child, int column);

  public:
    MainWindow(QWidget* parent = nullptr);

    void
    fillTechTree(const QVector<Tech>& technologies);

    void
    fillCategoriesList(const QStringList& categories);

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
