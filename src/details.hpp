// -*- C++ -*-
// details.hpp
//

#pragma once

#include "parser.hpp"

#include <QWidget>
#include <QLabel>
#include <QFont>

class IntrinsicDetails : public QWidget
{
    Q_OBJECT

    QFont monospace = QFont("Monospace");
    QFont bold;

    QLabel* p_signature = new QLabel;
    QLabel* p_header = new QLabel;
    QLabel* p_instructions_label = new QLabel("Instruction:");
    QLabel* p_instructions = new QLabel;
    QLabel* p_description = new QLabel;
    QLabel* p_operation_label = new QLabel("Operation:");
    QLabel* p_operation = new QLabel;

public:
    IntrinsicDetails(QWidget* parent = nullptr);

    void setIntrinsic(const Intrinsic&);
};
