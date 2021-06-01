/**
关于订阅部分的代码
*/
#include <mywidget.h>

void MyWidget::onSubscribe(){
//    m_subscr = ;
    foreach (QString var, SubscribeTopic) {
        if (!m_client->subscribe(var,SubscribeQOS)) {
            QMessageBox::critical(this, QLatin1String("Error"),
                                  QLatin1String("Could not subscribe. Is there a valid connection?"));
            return;
        }
    }
}
