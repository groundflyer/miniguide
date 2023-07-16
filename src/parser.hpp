// -*- C++ -*-
// parser.hpp
//

#pragma once

#include "intrinsic.hpp"

#include <QList>
#include <QFile>

struct ParsingError
{
    enum {
        NOT_XML,
        NOT_IIDATA
    } reason = NOT_XML;
};

struct ParseData
{
    QString version;
    QString date;
    QList<Intrinsic> intrinsics;
};

ParseData parse_doc(QFile* data_file);
