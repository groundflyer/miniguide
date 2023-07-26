#include "layout.hpp"

#include <QVBoxLayout>
#include <QLineEdit>
#include <QTreeWidgetItem>
#include <QBrush>
#include <QLabel>
#include <QWidget>
#include <QSet>
#include <QStringList>


MainLayout::MainLayout(QWidget* parent) : QVBoxLayout(parent)
{
    QLineEdit* search_edit = new QLineEdit;
    search_edit->setPlaceholderText("_mm_search");

    p_tech_tree = new QTreeWidget;

    p_cat_list = new QListWidget;

    QVBoxLayout* tech_lay = new QVBoxLayout;
    tech_lay->addWidget(new QLabel("<b>Technologies</b>"));
    tech_lay->addWidget(p_tech_tree);
    tech_lay->addWidget(new QLabel("<b>Categories</b>"));
    tech_lay->addWidget(p_cat_list);

    p_name_list = new QListWidget;
    p_details = new IntrinsicDetails;

    QHBoxLayout* h_layout = new QHBoxLayout;
    h_layout->setAlignment(Qt::AlignLeft);
    h_layout->addLayout(tech_lay);
    h_layout->addWidget(p_name_list);
    h_layout->addWidget(p_details);

    addWidget(search_edit, 0, Qt::AlignTop);
    addLayout(h_layout);
}

QString format_parms(const QVector<Var>& parms) noexcept
{
    QStringList varlist;

    for (const Var& var: parms)
        varlist.append(var.type + ' ' + var.name);

    return varlist.join(' ');
}


void
MainLayout::addIntrinsics(const Intrinsics& intrinsics)
{
    QSet<QString> techs;
    QSet<QString> categories;
    m_intrinsics_widgets.reserve(intrinsics.count());
    m_intrinsics_map.reserve(intrinsics.count());

    for (const Intrinsic& i : intrinsics)
    {
        techs.insert(i.tech);
        categories.insert(i.category);

        const QString tooltip = QString("%1 %2(%3)").arg(i.ret_type, i.name, format_parms(i.parms));
        QListWidgetItem* item = new QListWidgetItem(i.name);
        item->setToolTip(tooltip);
        m_intrinsics_widgets.append(item);
        p_name_list->addItem(item);
        m_intrinsics_map.insert(i.name, &i);
    }

    QStringList cats = categories.values();
    cats.sort();
    for (const QString &c : cats)
    {
        QListWidgetItem* item = new QListWidgetItem(c);
        item->setFlags(item->flags() | Qt::ItemIsUserCheckable);
        item->setCheckState(Qt::Unchecked);
        p_cat_list->addItem(item);
    }

static QStringList tech_order =
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

 for (const QString &c : tech_order)
 {
     QTreeWidgetItem *item = new QTreeWidgetItem;
     item->setFlags(item->flags() | Qt::ItemIsUserCheckable);
     item->setCheckState(0, Qt::Unchecked);
     item->setText(0, c);
     p_tech_tree->addTopLevelItem(item);
 }
}
