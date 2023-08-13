// -*- C++ -*-
// parser.hpp
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

#include <QFile>
#include <QSet>
#include <QString>
#include <QStringList>
#include <QVector>

struct Var
{
    QString name;
    QString type;
};

struct Instruction
{
    QString name;
    QString form;
    QString xed;
};

struct Intrinsic
{
    QString              name;
    QString              tech;
    QString              category;
    QSet<QString>        cpuids;
    QString              ret_type;
    QVector<Var>         parms;
    QString              description;
    QString              operation;
    QVector<Instruction> instructions;
    QString              header;
};

struct ParsingError
{
    enum
    {
        NOT_OPEN,
        NOT_IIDATA
    } reason = NOT_OPEN;
};

struct Tech
{
    QString     family;
    QStringList techs;
};

using Intrinsics = QVector<Intrinsic>;

struct ParseData
{
    QString       version;
    QString       date;
    Intrinsics    intrinsics;
    QVector<Tech> technologies;
    QStringList   categories;
    QStringList   rets;
};

ParseData
parse_doc(QFile* data_file);
