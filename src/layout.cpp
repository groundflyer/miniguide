#include "layout.hpp"
#include "Section.h"

#include <QVBoxLayout>
#include <QLineEdit>
#include <QListWidget>
#include <QListWidgetItem>
#include <QBrush>
#include <QLabel>
#include <QFont>
#include <QScrollArea>
#include <QWidget>


enum class TECH
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

static constexpr auto techs =
    {
        "MMX",
        "SSE",
        "SSE2",
        "SSE3",
        "SSSE3",
        "SSE4.1",
        "SSE4.2",
        "AVX",
        "AVX2",
        "FMA",
        "AVX-512",
        "KNC",
        "SVML",
        "Other"
    };

static const inline QString type_html = "<font color=darkBlue>%1</font>";
static const inline QString parm_html = "<font color=darkCyan>%1</font>";

QString html_parms(const QVector<Var>& parms) noexcept
{
    QStringList varlist;

    for (const Var& var: parms)
        varlist.append(type_html.arg(var.type) + ' ' + parm_html.arg(var.name));

    static const QString comma(", ");
    return varlist.join(comma);
}

QString format_parms(const QVector<Var>& parms) noexcept
{
    QStringList varlist;

    for (const Var& var: parms)
        varlist.append(var.type + ' ' + var.name);

    return varlist.join(' ');
}

QString format_instructions(const QVector<Instruction>& ins) noexcept
{
    QStringList list;

    for (const Instruction& i : ins)
        list.append(QString("%1 %2").arg(i.name.toLower(), i.form));

    return list.join('\n');
}

Section* make_section(const Intrinsic& intr)
{
    const QString name = QString("%1 %2(%3)").arg(intr.ret_type, intr.name, format_parms(intr.parms));

    static const QString font_family_ms("Monospace");
    QFont monospace(font_family_ms);
    monospace.setStyleHint(QFont::TypeWriter);

    QFont bold;
    bold.setBold(true);

    static const QString qs_synopsis ("Synopsis");
    QLabel* synopsis = new QLabel(qs_synopsis);
    synopsis->setFont(bold);
    synopsis->setTextFormat(Qt::PlainText);

    static const QString sign_html("<font color=darkBlue>%1</font> %2(%3)");
    QLabel* signature = new QLabel(sign_html.arg(intr.ret_type, intr.name, html_parms(intr.parms)));
    signature->setFont(monospace);
    signature->setTextFormat(Qt::RichText);
    signature->setTextInteractionFlags(Qt::TextSelectableByMouse);

    static const QString inc_template("#include <%1>");
    QLabel* header = new QLabel(inc_template.arg(intr.header));
    header->setFont(monospace);
    header->setTextFormat(Qt::PlainText);
    header->setTextInteractionFlags(Qt::TextSelectableByMouse);

    static const QString qs_instruction ("Instruction:");
    QLabel* instruction = new QLabel(qs_instruction);
    instruction->setTextFormat(Qt::PlainText);
    instruction->setAlignment(Qt::AlignTop);

    QLabel* instruction_list = new QLabel(format_instructions(intr.instructions));
    instruction_list->setTextFormat(Qt::PlainText);
    instruction_list->setFont(monospace);

    QHBoxLayout* instructions_layout = new QHBoxLayout;
    instructions_layout->setAlignment(Qt::AlignLeft);
    instructions_layout->addWidget(instruction);
    instructions_layout->addWidget(instruction_list);

    static const QString qs_description("Description");
    QLabel* description_label = new QLabel(qs_description);
    description_label->setFont(bold);
    description_label->setTextFormat(Qt::PlainText);

    QLabel* description = new QLabel(intr.description);
    description->setTextFormat(Qt::PlainText);
    description->setWordWrap(true);

    static const QString qs_operation("Operation");
    QLabel* operation_label = new QLabel(qs_operation);
    operation_label->setFont(bold);
    operation_label->setTextFormat(Qt::PlainText);

    QLabel* operation = new QLabel(intr.operation);
    operation->setFont(monospace);
    operation->setTextFormat(Qt::PlainText);

    QVBoxLayout* layout = new QVBoxLayout;
    layout->addWidget(synopsis);
    layout->addWidget(signature);
    layout->addWidget(header);
    layout->addLayout(instructions_layout);
    layout->addWidget(description_label);
    layout->addWidget(description);
    layout->addWidget(operation_label);
    layout->addWidget(operation);

    Section* section = new Section(name, 300);
    section->setContentLayout(layout);

    return section;
}

MainLayout::MainLayout(QWidget* parent) : QVBoxLayout(parent)
{
    QLineEdit* search_edit = new QLineEdit;
    search_edit->setPlaceholderText("_mm_search");

    QListWidget* tech_list = new QListWidget;
    tech_list->setMinimumWidth(100);
    tech_list->setMaximumWidth(200);

    // filling up tech list
    for (const auto& t : techs)
    {
        QListWidgetItem* item = new QListWidgetItem(t);
        // item->setBackground(QBrush(QColor("blue"), Qt::SolidPattern));
        item->setFlags(item->flags() | Qt::ItemIsUserCheckable);
        item->setCheckState(Qt::Unchecked);
        tech_list->addItem(item);
    }

    QVBoxLayout* tech_lay = new QVBoxLayout;
    tech_lay->addWidget(new QLabel("<b>Technologies</b>"));
    tech_lay->addWidget(tech_list);

    p_data_layout = new QVBoxLayout;
    p_data_layout->setAlignment(Qt::AlignTop);

    p_data_widget = new QWidget;
    p_data_widget->setLayout(p_data_layout);

    QScrollArea* data_scroll = new QScrollArea;
    data_scroll->setWidget(p_data_widget);
    data_scroll->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    data_scroll->setWidgetResizable(true);

    QHBoxLayout* h_layout = new QHBoxLayout;
    h_layout->setAlignment(Qt::AlignLeft);
    h_layout->addLayout(tech_lay);
    h_layout->addWidget(data_scroll);

    addWidget(search_edit, 0, Qt::AlignTop);
    addLayout(h_layout);
}

void
MainLayout::addIntrinsics(const Intrinsics& intrinsics)
{
    for (int i = 0; i < 50; ++i)
      p_data_layout->addWidget(make_section(intrinsics[i]));
}
