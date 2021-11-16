#include "loadingdialog.h"

#include <QGraphicsDropShadowEffect>

#define LOADING_LAYOUT_WIDTH 18
#define LOADING_LAYOUT_HEIGHT 18
#define LOADING_TIP_HEIGHT 40

LoadingDialog::LoadingDialog(QWidget* parent) :
    QWidget(parent)
{
	int pwidth = parent->width();
	int pheight = parent->height();
	this->setFixedSize(parent->width(), parent->height());

	loading = new QFrame(this);
	loading->setFixedSize(parent->width(), parent->height());

	loading->setStyleSheet("background-color:rgba(128,128,128,0.3)");

	background = new QFrame(loading);
	//设置初始位置和大小
	background->setGeometry((pwidth - LOADING_LAYOUT_WIDTH) / 2, (pheight - (LOADING_LAYOUT_HEIGHT + LOADING_LAYOUT_HEIGHT / 2)) / 2, LOADING_LAYOUT_WIDTH, LOADING_LAYOUT_HEIGHT);
	background->setStyleSheet("background-color:rgba(0,0,0,0)");
	label = new QLabel(background);
	label->setGeometry(0, 0, LOADING_LAYOUT_WIDTH, LOADING_LAYOUT_HEIGHT);
	movie = new QMovie(":/FireDogEditor/resources/loading.gif");
	movie->setScaledSize(QSize(LOADING_LAYOUT_WIDTH, LOADING_LAYOUT_HEIGHT));
	label->setScaledContents(true);
	label->setMovie(movie);
	movie->start();

    connect(this, SIGNAL(loadingEnd()), this, SLOT(hide()));

	this->setGeometry(0, 0, pwidth, pheight);
	loading->setGeometry(0, 0, pwidth, pheight);

	this->hide();
}

void LoadingDialog::resizeEvent(QResizeEvent* event) {

    int pwidth = this->parentWidget()->width();
    int pheight = this->parentWidget()->height();

	this->setFixedSize(pwidth, pheight);
	loading->setFixedSize(pwidth, pheight);

	this->setGeometry(0, 0, pwidth, pheight);
	loading->setGeometry(0, 0, pwidth, pheight);

    this->background->move((pwidth - LOADING_LAYOUT_WIDTH) / 2, (pheight - (LOADING_LAYOUT_HEIGHT + LOADING_LAYOUT_HEIGHT / 2)) / 2);

}

void LoadingDialog::updateTip(QString tipstr) {
    //tip_label->setText(tipstr);
}


LoadingDialog::~LoadingDialog()
{
	//只用把根删掉
	if (this->loading != NULL) {
		delete this->loading;
		this->loading = NULL;
	}
}

