#include "FireDogAbout.h"
#include "ui_FreDogAbout.h"

#include "config.h"
#include "FireDogEditor.h"

FireDogAbout::FireDogAbout(QWidget *parent) :
    QssDialog(parent),
    ui(new Ui::FireDogAbout)
{
    ui->setupUi(this);

    this->ui->labelFireDogEditorVersion->setText(tr(FIREDOG_EDITOR_VERSION));

    this->ui->labelFireDogVesion->setText(ui->labelFireDogVesion->text().append(FIREDOG_VERSION));

	this->ui->labelFireDogFeatureLibraryVesion->setText(ui->labelFireDogFeatureLibraryVesion->text().append(FIREDOG_FEATURE_LIBRARY_VERSION));
}

FireDogAbout::~FireDogAbout()
{
    delete ui;
}
