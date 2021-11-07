#ifndef __FIREDOG_GUI_BIGDATATABLEITEM_H__
#define __FIREDOG_GUI_BIGDATATABLEITEM_H__

#include <qvariant.h>
#include <qvector.h>
#include <qstring.h>
#include <qhash.h>
#include <qicon.h>

class BigDataTableCol {
public:
    QVariant content;
    QIcon icon;
    BigDataTableCol();
    BigDataTableCol(QVariant content);
    BigDataTableCol(QVariant content, QIcon icon);
};

class BigDataTableRow {
public:
    QVector<BigDataTableCol> contents;
    QString id;
    QHash<QString, QString> data;
    bool isEmpty = false;
};
#endif