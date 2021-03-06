#ifndef __FIREDOG_GUI_LOADINGDIALOG_H__
#define __FIREDOG_GUI_LOADINGDIALOG_H__

#include <qwidget.h>
#include <qtimer.h>
#include <qlabel.h>
#include <qmovie.h>
#include <qframe.h>

class LoadingDialog : public QWidget
{
    Q_OBJECT

public slots:
    void updateTip(QString tipstr);

public:
    explicit LoadingDialog(QWidget* parent = 0);
    ~LoadingDialog();
    void resizeEvent(QResizeEvent* event) override;

signals:
    void loadingEnd();

private:
	QFrame* loading = NULL;
    QMovie* movie = NULL;
    QLabel* label = NULL;
    QFrame* background = NULL;
};

#endif