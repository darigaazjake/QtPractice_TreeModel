#ifndef TREEITEM_H
#define TREEITEM_H

#include <QList>
#include <QVariant>

class TreeItem
{
public:
    explicit TreeItem(const QList<QVariant> &data, TreeItem *parentItem = 0);
    ~TreeItem();

    //子ノードのポインタを追加する
    void appendChild(TreeItem *child);

    //指定した子ノードのポインタを返す
    TreeItem *child(int row);

    //子の数を返す
    int childCount() const;

    //自分が持っているデータの数を返す
    int columnCount() const;

    //指定した番号のデータを返す
    QVariant data(int column) const;

    //自分のrow番号を返す
    int row() const;

    //自分の親ノードのポインタを返す
    TreeItem *parentItem();

private:
    //親ノードのポインタ
    TreeItem *m_parentItem;

    //子ノードのポインタ（複数）
    QList<TreeItem*> m_childItems;

    //保持するデータ
    QList<QVariant> m_itemData;
};

#endif // TREEITEM_H
