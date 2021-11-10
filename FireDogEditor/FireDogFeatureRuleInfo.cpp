#include "FireDogFeatureRuleInfo.h"
#include "ui_FireDogFeatureRuleInfo.h"

#include "Qss/Qss.h"

FireDogFeatureRuleInfo::FireDogFeatureRuleInfo(QWidget *parent) :
    QssDialog(parent),
    ui(new Ui::FireDogFeatureRuleInfo)
{
    ui->setupUi(this);

    init();
}

FireDogFeatureRuleInfo::~FireDogFeatureRuleInfo()
{
    delete ui;
}

void FireDogFeatureRuleInfo::init() {
	QRegExp keyregx(FEATURE_RULE_FORMATE_REG_KEY);
	this->keyValidator = new QRegExpValidator(keyregx, ui->lineEditKey);
	ui->lineEditKey->setValidator(keyValidator);

    connect(ui->radioButtonAnd, &QRadioButton::clicked, this, &FireDogFeatureRuleInfo::slots_radioClick);
    connect(ui->radioButtonOr, &QRadioButton::clicked, this, &FireDogFeatureRuleInfo::slots_radioClick);
    connect(ui->radioButtonKey, &QRadioButton::clicked, this, &FireDogFeatureRuleInfo::slots_radioClick);

    connect(ui->pushButtonSave, &QPushButton::clicked, this, &FireDogFeatureRuleInfo::slots_saveClick);
    connect(ui->pushButtonCancel, &QPushButton::clicked, this, &FireDogFeatureRuleInfo::slots_cancelClick);
}

void FireDogFeatureRuleInfo::slots_radioClick() {
	if (ui->radioButtonAnd->isChecked() || ui->radioButtonOr->isChecked()) {
		ui->lineEditKey->setEnabled(false);
	}
	else {
		ui->lineEditKey->setEnabled(true);
    }
}


void FireDogFeatureRuleInfo::slots_saveClick() {
    this->isOk = true;

	if (ui->radioButtonKey->isChecked()){
        if (ui->lineEditKey->text().isEmpty()) {
			QssMessageBox::warn("Please enter [Key].", this, "Warn");
            return;
        }
	}

    this->hide();
}

void FireDogFeatureRuleInfo::slots_cancelClick() {
	this->isOk = false;
    this->hide();
}

bool FireDogFeatureRuleInfo::updateRule(QString* rule, bool isRoot) {
    this->isOk = false;

    if (isRoot) {
        ui->radioButtonKey->setEnabled(false);
    }
	else {
		ui->radioButtonKey->setEnabled(true);
    }

    if (rule->isEmpty() || *rule == "$and") {
		ui->radioButtonAnd->setChecked(true);
		ui->lineEditKey->setEnabled(false);

		ui->lineEditKey->setText("");
    }else if(*rule == "$or") {
		ui->radioButtonOr->setChecked(true);
		ui->lineEditKey->setEnabled(false);

		ui->lineEditKey->setText("");
    }
    else {
        ui->radioButtonKey->setChecked(true);
        ui->lineEditKey->setEnabled(true);
        ui->lineEditKey->setText(*rule);
    }

    this->exec();

    if (this->isOk) {
		if (ui->radioButtonAnd->isChecked()) {
            rule->clear();
            rule->append("$and");
        }
		else if (ui->radioButtonOr->isChecked()) {
			rule->clear();
			rule->append("$or");
        }
		else {
			rule->clear();
			rule->append(ui->lineEditKey->text());
		}
    }

    return this->isOk;
}
