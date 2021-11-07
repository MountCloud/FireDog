#include "bigdatatableitem.h"

BigDataTableCol::BigDataTableCol() {

}

BigDataTableCol::BigDataTableCol(QVariant content) {
	this->content = content;
}

BigDataTableCol::BigDataTableCol(QVariant content, QIcon icon) {
	this->content = content;
	this->icon = icon;
}