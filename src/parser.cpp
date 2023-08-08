// -*- C++ -*-
// parser.cpp
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

#include "parser.hpp"

#include <QDomDocument>

#include <utility>

static const inline QStringList order = {
    "MMX",     "SSE Family", "SSE",    "SSE2",       "SSE3",
    "SSSE3",   "SSE4.1",     "SSE4.2", "AVX Family", "AVX",
    "F16C",    "FMA",        "AVX2",   "FMA",        "AVX-512 Family",
    "AVX-512", "AMX Family", "AMX",    "KNC",        "SVML",
    "Other"};

Var
parse_var(const QDomNode& node)
{
    const QDomNamedNodeMap attrs = node.attributes();
    return Var{attrs.namedItem("varname").nodeValue(),
               attrs.namedItem("type").nodeValue()};
}

Instruction
parse_instruction(const QDomNode& node)
{
    const QDomNamedNodeMap attrs = node.attributes();
    return Instruction{attrs.namedItem("name").nodeValue(),
                       attrs.namedItem("form").nodeValue(),
                       attrs.namedItem("xed").nodeValue()};
}

template <typename Op, std::size_t N, typename... Rest>
void
find_match(const QString& name,
           const char (&match)[N],
           Op&& op,
           Rest&&... rest) noexcept
{
    if(name == match)
    {
        op();
        return;
    }

    if constexpr(sizeof...(Rest))
        return find_match(name, std::forward<Rest>(rest)...);
}

template <std::size_t N, typename... Rest>
QString&
add_family(QString& name, const char (&match)[N], Rest&&... rest)
{
    if(name == match) return name += " Family";

    if constexpr(sizeof...(Rest))
        return add_family(name, std::forward<Rest>(rest)...);
    else
        return name;
}

Intrinsic
parse_intrinsic(const QDomNode& node,
                QSet<QString>&  techs,
                QSet<QString>&  cpuids,
                QSet<QString>&  categories)
{
    Intrinsic ret;

    const QDomNamedNodeMap attrs = node.attributes();
    ret.name                     = attrs.namedItem("name").nodeValue();

    QString tech = attrs.namedItem("tech").nodeValue();

    if(tech.endsWith("_ALL"))
        tech.replace("_ALL", " Family");
    else
        add_family(tech, "AVX-512", "AMX");

    ret.tech = tech;
    techs.insert(tech);

    const QDomNodeList fields = node.childNodes();

    for(int i = 0; i < fields.count(); ++i)
    {
        const QDomNode field = fields.at(i);
        const QString  name  = field.nodeName();
        QString        text  = field.toElement().text();

        const auto set_text = [&](auto& member)
        { return [&]() { member = text; }; };

        find_match(
            name,
            "category",
            [&]()
            {
                ret.category = text;
                categories.insert(text);
            },
            "CPUID",
            [&]()
            {
                text.replace("AVX512", "AVX-512");
                ret.cpuids.insert(text);
                cpuids.insert(text);
            },
            "return",
            [&]() {
                ret.ret_type = field.attributes().namedItem("type").nodeValue();
            },
            "parameter",
            [&]() { ret.parms.append(parse_var(field)); },
            "description",
            set_text(ret.description),
            "operation",
            set_text(ret.operation),
            "instruction",
            [&]() { ret.instructions.append(parse_instruction(field)); },
            "header",
            set_text(ret.header));
    }

    if(!ret.parms.empty()) ret.parms.shrink_to_fit();
    if(!ret.instructions.empty()) ret.instructions.shrink_to_fit();

    return ret;
}

template <std::size_t N, typename... Rest>
QString
start_super(const QString& cpuid,
            const char (&super)[N],
            Rest&&... rest) noexcept
{
    if(cpuid.startsWith(super)) return super;

    if constexpr(sizeof...(Rest))
        return start_super(cpuid, std::forward<Rest>(rest)...);
    else
        return "";
}

QString
cpuid_super(const QString& cpuid) noexcept
{
    const QString cand =
        start_super(cpuid, "AMX", "AVX-512", "AVX", "SSE", "MMX");
    if(!cand.isEmpty()) return cand;

    static const QHash<QString, QString> map{
        {"SSSE3", "SSE"},
        { "F16C", "AVX"},
        {  "FMA", "AVX"}
    };

    return map.value(cpuid, "Other");
}

ParseData
parse_doc(QFile* data_file)
{
    QDomDocument data_doc;
    if(data_doc.setContent(data_file))
    {
        const QDomElement      root       = data_doc.documentElement();
        const QDomNamedNodeMap root_attrs = root.attributes();

        const QDomNode date_node    = root_attrs.namedItem("date");
        const QDomNode version_node = root_attrs.namedItem("version");

        if(date_node.isNull() || version_node.isNull())
            throw ParsingError{ParsingError::NOT_IIDATA};

        const QDomNodeList xml_intrinsics = root.childNodes();

        ParseData                     ret;
        QHash<QString, QSet<QString>> techmap;
        QSet<QString>                 categories;

        {
            QSet<QString> techs;
            QSet<QString> cpuids;

            ret.intrinsics.reserve(xml_intrinsics.count());
            for(int i = 0; i < xml_intrinsics.count(); ++i)
                ret.intrinsics.append(parse_intrinsic(xml_intrinsics.at(i),
                                                      techs,
                                                      cpuids,
                                                      categories));

            for(const QString& cpuid: cpuids)
            {
                QString super = cpuid_super(cpuid);
                add_family(super, "SSE", "AVX", "AVX-512", "AMX");
                if(!techmap.contains(super))
                    techmap.insert(super, {cpuid});
                else
                    techmap[super].insert(cpuid);
            }

            for(const QString& t: techs)
                if(!techmap.contains(t)) techmap.insert(t, {});
        }

        const auto cmp = [](const QString& lhs, const QString& rhs) noexcept
        {
            const int  lhs_idx = order.indexOf(lhs);
            const int  rhs_idx = order.indexOf(rhs);
            const bool lhs_ord = lhs_idx != -1;
            const bool rhs_ord = rhs_idx != -1;
            if(lhs_ord && rhs_ord)
                return lhs_idx < rhs_idx;
            else if(lhs_ord)
                return true;
            else if(rhs_ord)
                return false;
            else
                return lhs < rhs;
        };

        // fill up technologies
        ret.technologies.reserve(techmap.count());
        for(auto it = techmap.cbegin(); it != techmap.cend(); ++it)
        {
            QString     tech   = it.key();
            QStringList cpuids = it->values();

            if(cpuids.empty())
                ret.technologies.append({tech, {}});
            else if(cpuids.count() == 1 && tech == cpuids.front())
                ret.technologies.append({tech, {}});
            else
            {
                std::sort(cpuids.begin(), cpuids.end(), cmp);
                ret.technologies.append({tech, std::move(cpuids)});
            }
        }
        std::sort(ret.technologies.begin(),
                  ret.technologies.end(),
                  [&](const Tech& lhs, const Tech& rhs)
                  { return cmp(lhs.family, rhs.family); });

        // fill up categories
        ret.categories.reserve(categories.count());
        ret.categories.append(categories.values());
        ret.categories.sort();

        ret.version = version_node.nodeValue();
        ret.date    = date_node.nodeValue();

        return ret;
    }
    else
        throw ParsingError{};
}
