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

	ui->comboBoxNum1Type->addItem("$count");
	ui->comboBoxNum1Type->addItem("$int");
	ui->spinBoxNum1Int->setVisible(false);

	ui->comboBoxNum2Type->addItem("$count");
	ui->comboBoxNum2Type->addItem("$int");
	ui->spinBoxNum2Int->setVisible(false);

    connect(ui->radioButtonAnd, &QRadioButton::clicked, this, &FireDogFeatureRuleInfo::slots_radioClick);
    connect(ui->radioButtonOr, &QRadioButton::clicked, this, &FireDogFeatureRuleInfo::slots_radioClick);
    connect(ui->radioButtonNot, &QRadioButton::clicked, this, &FireDogFeatureRuleInfo::slots_radioClick);

	connect(ui->radioButtonAll, &QRadioButton::clicked, this, &FireDogFeatureRuleInfo::slots_radioClick);
	connect(ui->radioButtonCount, &QRadioButton::clicked, this, &FireDogFeatureRuleInfo::slots_radioClick);
	connect(ui->radioButtonInt, &QRadioButton::clicked, this, &FireDogFeatureRuleInfo::slots_radioClick);

	connect(ui->radioButtonLt, &QRadioButton::clicked, this, &FireDogFeatureRuleInfo::slots_radioClick);
	connect(ui->radioButtonLe, &QRadioButton::clicked, this, &FireDogFeatureRuleInfo::slots_radioClick);
	connect(ui->radioButtonGt, &QRadioButton::clicked, this, &FireDogFeatureRuleInfo::slots_radioClick);
	connect(ui->radioButtonGe, &QRadioButton::clicked, this, &FireDogFeatureRuleInfo::slots_radioClick);

    connect(ui->pushButtonSave, &QPushButton::clicked, this, &FireDogFeatureRuleInfo::slots_saveClick);
    connect(ui->pushButtonCancel, &QPushButton::clicked, this, &FireDogFeatureRuleInfo::slots_cancelClick);


	connect(ui->comboBoxNum1Type, SIGNAL(currentIndexChanged(QString)), this, SLOT(currentIndexChanged(QString)));
	connect(ui->comboBoxNum2Type, SIGNAL(currentIndexChanged(QString)), this, SLOT(currentIndexChanged(QString)));

    //i18n
	//Gui18nUtil::SetText(ui->labelType, "feature-rule-info-type");
	Gui18nUtil::SetText(ui->labelKey, "feature-rule-info-key");
	Gui18nUtil::SetText(ui->pushButtonSave, "feature-rule-info-save");
	Gui18nUtil::SetText(ui->pushButtonCancel, "feature-rule-info-cancel");
}

void FireDogFeatureRuleInfo::slots_radioClick() {
	if (ui->radioButtonAll->isChecked() || ui->radioButtonCount->isChecked()) {
		ui->stackedWidgetRuleInfo->setCurrentIndex(0);
		ui->lineEditKey->setReadOnly(false);
	}
	else {
		ui->lineEditKey->setReadOnly(true);
    }

    if (ui->radioButtonInt->isChecked()) {
        ui->stackedWidgetRuleInfo->setCurrentIndex(1);
		ui->spinBoxInt->setReadOnly(false);
        ui->spinBoxInt->setEnabled(true);
    }
	else {
		ui->spinBoxInt->setReadOnly(true);
		ui->spinBoxInt->setEnabled(false);
    }

	if (ui->radioButtonLt->isChecked()|| ui->radioButtonLe->isChecked()|| ui->radioButtonGt->isChecked()|| ui->radioButtonGe->isChecked()) {
		ui->stackedWidgetRuleInfo->setCurrentIndex(2);

		ui->comboBoxNum1Type->setEnabled(true);
		ui->lineEditNum1Count->setEnabled(true);
		ui->spinBoxNum1Int->setEnabled(true);

		ui->comboBoxNum2Type->setEnabled(true);
		ui->lineEditNum2Count->setEnabled(true);
		ui->spinBoxNum2Int->setEnabled(true);
	}
	else {

		ui->comboBoxNum1Type->setEnabled(false);
		ui->lineEditNum1Count->setEnabled(false);
		ui->spinBoxNum1Int->setEnabled(false);

		ui->comboBoxNum2Type->setEnabled(false);
		ui->lineEditNum2Count->setEnabled(false);
		ui->spinBoxNum2Int->setEnabled(false);
	}

}


void FireDogFeatureRuleInfo::slots_saveClick() {
    this->isOk = true;

	//if (ui->radioButtonKey->isChecked()){
 //       if (ui->lineEditKey->text().isEmpty()) {
	//		QString titleStr = Gui18n::GetInstance()->GetConfig("text-message-box-title-warning", "Warning");
	//		QString messageStr = Gui18n::GetInstance()->GetConfig("feature-rule-please-enter-key", "Please enter [Key].");
	//		QssMessageBox::warn(messageStr, this, titleStr);
 //           return;
 //       }
	//}

    this->hide();
}

