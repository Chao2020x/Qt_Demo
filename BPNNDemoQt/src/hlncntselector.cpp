#include "inc/hlncntselector.h"
#include "ui_hlncntselector.h"

HLNCntSelector::HLNCntSelector(int num,QWidget *parent) :
    QWidget(parent),
    ui(new Ui::HLNCntSelector),
    mNum(num)
{
    ui->setupUi(this);
    QString s= QStringLiteral("第%1层").arg(num+1);
    ui->lblNum->setText(s);
}

HLNCntSelector::~HLNCntSelector()
{
    delete ui;
}

int HLNCntSelector::num() const
{
    return mNum;
}

void HLNCntSelector::setNum(int num)
{
    mNum = num;

}

int HLNCntSelector::count()
{

    return ui->sbCnt->value();

}

void HLNCntSelector::setCount(int cnt)
{
    ui->sbCnt->setValue(cnt);
}
