#include "FireDogSwitchLanguage.h"

#include "Qss/Qss.h"
#include "gui18n.h"

using namespace Ui;

FireDogSwitchLanguageUi::FireDogSwitchLanguageUi(QWidget *parent) :
    QssDialog(parent),
    ui(new Ui::FireDogSwitchLanguageClass)
{
	ui->setupUi(this);
    //this->setWindowFlags(Qt::WindowCloseButtonHint | Qt::Dialog);
    //this->setFixedSize(this->width(), this->height());
    //this->setModal(true);

    //i18n
	QString i18nlanguagetitle = Gui18n::GetInstance()->GetConfig("switch-language-window-title", "Language");
	QString i18nlablelanguage = Gui18n::GetInstance()->GetConfig("switch-language-lable-language", "Language:");
	QString i18nlablenowlanguage = Gui18n::GetInstance()->GetConfig("switch-language-lable-now-language", "NowLanguage:");
    QString i18nbuttonsetlanguage = Gui18n::GetInstance()->GetConfig("switch-language-button-set-language", "SetLanguage");

    this->setWindowTitle(i18nlanguagetitle);
    ui->labelLanguage->setText(i18nlablelanguage);
    ui->pushButtonSetLanguage->setText(i18nbuttonsetlanguage);
    ui->labelNowLanguageMsg->setText(i18nlablenowlanguage);
    connect(ui->pushButtonSetLanguage, SIGNAL(clicked()), this, SLOT(slots_setbtnclick()));

    this->hide();
}

void FireDogSwitchLanguageUi::showEvent(QShowEvent* event) {
    ui->pushButtonSetLanguage->setEnabled(false);

    int languageCount = reloadLanguages();
    if (languageCount == 0) {
		QString mbtitle = Gui18n::GetInstance()->GetConfig("text-message-box-title-warning", "Warning");
		QString mbcontent = Gui18n::GetInstance()->GetConfig("switch-language-message-no-language-items", "The list of language options is empty.");
        QssMessageBox::warn(mbcontent, this, mbtitle);
	}
	if (languageCount > 0) {
        ui->pushButtonSetLanguage->setEnabled(true);
	}
}

int FireDogSwitchLanguageUi::reloadLanguages() {

	ui->comboBoxLanguage->clear();
    QString nowLanguage = Gui18n::GetInstance()->GetNowLanguage();
    ui->labelNowLanguage->setText(nowLanguage);

	QVector<QString> languages = Gui18n::GetInstance()->GetLanguages();
    int result = languages.size();
    if (result > 0) {
        for (int i = 0; i < result; i++) {
            QString language = languages[i];
            ui->comboBoxLanguage->addItem(language);
        }
        if (languages.contains(nowLanguage)) {
            ui->comboBoxLanguage->setCurrentText(nowLanguage);
        }
    }

    return result;
}

void FireDogSwitchLanguageUi::slots_setbtnclick() {
    QString setlanguageStr = ui->comboBoxLanguage->currentText();
    QString nowLanguage = Gui18n::GetInstance()->GetNowLanguage();
    if (setlanguageStr == nowLanguage) {
		QString mbtitle = Gui18n::GetInstance()->GetConfig("text-message-box-title-information", "Tips");
		QString mbcontent = Gui18n::GetInstance()->GetConfig("switch-language-message-no-changed", "The set language has not changed from the current language.");
        QssMessageBox::information(this, mbtitle, mbcontent);
        return;
    }

	QString mbtitle = Gui18n::GetInstance()->GetConfig("text-message-box-title-information", "Tips");
	QString mbcontent = Gui18n::GetInstance()->GetConfig("switch-language-message-update-language-tips", "Changing the language will restart the application.");
    QssMessageBox::information(this, mbtitle, mbcontent);

    this->hide();
    Gui18n::GetInstance()->SetLanguage(setlanguageStr);
    emit switchSuccess();
}

FireDogSwitchLanguageUi::~FireDogSwitchLanguageUi()
{
	delete ui;
}
