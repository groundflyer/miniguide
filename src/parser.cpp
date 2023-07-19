// -*- C++ -*-
// parser.cpp

#include "parser.hpp"

#include <QDomDocument>

Var parse_var(const QDomNode& node)
{
    const QDomNamedNodeMap attrs = node.attributes();
    return Var{ attrs.namedItem("varname").nodeValue(),
                attrs.namedItem("type").nodeValue() };
}

Instruction parse_instruction(const QDomNode& node)
{
    const QDomNamedNodeMap attrs = node.attributes();
    return Instruction { attrs.namedItem("name").nodeValue(),
                         attrs.namedItem("form").nodeValue(),
                         attrs.namedItem("xed").nodeValue() };
}

Intrinsic parse_intrinsic(const QDomNode& node)
{
    Intrinsic ret;

    const QDomNamedNodeMap attrs = node.attributes();
    ret.name = attrs.namedItem("name").nodeValue();
    ret.tech = attrs.namedItem("tech").nodeValue();

    const QDomNodeList fields = node.childNodes();
    for (int i = 0; i < fields.count(); ++i)
    {
        const QDomNode field = fields.at(i);
        const QString name = field.nodeName();
        const QString text = field.toElement().text();

        if (name == "category")
            ret.category = text;
        else if (name == "CPUID")
            ret.cpuids.append(text);
        else if (name == "return")
            ret.ret_type = field.attributes().namedItem("type").nodeValue();
        else if (name == "parameter")
            ret.parms.append(parse_var(field));
        else if (name == "description")
            ret.description = text;
        else if (name == "operation")
            ret.operation = text;
        else if (name == "instruction")
            ret.instructions.append(parse_instruction(field));
        else if (name == "header")
            ret.header = text;
    }

    if (!ret.parms.empty())
        ret.parms.shrink_to_fit();

    if (!ret.instructions.empty())
        ret.instructions.shrink_to_fit();

    return ret;
}

ParseData parse_doc(QFile* data_file)
{
    ParseData ret;

    QDomDocument data_doc;
    if (data_doc.setContent(data_file))
    {
        const QDomElement root = data_doc.documentElement();
        const QDomNamedNodeMap root_attrs = root.attributes();

        const QDomNode date_node = root_attrs.namedItem("date");
        const QDomNode version_node = root_attrs.namedItem("version");

        if (date_node.isNull() || version_node.isNull())
            throw ParsingError{ ParsingError::NOT_IIDATA };

        const QDomNodeList xml_intrinsics = root.childNodes();

        ret.intrinsics.reserve(xml_intrinsics.count());
        for (int i = 0; i < xml_intrinsics.count(); ++i)
            ret.intrinsics.append(parse_intrinsic(xml_intrinsics.at(i)));

        ret.version = version_node.nodeValue();
        ret.date = date_node.nodeValue();
    }
    else throw ParsingError{};

    return ret;
}
