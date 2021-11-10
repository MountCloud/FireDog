#ifndef FIREDOGFEATURERULEINFO_H
#define FIREDOGFEATURERULEINFO_H

#include <QDialog>
#include <QRegExpValidator>

#include "Qss/Qss.h"


#define FEATURE_RULE_FORMATE_REG_KEY "[0-9a-zA-Z]{0,}"

namespace Ui {
class FireDogFeatureRuleInfo;
}

class FireDogFeatureRuleInfo : public QssDialog
{
    Q_OBJECT

public:
    explicit FireDogFeatureRuleInfo(QWidget *parent = nullptr);
    ~FireDogFeatureRuleInfo();

    bool updateRule(QString* rule,bool isRoot);

private slots:
    void slots_radioClick();
    void slots_saveClick();
    void slots_cancelClick();

private:
    Ui::FireDogFeatureRuleInfo *ui;

	QValidator* keyValidator = NULL;

    bool isOk = false;

    void init();

};

#endif // FIREDOGFEATURERULEINFO_H
