// -*- C++ -*-
// layout.hpp
//

#pragma once

#include "parser.hpp"
#include "details.hpp"

#include <QVBoxLayout>
#include <QTreeWidget>
#include <QListWidget>
#include <QListWidgetItem>
#include <QTreeWidgetItem>
#include <QVector>
#include <QHash>
#include <QLineEdit>
#include <QSet>
#include <QStringList>

class MainLayout : public QVBoxLayout
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
    MainLayout(QWidget* parent = nullptr);

    void
    addIntrinsics(const Intrinsics&);

    QString
    search_text() const;

    QSet<QString>
    selected_techs() const;

    QSet<QString>
    selected_categories() const;

    QSet<QString>
    selected_cpuids() const;
};
