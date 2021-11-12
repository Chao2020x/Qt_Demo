#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <myzipthread.h>
#include <QSystemTrayIcon>

QT_BEGIN_NAMESPACE
namespace Ui { class Widget; }
QT_END_NAMESPACE

class Widget : public QWidget
{
    Q_OBJECT

public:
    Widget(QWidget *parent = nullptr);
    ~Widget();

private slots:
    void on_btn_select_file_clicked();

    void on_btn_Compressed_clicked();

    void slot_CompressedOK(bool isOK);

    void slot_Over_A_Day();//过了一天

    void slot_textBrowserShowAlarmInfo(const QString &t_alarmInfo);

    void on_btn_auto_yasuo_clicked();

    void slot_SaveConf();
    void iconIsActived(QSystemTrayIcon::ActivationReason);

protected:
    virtual void closeEvent(QCloseEvent *event);


private:
    Ui::Widget *ui;
    MyZipThread *m_CompressedThread;
    QSystemTrayIcon *m_SystemTrayIcon;

    QString m_CurrentCompresseDir;

    void initConf();
};
#endif // WIDGET_H
