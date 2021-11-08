/////实现
#include "bigdatatablemodel.h"
#include <QColor>
BigDataTableModel::BigDataTableModel(QObject* parent) : QAbstractTableModel(parent)
{
}
BigDataTableModel::~BigDataTableModel()
{
}
int BigDataTableModel::rowCount(const QModelIndex&) const
{
    int rowCount = m_vecData.size();
    return rowCount;
}
int BigDataTableModel::columnCount(const QModelIndex&) const
{
    int colCount = horHeaderData.size();
    return colCount;
}
void BigDataTableModel::setHeaders(const QList<QString>& headers)
{
    //自定义的表头设置接口，horHeaderData为QList<QString>成员变量
    this->horHeaderData = headers;
    emit headerDataChanged(Qt::Horizontal, 0, headers.count() - 1);
}
QVariant BigDataTableModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role == Qt::DisplayRole && orientation == Qt::Horizontal)
    {
        if (section < horHeaderData.size())
        {
            return horHeaderData[section];
        }
    }
    return QAbstractItemModel::headerData(section, orientation, role);
}
QVariant BigDataTableModel::data(const QModelIndex& index, int role) const
{
    if (!index.isValid())
    {
        return QVariant();
    }
    switch (role)
    {
    case Qt::DecorationRole:
    {
        if (m_vecData.size() > index.row())
        {
            QVector<BigDataTableCol> vecdata = m_vecData[0].contents;
            if (vecdata.size() > index.column())
            {
                QVector<BigDataTableCol> vecdata = m_vecData[index.row()].contents;
                QIcon icon = vecdata[index.column()].icon;
                if (!icon.isNull()) {
                    return vecdata[index.column()].icon;
                }
            }
        }
        return QVariant();
    }
    case Qt::TextColorRole:
        return QColor(Qt::black);
    case Qt::TextAlignmentRole:
        return QVariant(Qt::AlignLeft | Qt::AlignVCenter);
    case Qt::DisplayRole:
    {
        if (m_vecData.size() > index.row())
        {
            QVector<BigDataTableCol> vecdata = m_vecData[0].contents;
            if (vecdata.size() > index.column())
            {
                QVector<BigDataTableCol> vecdata = m_vecData[index.row()].contents;
                return vecdata[index.column()].content;
            }
        }
        return QVariant();
    }
    case Qt::CheckStateRole:
    {
        return QVariant(); // 返回勾选框，如果返回QVariant()就没有勾选框
    }
    default:
        return QVariant();
    }
}
BigDataTableRow BigDataTableModel::getRowData(int nRow)
{
    if (nRow < 0 || nRow >= m_vecData.size())
    {
        BigDataTableRow item;
        item.isEmpty = true;
        return item;
    }
    return m_vecData[nRow];
}

bool BigDataTableModel::removeRow(int row, const QModelIndex& parent) {
	beginResetModel();
	m_vecData.removeAt(row);
	endResetModel();
    return true;
}

bool BigDataTableModel::removeRows(int row, int count, const QModelIndex& parent) {
	beginResetModel();
    m_vecData.remove(row, count);
	endResetModel();
    return true;
}

void BigDataTableModel::addRow(BigDataTableRow row) {
	beginResetModel();
	m_vecData.append(row);
	endResetModel();
}

void BigDataTableModel::insertRow(int index, BigDataTableRow row) {
	beginResetModel();
    m_vecData.insert(index, row);
	endResetModel();
}

void BigDataTableModel::clear()
{
	beginResetModel();
    m_vecData.clear();
	endResetModel();
}
void BigDataTableModel::handleData(const QVector<BigDataTableRow>& vecData)
{
    //emit signalRecvhandleData();
    // 开始刷新模型
    beginResetModel();
    m_vecData = vecData;
    // 结束刷新模型
    endResetModel();
}