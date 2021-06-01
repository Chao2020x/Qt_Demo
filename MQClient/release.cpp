/**
关于发布部分的代码
*/
#include <mywidget.h>


void MyWidget::onRelease(){

    foreach (QString var, ReleaseTopic) {
        if(m_client->publish(var,
                             ReleaseText.toUtf8(),
                             ReleaseQOS,
                             ReleaseRetained) == -1)
            QMessageBox::critical(this,
                                  QLatin1String("Error"),
                                  QLatin1String("Could not publish message"));
    }
}
