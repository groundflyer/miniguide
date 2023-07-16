// -*- C++ -*-
// data_types.hpp
//

#pragma once

#include <QString>
#include <QList>


enum CPUID
    {
        MMX,
        SSE,
        SSE2,
        SSE3,
        SSSE3,
        SSE41,
        SSE42,
        AVX,
        AVX2,
        FMA,
        AVX512,
        KNC,
        SVML,
        OTHER
    };

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
    QList<Var> parms;
    QString description;
    QString operation;
    QList<Instruction> instructions;
    QString header;
};
