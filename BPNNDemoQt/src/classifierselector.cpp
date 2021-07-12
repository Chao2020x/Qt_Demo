#include "inc/classifierselector.h"
#include "ui_classifierselector.h"

ClassifierSelector::ClassifierSelector(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ClassifierSelector)
{
    ui->setupUi(this);

}
ClassifierSelector::ClassifierSelector(int type,QColor color,QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ClassifierSelector)
{
    mType=type;
    mColor=color;
    ui->setupUi(this);
    ui->type->setType(type);
    ui->color->setColor(color);

}

QColor ClassifierSelector::color() const
{
    return mColor;
}

void ClassifierSelector::setColor(const QColor &color)
{
    mColor = color;
    ui->color->setColor(mColor);
}


ClassifierSelector::~ClassifierSelector()
{
    delete ui;
}
