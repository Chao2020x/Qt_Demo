#ifndef FRMEMAILTOOL_H
#define FRMEMAILTOOL_H

#include <QWidget>
#include "sendemailthread.h"
namespace Ui
{
    class frmEmailTool;
}
namespace SetLabelColor {
    const QString Color_Green = "QLabel{background-color:rgb(0,255,0);}"; //绿色
    const QString Color_White = "QLabel{background-color:rgb(255,255,255);}"; //白色
    const QString Color_Red   = "QLabel{background-color:rgb(255,0,0);}"; //红色
}

class frmEmailTool : public QWidget
{
	Q_OBJECT

public:
    explicit frmEmailTool(QWidget *parent = 0);
    ~frmEmailTool();

private:
    Ui::frmEmailTool *ui;

private:
	bool check();

private slots:
    void initForm();
    void receiveEmailResult(EmailSendResult *result);

private slots:
	void on_btnSend_clicked();
	void on_btnSelect_clicked();
	void on_cboxServer_currentIndexChanged(int index);
};

#endif // FRMEMAILTOOL_H
