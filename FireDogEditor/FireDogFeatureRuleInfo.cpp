#include "FireDogFeatureRuleInfo.h"
#include "ui_FireDogFeatureRuleInfo.h"

#include "Qss/Qss.h"

#include "gui18n.h"
#include "gui18nutil.h"

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

    //i18n
	Gui18nUtil::SetText(ui->labelType, "feature-rule-info-type");
	Gui18nUtil::SetText(ui->labelKey, "feature-rule-info-key");
	Gui18nUtil::SetText(ui->pushButtonSave, "feature-rule-info-save");
	Gui18nUtil::SetText(ui->pushButtonCancel, "feature-rule-info-cancel");
}

void FireDogFeatureRuleInfo::slots_radioClick() {
	if (ui->radioButtonAnd->isChecked() || ui->radioButtonOr->isChecked()) {
		ui->lineEditKey->setReadOnly(true);
	}
	else {
		ui->lineEditKey->setReadOnly(false);
    }
}


void FireDogFeatureRuleInfo::slots_saveClick() {
    this->isOk = true;

	if (ui->radioButtonKey->isChecked()){
        if (ui->lineEditKey->text().isEmpty()) {
			QString titleStr = Gui18n::GetInstance()->GetConfig("text-message-box-title-warning", "Warning");
			QString messageStr = Gui18n::GetInstance()->GetConfig("feature-rule-please-enter-key", "Please enter [Key].");
			QssMessageBox::warn(messageStr, this, titleStr);
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
		ui->lineEditKey->setReadOnly(true);

		ui->lineEditKey->setText("");
    }else if(*rule == "$or") {
		ui->radioButtonOr->setChecked(true);
		ui->lineEditKey->setReadOnly(true);

		ui->lineEditKey->setText("");
    }
    else {
        ui->radioButtonKey->setChecked(true);
        ui->lineEditKey->setReadOnly(false);
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
