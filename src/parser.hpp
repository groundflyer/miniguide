// -*- C++ -*-
// parser.hpp
//

#pragma once

#include <QVector>
#include <QFile>
#include <QString>


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
    QString name;
    QString tech;
    QString category;
    QStringList cpuids;
    QString ret_type;
    QVector<Var> parms;
    QString description;
    QString operation;
    QVector<Instruction> instructions;
    QString header;
};

struct ParsingError
{
    enum {
        NOT_XML,
        NOT_IIDATA
    } reason = NOT_XML;
};

using Intrinsics = QVector<Intrinsic>;

struct ParseData
{
    QString version;
    QString date;
    Intrinsics intrinsics;
};

ParseData parse_doc(QFile* data_file);
