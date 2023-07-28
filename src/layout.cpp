#include "layout.hpp"

#include <QVBoxLayout>
#include <QBrush>
#include <QLabel>
#include <QWidget>

#include <functional>

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

    fill_tech_tree(cpuids);
    fill_categories_list(categories);

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

template <typename Widgets, typename ItemCheck, typename ItemText>
QSet<QString>
selected_widgets(const Widgets& widgets, ItemCheck&& item_check, ItemText&& item_text) noexcept
{
    QSet<QString> ret;

    for (const auto& item : widgets)
        if (item_check(item) == Qt::Checked)
            ret.insert(item_text(item));

    return ret;
}

auto
tree_item_check(const QTreeWidgetItem* item) noexcept
{
    return item->checkState(0);
}

QString
tree_item_text(const QTreeWidgetItem* item) noexcept
{
    return item->text(0);
}

QSet<QString>
MainLayout::selected_techs() const
{
    return selected_widgets(m_tech_widgets, tree_item_check, tree_item_text);
}

QSet<QString>
MainLayout::selected_cpuids() const
{
    return selected_widgets(m_cpuid_widgets, tree_item_check, tree_item_text);
}

QSet<QString>
MainLayout::selected_categories() const
{
    return selected_widgets(m_category_widgets, std::mem_fn(&QListWidgetItem::checkState), std::mem_fn(&QListWidgetItem::text));
}

void
MainLayout::filter()
{
    const QString search_name = search_text();
    const QSet<QString> techs = selected_techs();
    const QSet<QString> cpuids = selected_cpuids();
    const QSet<QString> cats = selected_categories();

    for (QListWidgetItem* item : m_intrinsics_widgets)
    {
        const QString iname = item->text();
        const Intrinsic& i = m_intrinsics_map[iname];

        const bool name_match = search_name.isEmpty() || iname.contains(search_name, Qt::CaseInsensitive);
        const bool cat_match = cats.empty() || cats.contains(i.category);
        const bool tech_match = (techs.empty() && cpuids.empty()) || techs.contains(i.tech) || cpuids.intersects(i.cpuids);

        const bool match = name_match &&
            tech_match &&
            cat_match;

        item->setHidden(!match);
    }
}

void
MainLayout::fill_categories_list(const QSet<QString>& categories)
{
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
}

void
MainLayout::fill_tech_tree(const QSet<QString>& cpuids)
{
    QHash<QString, QStringList> subtech;

    for (const QString& tt : techs)
        subtech.insert(tt, QStringList());

    for (const QString& cpuid : cpuids)
    {
        if (techs.contains(cpuid))
            continue;

        bool other = true;

        for(auto it = techs.crbegin(); it != techs.crend(); ++it)
        {
            QString tt = *it;
            // AVX-512 cpuids are actually AVX512_BLABLABLA
            // so we remove '-' when comparing
            tt.remove('-');

            if (cpuid != tt && cpuid.startsWith(tt))
            {
                // but the key is actuall tech name
                subtech[*it].append(cpuid);
                other = false;
                break;
            }
        }

        if (other)
            subtech["Other"].append(cpuid);
    }

    for (const QString &tt : techs)
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

                m_cpuid_widgets.append(child);
            }
        }
    }
}
