#include "FireDogFeatureInfo.h"
#include "ui_FireDogFeatureInfo.h"

#include "featurelibrary.h"
#include "Qss/Qss.h"

#include "gui18n.h"
#include "gui18nutil.h"

FireDogFeatureInfo::FireDogFeatureInfo(QWidget *parent) :
    QssDialog(parent),
    ui(new Ui::FireDogFeatureInfo)
{
    ui->setupUi(this);

    init();
}


void FireDogFeatureInfo::init() {


	QRegExp hexregx(FEATURE_FORMATE_REG_HEX);
    this->hexValidator = new QRegExpValidator(hexregx, ui->lineEditContent);

    QRegExp keyregx(FEATURE_FORMATE_REG_KEY);
    this->keyValidator = new QRegExpValidator(keyregx, ui->lineEditKey);
    ui->lineEditKey->setValidator(keyValidator);

    connect(ui->pushButtonCancel, &QPushButton::clicked, this, &FireDogFeatureInfo::slots_cancel);
    connect(ui->pushButtonSave, &QPushButton::clicked, this, &FireDogFeatureInfo::slots_save);

    connect(ui->radioButtonHex, &QRadioButton::clicked, this, &FireDogFeatureInfo::slots_radioClick);
    connect(ui->radioButtonText, &QRadioButton::clicked, this, &FireDogFeatureInfo::slots_radioClick);

    //i18n
	Gui18nUtil::SetText(ui->labelType, "feature-info-type");
	Gui18nUtil::SetText(ui->radioButtonHex, "feature-info-type-hex");
	Gui18nUtil::SetText(ui->radioButtonText, "feature-info-type-text");
	Gui18nUtil::SetText(ui->labelKey, "feature-info-key");
	Gui18nUtil::SetText(ui->lineEditKey, "feature-info-key-text");
	Gui18nUtil::SetText(ui->labelContent, "feature-info-content");
	Gui18nUtil::SetText(ui->lineEditContent, "feature-info-content-text");
	Gui18nUtil::SetText(ui->pushButtonSave, "feature-info-btn-save");
	Gui18nUtil::SetText(ui->pushButtonCancel, "feature-info-btn-cancel");
}

void FireDogFeatureInfo::slots_radioClick() {
    if (ui->radioButtonHex->isChecked()) {
		ui->lineEditContent->setValidator(this->hexValidator);
    }
    else {
		ui->lineEditContent->setValidator(NULL);
    }
}

bool FireDogFeatureInfo::updateFeature(QStringList existsKeys, firedog::Feature* feature) {

    this->isOk = false;

    this->existsKeys = existsKeys;

    QString key = feature->key.c_str();

	ui->lineEditKey->setText(key);

    bool ishex = feature->hex.empty() ? false : true;
    if (feature->hex.empty()&&feature->text.empty()) {
        ishex = true;
    }

    if (ishex) {
		QString content = feature->hex.c_str();
		ui->lineEditContent->setText(content);
		ui->lineEditContent->setValidator(this->hexValidator);

        ui->radioButtonHex->setChecked(true);
        ui->radioButtonText->setChecked(false);
    }
	else {
		QString content = feature->text.c_str();
		ui->lineEditContent->setText(content);
		ui->lineEditContent->setValidator(NULL);

		ui->radioButtonHex->setChecked(false);
		ui->radioButtonText->setChecked(true);
    }

    //this->show();
    this->exec();

    if (isOk) {
        feature->key = ui->lineEditKey->text().toStdString();
        if (ui->radioButtonHex->isChecked())
        {
            feature->hex = ui->lineEditContent->text().toStdString();
            feature->text = "";
        }
		else {
            feature->hex = "";
			feature->text = ui->lineEditContent->text().toStdString();
        }
    }

    return isOk;
}

void FireDogFeatureInfo::slots_save() {
    QString titleStr = Gui18n::GetInstance()->GetConfig("text-message-box-title-warning", "Warning");

	if (ui->lineEditKey->text().isEmpty()) {
		QString messageStr = Gui18n::GetInstance()->GetConfig("feature-info-please-enter-key", "Please enter [Key].");
        QssMessageBox::warn(messageStr, this, titleStr);
        return;
	}
	if (this->existsKeys.contains(ui->lineEditKey->text())) {
		QString messageStr = Gui18n::GetInstance()->GetConfig("feature-info-key-exists", "[Key] is exists.");
		QssMessageBox::warn(messageStr, this, titleStr);
		return;
    }

	QString keyreg = "^";
    keyreg.append(FEATURE_FORMATE_REG_KEY);
    keyreg.append("$");
	QRegExp keyrx(keyreg);
	if (!keyrx.exactMatch(ui->lineEditKey->text())) {
		QString messageStr = Gui18n::GetInstance()->GetConfig("feature-info-key-formate-fail", "[Key] format verification failed.\nYou can enter: 0-9a-zA-Z");
		QssMessageBox::warn(messageStr, this, titleStr);
		return;
    }

	if (ui->lineEditContent->text().isEmpty()) {
		QString messageStr = Gui18n::GetInstance()->GetConfig("feature-info-please-enter-content", "Please enter [Content].");
		QssMessageBox::warn(messageStr, this, titleStr);
		return;
    }

    if (ui->radioButtonHex->isChecked()) {
        QString hexStr = ui->lineEditContent->text();

        QString reg = "^";
        reg.append(FEATURE_FORMATE_REG_HEX);
        reg.append("$");
		QRegExp rx(reg);

		bool ok = rx.exactMatch(hexStr);
        if (!ok) {
			QString messageStr = Gui18n::GetInstance()->GetConfig("feature-info-content-formate-fail", "[Content] format verification failed.\nYou can enter: 0F 9? ?0 ?? [00-FF] [00-02,06-09]");
			QssMessageBox::warn(messageStr, this, titleStr);
			return;
        }
    }
	isOk = true;
    this->hide();
}

void FireDogFeatureInfo::slots_cancel() {
	isOk = false;
    this->hide();
}

FireDogFeatureInfo::~FireDogFeatureInfo()
{
    delete ui;
    if (hexValidator != NULL) {
        delete hexValidator;
        hexValidator = NULL;
    }
}
