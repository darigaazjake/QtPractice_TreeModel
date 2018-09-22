#ifndef TREEMODEL_H
#define TREEMODEL_H

#include <QAbstractItemModel>
#include <QModelIndex>
#include <QVariant>

class TreeItem;

/**
 * @brief ポインタで繋がっている、いわゆるツリー構造のデータ
 * このサンプルコードは、データ編集しない前提で書かれている
 *
 * 読み取り専用の場合は、
 * QAbstractItemModelのサブクラスは以下の3つが純粋仮想(=実装必須)
 * 1: data アイテムのデータを返す
 * 2: rowCount 親から見た、自分のrowインデックスを返す
 * 3: columnCount 親から見た、自分のcolumnインデックスを返す
 *
 * 以下は、状況に応じてオーバーライド
 * 1: headerData
 * 2: flags 各アイテムのフラグ（編集可能・選択可能・など）を返す
 */
class TreeModel : public QAbstractItemModel
{
    Q_OBJECT

public:
    explicit TreeModel(const QString &data, QObject *parent = 0);
    ~TreeModel();

    QVariant data(const QModelIndex &index, int role) const override;
    Qt::ItemFlags flags(const QModelIndex &index) const override;
    QVariant headerData(int section, Qt::Orientation orientation,
                        int role = Qt::DisplayRole) const override;

    //viewとデリゲートがインデックスを得るために利用する
    QModelIndex index(int row, int column,
                      const QModelIndex &parent = QModelIndex()) const override;

    QModelIndex parent(const QModelIndex &index) const override;
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;

private:
    void setupModelData(const QStringList &lines, TreeItem *parent);

    TreeItem *rootItem;
};

#endif // TREEMODEL_H
