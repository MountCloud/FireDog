#ifndef FIREDOGFEATUREINFO_H
#define FIREDOGFEATUREINFO_H

#include <QDialog>

#include "Qss/Qss.h"

namespace Ui {
class FireDogFeatureInfo;
}

class FireDogFeatureInfo : public QssDialog
{
    Q_OBJECT

public:
    explicit FireDogFeatureInfo(QWidget *parent = nullptr);
    ~FireDogFeatureInfo();

private:
    Ui::FireDogFeatureInfo *ui;
};

#endif // FIREDOGFEATUREINFO_H
