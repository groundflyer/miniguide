#include "mainwindow.hpp"
#include "details.hpp"

#include <QVBoxLayout>
#include <QBrush>
#include <QLabel>
#include <QWidget>

#include <functional>

MainWindow::MainWindow(QWidget* parent) : QMainWindow(parent)
{
    p_search_edit->setPlaceholderText("_mm_search");

    p_tech_tree->setHeaderHidden(true);

    QVBoxLayout* tech_lay = new QVBoxLayout;
    tech_lay->addWidget(new QLabel("<b>Technologies</b>"));
    tech_lay->addWidget(p_tech_tree);

    QWidget* techs_widget = new QWidget;
    techs_widget->setLayout(tech_lay);

    QVBoxLayout* cat_lay = new QVBoxLayout;
    cat_lay->addWidget(new QLabel("<b>Categories</b>"));
    cat_lay->addWidget(p_cat_list);

    QWidget* cats_widget = new QWidget;
    cats_widget->setLayout(cat_lay);

    p_left_split->setChildrenCollapsible(false);
    p_left_split->addWidget(techs_widget);
    p_left_split->addWidget(cats_widget);

    p_top_split->setChildrenCollapsible(false);
    p_top_split->setSizePolicy(p_name_list->sizePolicy());
    p_top_split->addWidget(p_left_split);
    p_top_split->addWidget(p_name_list);

    QVBoxLayout* top_layout = new QVBoxLayout;
    top_layout->setAlignment(Qt::AlignTop);
    top_layout->addWidget(p_search_edit);
    top_layout->addWidget(p_top_split);

    QWidget* central = new QWidget;
    central->setLayout(top_layout);

    setCentralWidget(central);
}

QString format_parms(const QVector<Var>& parms) noexcept
{
    QStringList varlist;

    for (const Var& var: parms)
        varlist.append(var.type + ' ' + var.name);

    return varlist.join(' ');
}


void
MainWindow::addIntrinsics(const Intrinsics& intrinsics)
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

    fillTechTree(cpuids);
    fillCategoriesList(categories);

    auto slot = [&](auto...){
        filter();
    };

    QObject::connect(p_tech_tree, &QTreeWidget::itemChanged, slot);
    QObject::connect(p_cat_list, &QListWidget::itemChanged, slot);
    QObject::connect(p_search_edit, &QLineEdit::textChanged, slot);
    QObject::connect(p_name_list, &QListWidget::itemClicked, [&](QListWidgetItem* item) {
        showIntrinsic(m_intrinsics_map[item->text()]);
    });
}

QString
MainWindow::searchText() const
{
    return p_search_edit->text();
}

void
MainWindow::setSearch(const QString& s)
{
    p_search_edit->setText(s);
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

template <typename Widgets, typename ItemText, typename ItemCheck>
void
select_widgets(const QStringList& ss, Widgets& widgets, ItemText&& item_text, ItemCheck&& item_check) noexcept
{
    for (const QString& s : ss)
        for (auto& item : widgets)
            if (item_text(item) == s)
                item_check(item, Qt::Checked);
}

auto
tree_item_check(const QTreeWidgetItem* item) noexcept
{
    return item->checkState(0);
}

void
tree_item_set_check(QTreeWidgetItem* item, const Qt::CheckState cs) noexcept
{
    item->setCheckState(0, cs);
}

QString
tree_item_text(const QTreeWidgetItem* item) noexcept
{
    return item->text(0);
}

QSet<QString>
MainWindow::selectedTechs() const
{
    return selected_widgets(m_tech_widgets, tree_item_check, tree_item_text);
}

void
MainWindow::selectTechs(const QStringList& ss)
{
    select_widgets(ss, m_tech_widgets, tree_item_text, tree_item_set_check);
}

QSet<QString>
MainWindow::selectedCPUIDs() const
{
    return selected_widgets(m_cpuid_widgets, tree_item_check, tree_item_text);
}

void
MainWindow::selectCPUIDs(const QStringList& ss)
{
    select_widgets(ss, m_cpuid_widgets, tree_item_text, tree_item_set_check);
}

QSet<QString>
MainWindow::selectedCategories() const
{
    return selected_widgets(m_category_widgets, std::mem_fn(&QListWidgetItem::checkState), std::mem_fn(&QListWidgetItem::text));
}

void
MainWindow::selectCategories(const QStringList& ss)
{
    select_widgets(ss, m_category_widgets, std::mem_fn(&QListWidgetItem::text), std::mem_fn(&QListWidgetItem::setCheckState));
}

void
MainWindow::filter()
{
    const QString search_name = searchText();
    const QSet<QString> techs = selectedTechs();
    const QSet<QString> cpuids = selectedCPUIDs();
    const QSet<QString> cats = selectedCategories();

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
MainWindow::fillCategoriesList(const QSet<QString>& categories)
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
MainWindow::fillTechTree(const QSet<QString>& cpuids)
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

std::pair<QByteArray, QByteArray>
MainWindow::saveSplittersState() const
{
    return { p_left_split->saveState(), p_top_split->saveState() };
}

void
MainWindow::restoreSplittersState(const QByteArray& s1, const QByteArray& s2)
{
    p_left_split->restoreState(s1);
    p_top_split->restoreState(s2);
}

void
MainWindow::showIntrinsic(const Intrinsic& i)
{
    if (m_dock_widgets.contains(i.name))
    {
        m_dock_widgets[i.name]->setFocus(Qt::OtherFocusReason);
    } else {
        IntrinsicDetails *id = new IntrinsicDetails(i);
        QDockWidget *dw = new QDockWidget(i.name);
        dw->setObjectName(i.name);
        dw->setWidget(id);
        addDockWidget(Qt::RightDockWidgetArea, dw);
        m_dock_widgets.insert(i.name, dw);
    }
}

QStringList
MainWindow::shownIntrinsics() const
{
    QStringList ret;

    for (auto it = m_dock_widgets.cbegin(); it != m_dock_widgets.cend(); ++it)
        if (!it.value()->isHidden() || it.value()->isFloating())
            ret.append(it.key());

    return ret;
}

void
MainWindow::showIntrinsics(const QStringList& ins)
{
    for (const QString& in : ins)
        showIntrinsic(m_intrinsics_map[in]);

    for (QDockWidget* dw : m_dock_widgets)
        restoreDockWidget(dw);
}
