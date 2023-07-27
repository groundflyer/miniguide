#include "layout.hpp"

#include <QVBoxLayout>
#include <QBrush>
#include <QLabel>
#include <QWidget>
#include <QStringList>

#include <QDebug>

MainLayout::MainLayout(QWidget* parent) : QVBoxLayout(parent)
{
    p_search_edit->setPlaceholderText("_mm_search");

    p_tech_tree->setHeaderHidden(true);

    QVBoxLayout* tech_lay = new QVBoxLayout;
    tech_lay->addWidget(new QLabel("<b>Technologies</b>"));
    tech_lay->addWidget(p_tech_tree);
    tech_lay->addWidget(new QLabel("<b>Categories</b>"));
    tech_lay->addWidget(p_cat_list);

    QHBoxLayout* h_layout = new QHBoxLayout;
    h_layout->setAlignment(Qt::AlignLeft);
    h_layout->addLayout(tech_lay);
    h_layout->addWidget(p_name_list);
    h_layout->addWidget(p_details);

    addWidget(p_search_edit, 0, Qt::AlignTop);
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
    QSet<QString> cpuids;
    m_intrinsics_widgets.reserve(intrinsics.count());
    m_intrinsics_map.reserve(intrinsics.count());

    for (const Intrinsic& i : intrinsics)
    {
        techs.insert(i.tech);
        categories.insert(i.category);

        cpuids.unite(i.cpuids);

        const QString tooltip = QString("%1 %2(%3)").arg(i.ret_type, i.name, format_parms(i.parms));
        QListWidgetItem* item = new QListWidgetItem(i.name);
        item->setToolTip(tooltip);
        m_intrinsics_widgets.append(item);
        p_name_list->addItem(item);
        m_intrinsics_map.insert(i.name, i);
    }

    // filling up categories
    QStringList cats = categories.values();
    cats.sort();
    m_category_widgets.reserve(cats.count());

    for (const QString &c : cats)
    {
        QListWidgetItem* item = new QListWidgetItem(c);
        item->setFlags(item->flags() | Qt::ItemIsUserCheckable);
        item->setCheckState(Qt::Unchecked);
        p_cat_list->addItem(item);
        m_category_widgets.append(item);
    }

    const QStringList top_tech =
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

    QHash<QString, QStringList> subtech;
    for (const QString& tt : top_tech)
        subtech.insert(tt, QStringList());

    for (const QString& cpuid : cpuids)
    {
        if (top_tech.contains(cpuid))
            continue;

        bool other = true;

        for(auto it = top_tech.crbegin(); it != top_tech.crend(); ++it)
        {
            QString tt = *it;
            // AVX-512 cpuids are actually AVX512BLABLABLA
            tt.remove('-');
            if (cpuid != tt && cpuid.startsWith(tt))
            {
                subtech[*it].append(cpuid);
                other = false;
                break;
            }
        }

        if (other)
            subtech["Other"].append(cpuid);
    }

    for (const QString &tt : top_tech)
    {
        QTreeWidgetItem *item = new QTreeWidgetItem(p_tech_tree);
        item->setFlags(item->flags() | Qt::ItemIsUserCheckable);
        item->setCheckState(0, Qt::Unchecked);
        item->setText(0, tt);

        m_tech_widgets.append(item);

        QStringList& subs = subtech[tt];

        if (!subs.empty())
        {
            subs.sort();
            for (const QString& sub : subs)
            {
                QTreeWidgetItem *child = new QTreeWidgetItem(item);
                child->setFlags(child->flags() | Qt::ItemIsUserCheckable);
                child->setCheckState(0, Qt::Unchecked);
                child->setText(0, sub);

                m_tech_widgets.append(child);
            }
        }
    }

    auto slot = [&](auto...){ filter(); };

    QObject::connect(p_tech_tree, &QTreeWidget::itemChanged, slot);
    QObject::connect(p_cat_list, &QListWidget::itemChanged, slot);
    QObject::connect(p_search_edit, &QLineEdit::textChanged, slot);
}

QString
MainLayout::search_text() const
{
    return p_search_edit->text();
}

QSet<QString>
MainLayout::selected_techs() const
{
    QSet<QString> ret;

    for (QTreeWidgetItem* item : m_tech_widgets)
        if (item->checkState(0) == Qt::Checked)
            ret.insert(item->text(0));


    return ret;
}

QSet<QString>
MainLayout::selected_categories() const
{
    QSet<QString> ret;

    for (QListWidgetItem* item : m_category_widgets)
        if (item->checkState() == Qt::Checked)
            ret.insert(item->text());

    return ret;
}

void
MainLayout::filter()
{
    const QString search_name = search_text();
    const QSet<QString> techs = selected_techs();
    const QSet<QString> cats = selected_categories();

    for (QListWidgetItem* item : m_intrinsics_widgets)
    {
        const QString iname = item->text();
        const Intrinsic& i = m_intrinsics_map[iname];

        const bool name_match = search_name.isEmpty() || iname.contains(search_name, Qt::CaseInsensitive);
        const bool tech_match = techs.empty() || techs.contains(i.tech);
        // const bool cpuid_match = techs.empty() || techs.intersects(i.cpuids);
        const bool cat_match = cats.empty() || cats.contains(i.category);

        const bool match = name_match &&
            tech_match &&
            // cpuid_match &&
            cat_match;

        item->setHidden(!match);
    }
}
