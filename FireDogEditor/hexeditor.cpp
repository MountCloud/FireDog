#include "hexeditor.h"

HexEditor::HexEditor(QWidget* parent /* = 0 */) :QPlainTextEdit(parent) {
	connect(this, &QPlainTextEdit::textChanged, this, &HexEditor::slots_hexChange);
}

void HexEditor::slots_hexChange() {
	QTextCursor cur = this->textCursor();
	int pos = cur.position();

	this->blockSignals(true);
	disconnect(this, &QPlainTextEdit::textChanged, this, &HexEditor::slots_hexChange);

	QString hexStr = this->toPlainText();
	hexStr.remove(" ");
	int oldSize = hexStr.size();
	QRegExp notHexReg(NOT_HEX_REG);
	QString newHexStr = hexStr.remove(notHexReg);
	newHexStr = newHexStr.toUpper();

	int newSize = newHexStr.size();

	QString formatHexStr;
	for (int i = 0; i < newHexStr.size(); i++) {
		formatHexStr.append(newHexStr.at(i));
		if (i != newHexStr.size() - 1 && (i + 1) % 2 == 0) {
			formatHexStr.append(" ");
		}
	}

	if (this->oldHexStr.size() < newHexStr.size() && newSize == oldSize && newSize % 2 != 0) {
		pos = pos + 1;
	}

	this->setPlainText(formatHexStr);
	this->oldHexStr = newHexStr;

	if (newSize < oldSize) {
		pos = pos - 1;
	}

	if (pos < 0) {
		pos = 0;
	}

	if (pos > formatHexStr.size()) {
		cur.setPosition(formatHexStr.size());
	}
	else {
		cur.setPosition(pos);
	}

	this->setTextCursor(cur);

	connect(this, &QPlainTextEdit::textChanged, this, &HexEditor::slots_hexChange);
	this->blockSignals(false);
}