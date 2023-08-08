// -*- C++ -*-
// details.hpp
//
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

#pragma once

#include "parser.hpp"

#include <QFont>
#include <QLabel>
#include <QScrollArea>

class IntrinsicDetails : public QScrollArea
{
    Q_OBJECT

    QFont monospace = QFont("Monospace");
    QFont bold;

    QLabel* p_signature          = new QLabel;
    QLabel* p_header             = new QLabel;
    QLabel* p_instructions_label = new QLabel("Instruction:");
    QLabel* p_instructions       = new QLabel;
    QLabel* p_cpuids             = new QLabel;
    QLabel* p_description        = new QLabel;
    QLabel* p_operation_label    = new QLabel("Operation");
    QLabel* p_operation          = new QLabel;

    void
    setIntrinsic(const Intrinsic&);

  public:
    IntrinsicDetails(const Intrinsic& i, QWidget* parent = nullptr);
};
