#include "FireDogFeatureInfo.h"
#include "ui_FireDogFeatureInfo.h"

FireDogFeatureInfo::FireDogFeatureInfo(QWidget *parent) :
    QssDialog(parent),
    ui(new Ui::FireDogFeatureInfo)
{
    ui->setupUi(this);
}

FireDogFeatureInfo::~FireDogFeatureInfo()
{
    delete ui;
}
