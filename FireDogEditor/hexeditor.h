#ifndef __FIREDOG_GUI_HEXEDITOR_H__
#define __FIREDOG_GUI_HEXEDITOR_H__

#include <QPlainTextEdit>

#define NOT_HEX_REG "[^0-9a-fA-F]"

class HexEditor : public QPlainTextEdit {
	Q_OBJECT
public:
	HexEditor(QWidget* parent = 0);

private:
	//老的hex文本
	QString oldHexStr;

private slots:
	//hex编辑器
	void slots_hexChange();

};

#endif