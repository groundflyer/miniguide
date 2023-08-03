#include "details.hpp"

#include <QWidget>
#include <QString>
#include <QStringList>
#include <QVector>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QRegularExpression>


QString html_parms(const QVector<Var>& parms) noexcept
{
    QStringList varlist;

    static const QString type_html = "<font color=darkBlue>%1</font>";
    static const QString parm_html = "<font color=darkCyan>%1</font>";

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

IntrinsicDetails::IntrinsicDetails(const Intrinsic& i, QWidget* parent) : QScrollArea(parent)
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
    p_signature->setWordWrap(true);

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

    static const QString qs_cpuids("CPUID Flags:");
    QLabel* cpuid_label = new QLabel(qs_cpuids);
    QHBoxLayout* cpuid_layout = new QHBoxLayout;
    cpuid_layout->setAlignment(Qt::AlignLeft);
    cpuid_layout->addWidget(cpuid_label);
    cpuid_layout->addWidget(p_cpuids);

    static const QString qs_description("Description");
    QLabel* description_label = new QLabel(qs_description);
    description_label->setFont(bold);
    description_label->setTextFormat(Qt::PlainText);

    p_description->setTextFormat(Qt::RichText);
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
    layout->addLayout(cpuid_layout);
    layout->addWidget(description_label);
    layout->addWidget(p_description);
    layout->addWidget(p_operation_label);
    layout->addWidget(p_operation);

    QWidget* widget = new QWidget;
    widget->setLayout(layout);

    setWidget(widget);
    setWidgetResizable(true);

    setIntrinsic(i);
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

    p_cpuids->setText(QStringList(i.cpuids.values()).join(" + "));

    static const QRegularExpression re_var("\"(\\w+)\"");
    QString descr = i.description;
    p_description->setText(descr.replace(re_var, "<font color=darkCyan>\\1</font>"));

    p_operation_label->setHidden(i.operation.isEmpty());
    p_operation->setHidden(i.operation.isEmpty());
    if (!i.operation.isEmpty())
        p_operation->setText(i.operation);
}
