#ifndef FireDogSwitchLanguage_H
#define FireDogSwitchLanguage_H

#include <QWidget>
#include <QDialog>
#include "ui_FireDogSwitchLanguage.h"

#include "Qss/Qss.h"

namespace Ui {
	class FireDogSwitchLanguageUi : public QssDialog
	{
		Q_OBJECT

	signals:
		void switchSuccess();
	public:
		explicit FireDogSwitchLanguageUi(QWidget* parent = nullptr);
		~FireDogSwitchLanguageUi();

	private:
		Ui::FireDogSwitchLanguageClass* ui;
		int reloadLanguages();
		void showEvent(QShowEvent* event) override;
	private slots:
		void slots_setbtnclick();
	};
}

#endif // FireDogSwitchLanguage_H
