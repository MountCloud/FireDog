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
    //this->setStyleSheet("background-color:rgba(128,128,128,128)");

    QPalette pal(this->palette());

    //设置背景
    pal.setBrush(QPalette::Background, QBrush(QPixmap(":/FireDogEditor/resources/loading-new-backgroud.png")));
    this->setAutoFillBackground(true);
    this->setPalette(pal);

    //设置具体阴影
    QGraphicsDropShadowEffect* shadow_effect = new QGraphicsDropShadowEffect(this);
    shadow_effect->setOffset(0, 0);
    //阴影颜色
    shadow_effect->setColor(QColor(38, 78, 119, 127));
    //阴影半径
    shadow_effect->setBlurRadius(22);

    background = new QFrame(this);
    //设置初始位置和大小
    background->setGeometry((pwidth - LOADING_LAYOUT_WIDTH) / 2, (pheight - (LOADING_LAYOUT_HEIGHT + LOADING_LAYOUT_HEIGHT /2)) / 2, LOADING_LAYOUT_WIDTH, LOADING_LAYOUT_HEIGHT);
    //设置阴影
    background->setGraphicsEffect(shadow_effect);
    label = new QLabel(background);
    label->setGeometry(0, 0, LOADING_LAYOUT_WIDTH, LOADING_LAYOUT_HEIGHT);
    movie = new QMovie(":/FireDogEditor/resources/loading.gif");
    movie->setScaledSize(QSize(LOADING_LAYOUT_WIDTH, LOADING_LAYOUT_HEIGHT));
    label->setScaledContents(true);
    label->setMovie(movie);
    movie->start();

    //QFont tipFt;
    //tipFt.setWeight(QFont::Bold);
    //tipFt.setPointSize(LOADING_TIP_HEIGHT);
    //tipFt.setPixelSize(LOADING_TIP_HEIGHT);

    //Qt::Alignment lableAg;
    //lableAg.setFlag(Qt::AlignHCenter);
    //lableAg.setFlag(Qt::AlignVCenter);
    //tip_label = new QLabel(background);
    //tip_label->setText("Loading...");
    //tip_label->setAlignment(lableAg);
    //tip_label->setGeometry(0, LOADING_LAYOUT_HEIGHT - LOADING_TIP_HEIGHT, LOADING_LAYOUT_WIDTH, LOADING_TIP_HEIGHT);
    //tip_label->setStyleSheet("background-color:rgb(255,255,255);color:#494949;");
    //tip_label->setWordWrap(true);
    //tip_label->setFont(tipFt);

    connect(this, SIGNAL(loadingEnd()), this, SLOT(hide()));

	this->hide();
}

void LoadingDialog::resizeEvent(QResizeEvent* event) {

    int pwidth = this->parentWidget()->width();
    int pheight = this->parentWidget()->height();

	//this->setFixedSize(pwidth, pheight);

    this->background->move((pwidth - LOADING_LAYOUT_WIDTH) / 2, (pheight - (LOADING_LAYOUT_HEIGHT + LOADING_LAYOUT_HEIGHT / 2)) / 2);

}

void LoadingDialog::updateTip(QString tipstr) {
    //tip_label->setText(tipstr);
}


LoadingDialog::~LoadingDialog()
{
    delete label;
    delete movie;
    delete background;
}

