// -*- C++ -*-
// layout.hpp
//

#pragma once

#include "parser.hpp"

#include <QVBoxLayout>

class MainLayout : public QVBoxLayout
{
    Q_OBJECT

    QVBoxLayout* p_data_layout;
    QWidget* p_data_widget;

public:
    MainLayout(QWidget* parent = nullptr);

    void
    addIntrinsics(const Intrinsics&);
};
