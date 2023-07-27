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
#include <QVector>
#include <QHash>

class MainLayout : public QVBoxLayout
{
    Q_OBJECT

    QTreeWidget* p_tech_tree = new QTreeWidget;
    QListWidget* p_cat_list = new QListWidget;
    QListWidget* p_name_list = new QListWidget;
    IntrinsicDetails* p_details = new IntrinsicDetails;
    QVector<QListWidgetItem*> m_intrinsics_widgets;
    QHash<QString, const Intrinsic*> m_intrinsics_map;

public:
    MainLayout(QWidget* parent = nullptr);

    void
    addIntrinsics(const Intrinsics&);
};
