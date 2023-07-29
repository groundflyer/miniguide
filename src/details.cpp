#include "details.hpp"

#include <QString>
#include <QStringList>
#include <QVector>
#include <QHBoxLayout>
#include <QVBoxLayout>

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

QString format_instructions(const QVector<Instruction>& ins) noexcept
{
    QStringList list;

    for (const Instruction& i : ins)
        list.append(QString("%1 %2").arg(i.name.toLower(), i.form));

    return list.join('\n');
}

IntrinsicDetails::IntrinsicDetails(QWidget* parent) : QWidget(parent)
{
    monospace.setStyleHint(QFont::TypeWriter);
    bold.setBold(true);

    static const QString qs_synopsis ("Synopsis");
    QLabel* synopsis = new QLabel(qs_synopsis);
    synopsis->setFont(bold);
    synopsis->setTextFormat(Qt::PlainText);

    p_signature->setFont(monospace);
    p_signature->setTextFormat(Qt::RichText);
    p_signature->setTextInteractionFlags(Qt::TextSelectableByMouse);

    p_header->setFont(monospace);
    p_header->setTextFormat(Qt::PlainText);
    p_header->setTextInteractionFlags(Qt::TextSelectableByMouse);

    p_instructions_label->setTextFormat(Qt::PlainText);
    p_instructions_label->setAlignment(Qt::AlignTop);

    p_instructions->setTextFormat(Qt::PlainText);
    p_instructions->setFont(monospace);

    QHBoxLayout* instructions_layout = new QHBoxLayout;
    instructions_layout->setAlignment(Qt::AlignLeft);
    instructions_layout->addWidget(p_instructions_label);
    instructions_layout->addWidget(p_instructions);

    static const QString qs_description("Description");
    QLabel* description_label = new QLabel(qs_description);
    description_label->setFont(bold);
    description_label->setTextFormat(Qt::PlainText);

    p_description->setTextFormat(Qt::PlainText);
    p_description->setWordWrap(true);
    p_description->setScaledContents(true);

    p_operation_label->setFont(bold);
    p_operation_label->setTextFormat(Qt::PlainText);

    p_operation->setFont(monospace);
    p_operation->setTextFormat(Qt::PlainText);

    QVBoxLayout* layout = new QVBoxLayout;
    layout->setAlignment(Qt::AlignTop);
    layout->addWidget(synopsis);
    layout->addWidget(p_signature);
    layout->addWidget(p_header);
    layout->addLayout(instructions_layout);
    layout->addWidget(description_label);
    layout->addWidget(p_description);
    layout->addWidget(p_operation_label);
    layout->addWidget(p_operation);

    setLayout(layout);
}

void
IntrinsicDetails::setIntrinsic(const Intrinsic& i)
{
    static const QString sign_html("<font color=darkBlue>%1</font> %2(%3)");
    p_signature->setText(sign_html.arg(i.ret_type, i.name, html_parms(i.parms)));

    static const QString inc_template("#include <%1>");
    p_header->setText(inc_template.arg(i.header));

    p_instructions_label->setHidden(i.instructions.empty());
    p_instructions->setHidden(i.instructions.empty());
    if (!i.instructions.empty())
        p_instructions->setText(format_instructions(i.instructions));

    p_description->setText(i.description);

    p_operation_label->setHidden(i.operation.isEmpty());
    p_operation->setHidden(i.operation.isEmpty());
    if (!i.operation.isEmpty())
        p_operation->setText(i.operation);
}
