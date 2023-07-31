// -*- C++ -*-
// mainwindow.hpp
//

#pragma once

#include "parser.hpp"
#include "details.hpp"

#include <QMainWindow>
#include <QTreeWidget>
#include <QListWidget>
#include <QListWidgetItem>
#include <QTreeWidgetItem>
#include <QVector>
#include <QHash>
#include <QLineEdit>
#include <QSet>
#include <QStringList>
#include <QScrollArea>
#include <QSplitter>

#include <utility>

class MainWindow : public QMainWindow
{
    Q_OBJECT

    static const inline QStringList techs =
        {
            "MMX",
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
            "Other"
        };

    QLineEdit* p_search_edit = new QLineEdit;
    QTreeWidget* p_tech_tree = new QTreeWidget;
    QListWidget* p_cat_list = new QListWidget;
    QListWidget* p_name_list = new QListWidget;
    IntrinsicDetails* p_details = new IntrinsicDetails;
    QScrollArea* p_details_scroll = new QScrollArea;
    QSplitter* p_left_split = new QSplitter(Qt::Vertical);
    QSplitter* p_top_split = new QSplitter(Qt::Horizontal);
    QVector<QListWidgetItem*> m_intrinsics_widgets;
    QVector<QListWidgetItem*> m_category_widgets;
    QVector<QTreeWidgetItem*> m_tech_widgets;
    QVector<QTreeWidgetItem*> m_cpuid_widgets;
    QHash<QString, Intrinsic> m_intrinsics_map;

    void
    fill_tech_tree(const QSet<QString>& cpuids);

    void
    fill_categories_list(const QSet<QString>& categories);

    void
    filter();

public:
    MainWindow(QWidget* parent = nullptr);

    void
    addIntrinsics(const Intrinsics&);

    QString
    searchText() const;

    QSet<QString>
    selectedTechs() const;

    QSet<QString>
    selectedCategories() const;

    QSet<QString>
    selectedCPUIDs() const;

    std::pair<QByteArray, QByteArray>
    saveSplittersState() const;

    void
    restoreSplittersState(const QByteArray&, const QByteArray&);
};
