#include "bottomwidget.h"
#include <QLabel>
#include <QFrame>
#include <QBoxLayout>
#include <QTimer>
#include <QDateTime>
#include <QStyle>
#include <QStyleOption>
#include <QPainter>

BottomWidget::BottomWidget(QWidget *parent) : QWidget(parent)
{
    title = "数据压缩处理软件";
    version = "V1.0";
    author = "深圳市光熠信息科技有限公司";
    currentUserName = "admin";
    currentUserType = "管理员";
    fontName = "Microsoft Yahei";
    fontSize = 9;
    lineFixedWidth = false;

    day = 0;
    hour = 0;
    minute = 0;
    seconds = 0;

    timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(showTime()));
    timer->stop();

    labTitle = 0;
    labAuthor = 0;
    labUser = 0;
    labLive = 0;
    labDateTime = 0;

    labTitleVisible = true;
    labAuthorVisible = false;
    labUserVisible = false;
    labLiveVisible = true;
    labDateTimeVisible = false;
}

BottomWidget::~BottomWidget()
{
    if(timer->isActive()){
        timer->stop();
    }
}

void BottomWidget::paintEvent(QPaintEvent *)
{
    //加上此代码让继承类样式起作用
    QStyleOption o;
    o.initFrom(this);
    QPainter p(this);
    style()->drawPrimitive(QStyle::PE_Widget, &o, &p, this);
}

void BottomWidget::showTime()
{
    seconds++;

    if (seconds == 60) {
        minute++;
        seconds = 0;
        emit sig_Over_A_Minute();
    }

    if (minute == 60) {
        hour++;
        minute = 0;
        emit sig_Over_A_Hour();
    }

    if (hour == 24) {
        day++;
        hour = 0;
        emit sig_Over_A_Day();
    }

    QDateTime now = QDateTime::currentDateTime();
    labLive->setText(QString("已运行: %1天%2时%3分%4秒").arg(day).arg(hour).arg(minute).arg(seconds));
    labDateTime->setText(now.toString("当前时间: yyyy年MM月dd日 HH:mm:ss"));
}

void BottomWidget::setLabTitleVisible(bool labTitleVisible)
{
    this->labTitleVisible = labTitleVisible;
}

void BottomWidget::setLabAuthorVisible(bool labAuthorVisible)
{
    this->labAuthorVisible = labAuthorVisible;
}

void BottomWidget::setLabUserVisible(bool labUserVisible)
{
    this->labUserVisible = labUserVisible;
}

void BottomWidget::setLabLiveVisible(bool labLiveVisible)
{
    this->labLiveVisible = labLiveVisible;
}

void BottomWidget::setLabDateTimeVisible(bool labDateTimeVisible)
{
    this->labDateTimeVisible = labDateTimeVisible;
}

void BottomWidget::setTitle(const QString &title)
{
    if (this->title != title) {
        this->title = title;
        if (labTitle != 0) {
            labTitle->setText(QString("欢迎使用%1 %2").arg(title).arg(version));
        }
    }
}

void BottomWidget::setVersion(const QString &version)
{
    if (this->version != version) {
        this->version = version;
        if (labTitle != 0) {
            labTitle->setText(QString("欢迎使用%1 %2").arg(title).arg(version));
        }
    }
}

void BottomWidget::setAuthor(const QString &author)
{
    if (this->author != author) {
        this->author = author;
        if (labAuthor != 0) {
            labAuthor->setText(QString("版权所有: %1").arg(author));
        }
    }
}

void BottomWidget::setCurrentUserName(const QString &currentUserName)
{
    if (this->currentUserName != currentUserName) {
        this->currentUserName = currentUserName;
        if (labUser != 0) {
            labUser->setText(QString("当前用户: %1【%2】").arg(currentUserName).arg(currentUserType));
        }
    }
}

void BottomWidget::setCurrentUserType(const QString &currentUserType)
{
    if (this->currentUserType != currentUserType) {
        this->currentUserType = currentUserType;
        if (labUser != 0) {
            labUser->setText(QString("当前用户: %1【%2】").arg(currentUserName).arg(currentUserType));
        }
    }
}

void BottomWidget::setFontName(const QString &fontName)
{
    this->fontName = fontName;
}

void BottomWidget::setFontSize(int fontSize)
{
    this->fontSize = fontSize;
}

void BottomWidget::setLineFixedWidth(bool lineFixedWidth)
{
    this->lineFixedWidth = lineFixedWidth;
}

void BottomWidget::start(int interval)
{
    labTitle = new QLabel;
    labTitle->setText(QString("欢迎使用%1 %2").arg(title).arg(version));

    labAuthor = new QLabel;
    labAuthor->setText(QString("版权所有: %1").arg(author));

    labUser = new QLabel;
    labUser->setText(QString("当前用户: %1【%2】").arg(currentUserName).arg(currentUserType));

    labLive = new QLabel;
    labDateTime = new QLabel;
    labDateTime->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    showTime();

    QFrame *line1 = new QFrame;
    line1->setFrameShape(QFrame::VLine);
    line1->setFrameShadow(QFrame::Sunken);

    QFrame *line2 = new QFrame;
    line2->setFrameShape(QFrame::VLine);
    line2->setFrameShadow(QFrame::Sunken);

    QFrame *line3 = new QFrame;
    line3->setFrameShape(QFrame::VLine);
    line3->setFrameShadow(QFrame::Sunken);

    QFrame *line4 = new QFrame;
    line4->setFrameShape(QFrame::VLine);
    line4->setFrameShadow(QFrame::Sunken);

    if (lineFixedWidth) {
        line1->setFixedWidth(1);
        line2->setFixedWidth(1);
        line3->setFixedWidth(1);
        line4->setFixedWidth(1);
    }

    QHBoxLayout *layout = new QHBoxLayout(this);
    layout->setSpacing(6);
    layout->setContentsMargins(6, 6, 6, 6);

    if (labTitleVisible) {
        layout->addWidget(labTitle);
        layout->addWidget(line1);
    }

    if (labAuthorVisible) {
        layout->addWidget(labAuthor);
        layout->addWidget(line2);
    }

    if (labUserVisible) {
        layout->addWidget(labUser);
        layout->addWidget(line3);
    }

    if (labLiveVisible) {
        layout->addWidget(labLive);
        layout->addWidget(line4);
    }

    if (labDateTimeVisible) {
        layout->addWidget(labDateTime);
    }

    QFont font = QFont(fontName, fontSize);

    for(auto each_lab : this->findChildren<QLabel *>()){
        each_lab->setFont(font);
    }

    timer->start(interval);
}
