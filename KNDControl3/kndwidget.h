#ifndef KNDWIDGET_H
#define KNDWIDGET_H

#include <QWidget>
#include <QThread>
#include <QtWidgets/QLabel>
#include "class_knd_control.h"

QT_BEGIN_NAMESPACE
namespace Ui
{
    class KNDWidget;
}
QT_END_NAMESPACE

struct ControlInfo
{
    int s_relay_ID;
    bool s_isOpenOrClose;
    ControlInfo(int id, bool isOpen) {
        s_relay_ID = id;
        s_isOpenOrClose = isOpen;
    }
};

class KNDWidget : public QWidget
{
    Q_OBJECT

public:
    KNDWidget(QWidget *parent = nullptr);
    ~KNDWidget();

private slots:
    void on_btn_open_clicked();
    void slot_tcpISConnectInfo(const QString &t_ip);
    void slot_tcpISConnectState(bool t_isConnect);
    void slot_tcp22BBJControlInfo(const QString &text);
    void slot_AddBBJControlInfoList22Show(int relay_ID,bool isOpenOrClose);

signals:
    void sig_add_warning_len(double len);

private:
    Ui::KNDWidget *ui;
    Class_KND_Control *m_KND_Control_Do;
    QThread *m_KND_Control_Thread;
    QList<ControlInfo> m_listBJControlInfo;
    QList<QLabel *> m_listLabelBJControlInfo;
    void CreateKNDControlThread22ConnectSignalSlot();
    void BBJControlInfoList22Show();
};
#endif // KNDWIDGET_H
