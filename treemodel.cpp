#include "treeitem.h"
#include "treemodel.h"

#include <QStringList>

/**
 * @brief コンストラクタ、ツリーデータを作成する入り口
 * @param data
 * @param parent
 */
TreeModel::TreeModel(const QString &data, QObject *parent)
    : QAbstractItemModel(parent)
{
    QList<QVariant> rootData;
    rootData << "Title" << "Summary";

    //rootItemを実体化
    rootItem = new TreeItem(rootData);

    //setupModelDataをコール
    //splitで行ごとに分けてから(QStringList)渡す
    setupModelData(data.split(QString("\n")), rootItem);
}

TreeModel::~TreeModel()
{
    //TreeItemのデストラクタは再帰的に子をdeleteするように仕込んであるので、
    //ここではrootを消すだけで良い
    delete rootItem;
}



/**
 * @brief アイテムのデータを返す
 * @param index 要求されているデータのインデックス
 * @param role 返すデータの使用目的 return値の分岐に使ったりする
 * @return データ
 */
QVariant TreeModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    if (role != Qt::DisplayRole)
        return QVariant();

    TreeItem *item = static_cast<TreeItem*>(index.internalPointer());

    return item->data(index.column());
}

Qt::ItemFlags TreeModel::flags(const QModelIndex &index) const
{
    if (!index.isValid())
        return 0;

    return QAbstractItemModel::flags(index);
}

QVariant TreeModel::headerData(int section, Qt::Orientation orientation,
                               int role) const
{
    if (orientation == Qt::Horizontal && role == Qt::DisplayRole)
        return rootItem->data(section);

    return QVariant();
}

QModelIndex TreeModel::index(int row, int column, const QModelIndex &parent)
            const
{
    if (!hasIndex(row, column, parent))
        return QModelIndex();

    TreeItem *parentItem;

    //親がvalidでない場合、自分はトップレベルの要素であるので、親=rootとする
    if (!parent.isValid())
        parentItem = rootItem;
    //親のQModelIndexからポインタを得て、TreeItem*にキャスト
    else
        parentItem = static_cast<TreeItem*>(parent.internalPointer());

    TreeItem *childItem = parentItem->child(row);
    if (childItem)
        return createIndex(row, column, childItem);
    else
        //QModelIndex()は無効なインデックスであることを表す
        return QModelIndex();
}

QModelIndex TreeModel::parent(const QModelIndex &index) const
{
    if (!index.isValid())
        return QModelIndex();

    TreeItem *childItem = static_cast<TreeItem*>(index.internalPointer());
    TreeItem *parentItem = childItem->parentItem();

    if (parentItem == rootItem)
        return QModelIndex();

    return createIndex(parentItem->row(), 0, parentItem);
}

int TreeModel::columnCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return static_cast<TreeItem*>(parent.internalPointer())->columnCount();
    else
        return rootItem->columnCount();
}

int TreeModel::rowCount(const QModelIndex &parent) const
{
    TreeItem *parentItem;
    if (parent.column() > 0)
        return 0;

    if (!parent.isValid())
        parentItem = rootItem;
    else
        parentItem = static_cast<TreeItem*>(parent.internalPointer());

    return parentItem->childCount();
}

/**
 * @brief ツリーデータを構築するメイン部分
 * @param lines 入力となる文字列データのリスト
 * @param parent 親ノードのポインタ
 */
void TreeModel::setupModelData(const QStringList &lines, TreeItem *parent)
{
    QList<TreeItem*> parents;
    QList<int> indentations;

    //親リストの先頭に、引数で受け取ったポインタを詰める
    parents << parent;
    indentations << 0;

    int number = 0;

    while (number < lines.count())
    {
        //行頭のインデント数を調べる(=空白ではない文字の位置を探す)
        int position = 0;
        while (position < lines[number].length())
        {
            if (lines[number].at(position) != ' ')
                break;
            position++;
        }

        //行頭空白と行末改行を取り除いた文字列
        QString lineData = lines[number].mid(position).trimmed();

        if (!lineData.isEmpty())
        {
            //文字列をタブで分割する
            //(behaviorにQString::SkipEmptyPartsを指定すると、空の部分は無視されて詰められる)
            QStringList columnStrings = lineData.split("\t", QString::SkipEmptyParts);

            //QListに文字列リストを詰め直す
            QList<QVariant> columnData;
            for (int column = 0; column < columnStrings.count(); ++column)
                columnData << columnStrings[column];

            //現在行はどの親にぶら下がるべきかを判定する
            //そのために、前行のインデントと比較
            if (position > indentations.last())
            {
                // The last child of the current parent is now the new parent
                // unless the current parent has no children.

                //前行よりもインデントが大きかった
                // => 基本的には、前行が現在行の親となる

                //前行の親に子がすでに居た
                //(いない場合=初回はrootが親であり、子がいない。その場合は親探ししなくて良い)
                if (parents.last()->childCount() > 0)
                {
                    //前行(=親リスト末尾の、子リストの末尾)を親とする
                    parents << parents.last()->child(parents.last()->childCount()-1);
                    indentations << position;
                }
            }
            else
            {
                //前行と同じインデントか、より少ないインデントだった

                //現在行と同じか小さいインデントをみつけるまで遡り、親を探す
                while (position < indentations.last() && parents.count() > 0)
                {
                    parents.pop_back();
                    indentations.pop_back();
                }
            }

            //親の子リストに、現在行のデータを登録する
            parents.last()->appendChild(new TreeItem(columnData, parents.last()));
        }

        ++number;
    }
}
