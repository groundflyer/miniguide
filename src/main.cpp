#include <QApplication>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QMainWindow>
#include <QLineEdit>
#include <QListWidget>
#include <QListWidgetItem>
#include <QBrush>
#include <QLabel>

int main(int argc, char* argv[])
{
    QApplication app(argc, argv);

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

    QHBoxLayout* layout1 = new QHBoxLayout;
    layout1->setAlignment(Qt::AlignLeft);
    layout1->addLayout(tech_lay);
    layout1->addSpacing(500);
    
    QVBoxLayout* main_layout = new QVBoxLayout;
    main_layout->addWidget(search_edit, 0, Qt::AlignTop);
    main_layout->addLayout(layout1);

    QWidget window;
    window.setLayout(main_layout);
    window.setWindowTitle("IntelⓇ Intrinsics Guide");
    window.show();

    return app.exec();
}
