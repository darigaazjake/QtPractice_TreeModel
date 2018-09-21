/**
  以下のサイトが参考になる
  https://qiita.com/tetsurom/items/acef221a06a2421e33d8
  */

#include "treemodel.h"

#include <QApplication>
#include <QFile>
#include <QTreeView>

int main(int argc, char *argv[])
{
    Q_INIT_RESOURCE(simpletreemodel);

    QApplication app(argc, argv);

    QFile file(":/default.txt");
    file.open(QIODevice::ReadOnly);
    TreeModel model(file.readAll());
    file.close();

    QTreeView view;
    view.setModel(&model);
    view.setWindowTitle(QObject::tr("Simple Tree Model"));
    view.show();
    return app.exec();
}
