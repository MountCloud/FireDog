#ifndef FIREDOGABOUT_H
#define FIREDOGABOUT_H

#include <QDialog>

#include "Qss/Qss.h"

namespace Ui {
class FireDogAbout;
}

class FireDogAbout : public QssDialog
{
    Q_OBJECT

public:
    explicit FireDogAbout(QWidget *parent = nullptr);
    ~FireDogAbout();

private:
    Ui::FireDogAbout *ui;
};

#endif // FIREDOGABOUT_H