void FireDogFeatureRuleInfo::slots_cancelClick() {
	this->isOk = false;
    this->hide();
}

bool FireDogFeatureRuleInfo::updateRule(QString* rule, QVector<QString>* childs, QString parent) {
    this->isOk = false;
	setDefaultValue();

 //   if (isRoot) {
 //       //ui->radioButtonKey->setEnabled(false);
 //   }
	//else {
	//	//ui->radioButtonKey->setEnabled(true);
 //   }

	auto getRuleInfoByStr = [](QString& rulestr) {
		int index = rulestr.lastIndexOf(":"); 
		QString infostr = rulestr.mid(index+1);
		return infostr;
	};

	if (!rule->isEmpty()) {
		if (*rule == "$and") {
			ui->radioButtonAnd->setChecked(true);
		}
		else if (*rule == "$or") {
			ui->radioButtonOr->setChecked(true);
		}
		else if (*rule == "$not") {
			ui->radioButtonNot->setChecked(true);
		}
		//////////////////////////
		else if (rule->startsWith("$all")) {
			ui->radioButtonAll->setChecked(true);
		}
		else if (rule->startsWith("$count")) {
			ui->radioButtonCount->setChecked(true);
		}
		else if (rule->startsWith("$int")) {
			ui->radioButtonInt->setChecked(true);
		}
		//////////////////////////
		else if (*rule == "$lt") {
			ui->radioButtonLt->setChecked(true);
		}
		else if (*rule == "$le") {
			ui->radioButtonLe->setChecked(true);
		}
		else if (*rule == "$gt") {
			ui->radioButtonGt->setChecked(true);
		}
		else if (*rule == "$ge") {
			ui->radioButtonGe->setChecked(true);
		}

		//设置默认值
		if (rule->startsWith("$all") || rule->startsWith("$count")) {
			QString infostr = getRuleInfoByStr(*rule);
			ui->lineEditKey->setText(infostr);
		}
		else if (rule->startsWith("$int")) {
			QString infostr = getRuleInfoByStr(*rule);
			int intval = infostr.toInt();
			ui->spinBoxInt->setValue(intval);
		}
		else if (*rule == "$lt" || *rule == "$le" || *rule == "$gt" || *rule == "$ge") {
			QString num1 = childs->at(0);
			QString num2 = childs->at(1);

			if (num1.startsWith("$count")) {
				ui->comboBoxNum1Type->setCurrentIndex(0);
				QString infostr = getRuleInfoByStr(num1);
				ui->lineEditNum1Count->setText(infostr);
			}
			else {
				ui->comboBoxNum1Type->setCurrentIndex(1);
				QString infostr = getRuleInfoByStr(num1);
				int intval = infostr.toInt();
				ui->spinBoxNum1Int->setValue(intval);
			}

			if (num2.startsWith("$count")) {
				ui->comboBoxNum2Type->setCurrentIndex(0);
				QString infostr = getRuleInfoByStr(num2);
				ui->lineEditNum2Count->setText(infostr);
			}
			else {
				ui->comboBoxNum2Type->setCurrentIndex(1);
				QString infostr = getRuleInfoByStr(num2);
				int intval = infostr.toInt();
				ui->spinBoxNum2Int->setValue(intval);
			}
		}
	}

	//触发一下刷新
	slots_radioClick();

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


void FireDogFeatureRuleInfo::currentIndexChanged(const QString text) {
	QObject* obj = sender();
	if (obj == ui->comboBoxNum1Type) {
		if (text == "$count") {
			ui->spinBoxNum1Int->setVisible(false);
			ui->lineEditNum1Count->setVisible(true);
		}
		else {
			ui->spinBoxNum1Int->setVisible(true);
			ui->lineEditNum1Count->setVisible(false);
		}
	}
	else {
		if (text == "$count") {
			ui->spinBoxNum2Int->setVisible(false);
			ui->lineEditNum2Count->setVisible(true);
		}
		else {
			ui->spinBoxNum2Int->setVisible(true);
			ui->lineEditNum2Count->setVisible(false);
		}
	}

}

void FireDogFeatureRuleInfo::setDefaultValue() {
	ui->radioButtonAnd->setChecked(true);

	ui->lineEditKey->setText("");

	ui->spinBoxInt->setValue(1);

	ui->lineEditNum1Count->setText("");
	ui->spinBoxNum1Int->setValue(1);

	ui->lineEditNum2Count->setText("");
	ui->spinBoxNum2Int->setValue(1);

	ui->stackedWidgetRuleInfo->setCurrentIndex(0);

	slots_radioClick();
}
