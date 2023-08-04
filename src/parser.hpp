// -*- C++ -*-
// parser.hpp
//

#pragma once

#include <QFile>
#include <QSet>
#include <QString>
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
    QSet<QString>        techs;
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

using Intrinsics = QVector<Intrinsic>;

struct ParseData
{
    QString    version;
    QString    date;
    Intrinsics intrinsics;
};

ParseData
parse_doc(QFile* data_file);
