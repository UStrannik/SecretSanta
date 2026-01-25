#pragma once

#include <QtWidgets>

/*
 * вместо виджета для редактирования данных в ячейке таблицы возвращает 0
 * таким образом редактирование данных становится невозможным
 */
class NonEditTableDelegate : public QStyledItemDelegate
{
    Q_OBJECT    
public:
    NonEditTableDelegate(QObject * parent = nullptr) : QStyledItemDelegate(parent) {}
    virtual QWidget * createEditor(QWidget *, const QStyleOptionViewItem &,
                                  const QModelIndex &) const
    {
        return 0;
    }
};
