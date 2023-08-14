// -*- C++ -*-
// mainwindow.cpp
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

#include "mainwindow.hpp"
#include "details.hpp"

#include <QBrush>
#include <QHBoxLayout>
#include <QLabel>
#include <QLinearGradient>
#include <QList>
#include <QTabBar>
#include <QVBoxLayout>
#include <QWidget>

#include <functional>

MainWindow::MainWindow(QWidget* parent) : QMainWindow(parent)
{
    p_search_edit->setPlaceholderText("_mm_search or instruction");
    p_search_edit->setClearButtonEnabled(true);

    QHBoxLayout* search_lay = new QHBoxLayout;
    search_lay->setAlignment(Qt::AlignRight);
    search_lay->addWidget(p_search_edit);
    search_lay->addWidget(new QLabel("Return"));
    search_lay->addWidget(p_ret_combo);

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
    top_layout->addLayout(search_lay);
    top_layout->addWidget(p_top_split);

    QWidget* central = new QWidget;
    central->setLayout(top_layout);

    setCentralWidget(central);

    setDockOptions(AnimatedDocks | AllowTabbedDocks);
    setTabPosition(Qt::AllDockWidgetAreas, QTabWidget::North);
}

QString
format_parms(const QVector<Var>& parms) noexcept
{
    QStringList varlist;

    for(const Var& var: parms) varlist.append(var.type + ' ' + var.name);

    return varlist.join(' ');
}

static constexpr inline int id_role = 1000;

QString
intrinsicID(const Intrinsic& i)
{
    static const QString id_template("%1 (%2: %3)");

    QStringList cpuids = i.cpuids.values();

    return id_template.arg(i.name, i.tech, cpuids.join('+'));
};

