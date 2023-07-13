#include "layout.hpp"
#include "Section.h"

#include <QVBoxLayout>
#include <QMainWindow>
#include <QLineEdit>
#include <QListWidget>
#include <QListWidgetItem>
#include <QBrush>
#include <QLabel>
#include <QFont>
#include <QScrollArea>
#include <QWidget>

Section* make_section()
{
    QFont monospace_font("Monospace");
    monospace_font.setStyleHint(QFont::TypeWriter);

    QLabel* label2 = new QLabel("Monospace font\nbla bla bla\nfoo bar");
    label2->setFont(monospace_font);

    QVBoxLayout* layout = new QVBoxLayout;
    layout->addWidget(new QLabel("<H1>Test Label</H1>"));
    layout->addWidget(label2);

    Section* section = new Section("Test section", 300);
    section->setContentLayout(layout);

    return section;
}


QVBoxLayout* make_layout()
{
    QLineEdit* search_edit = new QLineEdit;
    search_edit->setPlaceholderText("_mm_search");

    QListWidgetItem* test_item = new QListWidgetItem("Test Item");
    test_item->setBackground(QBrush(QColor("blue"), Qt::SolidPattern));
    test_item->setFlags(test_item->flags() | Qt::ItemIsUserCheckable);
    test_item->setCheckState(Qt::Unchecked);

    QListWidget* tech_list = new QListWidget;
    tech_list->setMinimumWidth(50);
    tech_list->setMaximumWidth(100);
    tech_list->addItem(test_item);

    QVBoxLayout* tech_lay = new QVBoxLayout;
    tech_lay->addWidget(new QLabel("<B>Technologies</B>"));
    tech_lay->addWidget(tech_list);

    QVBoxLayout* data_lay = new QVBoxLayout;
    data_lay->setAlignment(Qt::AlignTop);
    for (int i = 0; i < 50; ++i)
        data_lay->addWidget(make_section());

    QWidget* data_wd = new QWidget;
    data_wd->setLayout(data_lay);

    QScrollArea* data_scroll = new QScrollArea;
    data_scroll->setWidget(data_wd);
    data_scroll->setWidgetResizable(true);

    QHBoxLayout* layout1 = new QHBoxLayout;
    layout1->setAlignment(Qt::AlignLeft);
    layout1->addLayout(tech_lay);
    layout1->addWidget(data_scroll);
    
    QVBoxLayout* main_layout = new QVBoxLayout;
    main_layout->addWidget(search_edit, 0, Qt::AlignTop);
    main_layout->addLayout(layout1);

    return main_layout;
}
