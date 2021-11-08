#ifndef FIREDOGFEATUREINFO_H
#define FIREDOGFEATUREINFO_H

#include <QDialog>
#include <QRegExpValidator>

#include "Qss/Qss.h"

#define FEATURE_FORMATE_REG_HEX "(([a-fA-F0-9?]{2}[ ]?)|(\\[([0-9a-fA-F]{2}[ ]?-[ ]?[a-fA-F0-9]{2}[ ]?[,]?){1,}\\][ ]?)){0,}"

namespace firedog {
    class Feature;
}

namespace Ui {
    class FireDogFeatureInfo;
}

class FireDogFeatureInfo : public QssDialog
{
    Q_OBJECT
public:
    explicit FireDogFeatureInfo(QWidget *parent = nullptr);
    ~FireDogFeatureInfo();

    //…Ë÷√Ãÿ’˜
    bool updateFeature(firedog::Feature* feature);

private slots:
    void slots_radioClick();

    void slots_save();
    void slots_cancel();

private:
    Ui::FireDogFeatureInfo *ui;

    QValidator* hexValidator = NULL;

    void init();

    bool isOk = false;
    
};

#endif // FIREDOGFEATUREINFO_H