void
MainWindow::addIntrinsics(const Intrinsics& intrinsics)
{
    m_intrinsics_widgets.reserve(intrinsics.count());
    m_intrinsics_map.reserve(intrinsics.count());

    for(const Intrinsic& i: intrinsics)
    {
        const QString tooltip =
            QString("%1 %2(%3)").arg(i.ret_type, i.name, format_parms(i.parms));
        const QString    id   = intrinsicID(i);
        QListWidgetItem* item = new QListWidgetItem(i.name);
        item->setData(id_role, id);
        item->setToolTip(tooltip);
        item->setBackground(techBrush(i.tech));
        m_intrinsics_widgets.append(item);
        p_name_list->addItem(item);
        m_intrinsics_map.insert(id, i);
    }
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
selected_widgets(const Widgets& widgets,
                 ItemCheck&&    item_check,
                 ItemText&&     item_text) noexcept
{
    QSet<QString> ret;

    for(const auto& item: widgets)
        if(item_check(item) == Qt::Checked) ret.insert(item_text(item));

    return ret;
}

template <typename Widgets, typename ItemText, typename ItemCheck>
void
select_widgets(const QStringList& ss,
               Widgets&           widgets,
               ItemText&&         item_text,
               ItemCheck&&        item_check) noexcept
{
    for(const QString& s: ss)
        for(auto& item: widgets)
            if(item_text(item) == s) item_check(item, Qt::Checked);
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
    return selected_widgets(m_category_widgets,
                            std::mem_fn(&QListWidgetItem::checkState),
                            std::mem_fn(&QListWidgetItem::text));
}

void
MainWindow::selectCategories(const QStringList& ss)
{
    select_widgets(ss,
                   m_category_widgets,
                   std::mem_fn(&QListWidgetItem::text),
                   std::mem_fn(&QListWidgetItem::setCheckState));
}

bool
match_instructions(const QString&              search,
                   const QVector<Instruction>& instructions) noexcept
{
    bool ret = false;

    for(const Instruction& i: instructions)
        ret |= i.name.contains(search, Qt::CaseInsensitive);

    return ret;
}

void
MainWindow::filter()
{
    const QString       search   = searchText();
    const QString       ret_type = selectedRet();
    const QSet<QString> techs    = selectedTechs();
    const QSet<QString> cpuids   = selectedCPUIDs();
    const QSet<QString> cats     = selectedCategories();

    static const QString svml("SVML");

    for(QListWidgetItem* item: m_intrinsics_widgets)
    {
        const Intrinsic& i     = intrinsic(item);
        const QString    iname = item->text();

        const bool search_match = search.isEmpty() ||
                                  iname.contains(search, Qt::CaseInsensitive) ||
                                  match_instructions(search, i.instructions);
        const bool cat_match  = cats.empty() || cats.contains(i.category);
        const bool tech_match = (techs.empty() && cpuids.empty()) ||
                                techs.contains(i.tech) ||
                                cpuids.intersects(i.cpuids);

        // SVML intrinsics have a lot of CPUID flags
        // we don't wanna show them when it is not selected
        const bool svml_match = (i.tech != svml) ||
                                (techs.empty() && cpuids.empty()) ||
                                techs.contains(svml);

        const bool ret_match = (ret_type == "*") || ret_type == i.ret_type;

        const bool match =
            search_match && tech_match && cat_match && svml_match && ret_match;

        item->setHidden(!match);
    }
}

void
MainWindow::fillCategoriesList(const QStringList& categories)
{
    for(const QString& c: categories)
    {
        QListWidgetItem* item = new QListWidgetItem(c);
        item->setFlags(item->flags() | Qt::ItemIsUserCheckable);
        item->setCheckState(Qt::Unchecked);
        p_cat_list->addItem(item);
        m_category_widgets.append(item);
    }
}

void
MainWindow::fillTechTree(const QVector<Tech>& technologies)
{
    // filling colormap
    const int num_clrs = technologies.count() - 2;
    int       h        = 59;
    const int d        = (359 - h) / num_clrs;
    for(int i = 0; i <= num_clrs; ++i, h += d)
        m_colormap.insert(technologies[i].family, QColor::fromHsv(h, 255, 200));

    // fill the tree widget
    for(const Tech& tt: technologies)
    {
        QTreeWidgetItem* item = new QTreeWidgetItem(p_tech_tree);
        item->setFlags(item->flags() | Qt::ItemIsUserCheckable);
        item->setCheckState(0, Qt::Unchecked);
        item->setText(0, tt.family);
        item->setBackground(0, techBrush(tt.family));

        m_tech_widgets.append(item);

        for(const QString& sub: tt.techs)
        {
            QTreeWidgetItem* child = new QTreeWidgetItem(item);
            child->setFlags(child->flags() | Qt::ItemIsUserCheckable);
            child->setCheckState(0, Qt::Unchecked);
            child->setText(0, sub);
            child->setBackground(0, techBrush(tt.family, 127));

            m_cpuid_widgets.append(child);
        }
    }

    QObject::connect(p_tech_tree,
                     &QTreeWidget::itemChanged,
                     this,
                     &MainWindow::selectParent);
}

void
MainWindow::fillRetCombo(const QStringList& rets)
{
    p_ret_combo->addItems(rets);
}

QString
MainWindow::selectedRet() const
{
    return p_ret_combo->currentText();
}

void
MainWindow::selectRet(const QString& ret)
{
    p_ret_combo->setCurrentText(ret);
}

std::pair<QByteArray, QByteArray>
MainWindow::saveSplittersState() const
{
    return {p_left_split->saveState(), p_top_split->saveState()};
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
    const QString iid = intrinsicID(i);
    if(m_dock_widgets.contains(iid))
    {
        m_dock_widgets[iid]->show();
        m_dock_widgets[iid]->setFocus(Qt::OtherFocusReason);
    }
    else
    {
        static const QString stylesheet_template(
            "#idetails {border: 3px inset %1;}");
        IntrinsicDetails* idw = new IntrinsicDetails(i);
        QDockWidget*      dw  = new QDockWidget(iid);
        const QColor      clr = m_colormap.value(i.tech, Qt::gray);
        idw->setStyleSheet(stylesheet_template.arg(clr.name()));
        dw->setObjectName(iid);
        dw->setWidget(idw);
        dw->setAllowedAreas(Qt::RightDockWidgetArea);
        addDockWidget(Qt::RightDockWidgetArea, dw);
        if(!m_dock_widgets.empty())
            tabifyDockWidget(m_dock_widgets.values().back(), dw);
        m_dock_widgets.insert(iid, dw);
    }

    for(QTabBar* tab: findChildren<QTabBar*>("", Qt::FindDirectChildrenOnly))
        for(int ti = 0; ti < tab->count(); ++ti)
            if(tab->tabText(ti) == iid)
            {
                tab->setCurrentIndex(ti);
                return;
            }
}

const Intrinsic&
MainWindow::intrinsic(QListWidgetItem* item)
{
    return m_intrinsics_map[item->data(id_role).toString()];
}

QStringList
MainWindow::shownIntrinsics() const
{
    QStringList ret;

    for(auto it = m_dock_widgets.cbegin(); it != m_dock_widgets.cend(); ++it)
        if(!it.value()->isHidden() || it.value()->isFloating())
            ret.append(it.key());

    return ret;
}

void
MainWindow::showIntrinsics(const QStringList& ins)
{
    for(const QString& in: ins) showIntrinsic(m_intrinsics_map[in]);

    for(QDockWidget* dw: m_dock_widgets) restoreDockWidget(dw);
}

QBrush
MainWindow::techBrush(const QString& tech, const int alpha) const
{
    QColor clr = m_colormap[tech];
    clr.setAlpha(alpha);
    QLinearGradient grad(0., 1., 1., 1.);
    grad.setCoordinateMode(QGradient::ObjectMode);
    grad.setColorAt(0., Qt::white);
    grad.setColorAt(1., clr);

    return {grad};
}

bool
hasSelectedChildren(QTreeWidgetItem* item)
{
    for(int i = 0; i < item->childCount(); ++i)
        if(item->child(i)->checkState(0) == Qt::Checked) return true;

    return false;
}

void
deselectChildren(QTreeWidgetItem* item)
{
    if(item->checkState(0) == Qt::Unchecked)
        for(int i = 0; i < item->childCount(); ++i)
            if(item->child(i)->checkState(0) == Qt::Checked)
                item->child(i)->setCheckState(0, Qt::Unchecked);
}

void
smartSwitch(QTreeWidgetItem* item, int column)
{
    if(item->checkState(column) != Qt::Checked)
        item->setCheckState(column,
                            hasSelectedChildren(item) ? Qt::PartiallyChecked :
                                                        Qt::Unchecked);
}

void
MainWindow::selectParent(QTreeWidgetItem* child, int column)
{
    QTreeWidgetItem* parent = child->parent();
    if(parent)
        smartSwitch(parent, column);
    else
        deselectChildren(child);
}

void
MainWindow::connectSignals()
{
    auto slot = [&](auto...) { filter(); };

    QObject::connect(p_tech_tree, &QTreeWidget::itemChanged, slot);
    QObject::connect(p_cat_list, &QListWidget::itemChanged, slot);
    QObject::connect(p_search_edit, &QLineEdit::textChanged, slot);
    QObject::connect(p_ret_combo, &QComboBox::currentTextChanged, slot);
    QObject::connect(p_name_list,
                     &QListWidget::itemClicked,
                     [&](QListWidgetItem* item)
                     { showIntrinsic(intrinsic(item)); });
}

void
MainWindow::initialFilter()
{
    if(!(searchText().isEmpty() && selectedRet() == "*" &&
         selectedTechs().isEmpty() && selectedCPUIDs().isEmpty() &&
         selectedCategories().isEmpty()))
        filter();
}
