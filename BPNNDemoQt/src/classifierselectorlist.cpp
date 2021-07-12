#include "inc/classifierselectorlist.h"
#include "inc/classifierselector.h"
#include <QListWidgetItem>
#include <QMouseEvent>
#include <QDebug>
#include <QColorDialog>
ClassifierSelectorList::ClassifierSelectorList(QWidget *)
{
    mColors<<"#F61B00"
         <<"#FEFF29"
        <<"#1B00FC"
       <<"#F99017"
      <<"#50FF29"
     <<"#7B00FD";



}

ClassifierSelectorList::~ClassifierSelectorList()
{


}

void ClassifierSelectorList::addItemByNum(int num)
{
    QListWidgetItem *item=new QListWidgetItem(this);
    this->addItem(item);
    //this->setItemWidget(item,new ClassifierSelector(num,QColor(QColor::colorNames()[num%QColor::colorNames().count()])));
    this->setItemWidget(item,new ClassifierSelector(num,QColor(mColors[num])));
    item->setSizeHint(QSize(0,55));
}



void ClassifierSelectorList::removeItem()
{
    this->takeItem(this->count()-1);

}

ClassifierSelector* ClassifierSelectorList::getItem(int row)
{
    return static_cast<ClassifierSelector*>(this->itemWidget(this->item(row)));

}

void ClassifierSelectorList::setColorByIndex(int idx, QColor color)
{
    ClassifierSelector* item=static_cast<ClassifierSelector*>(this->itemWidget(this->item(idx)));
    item->setColor(color);
    emit colorChanged(idx,color.rgb());
    item->update();

}

void ClassifierSelectorList::mouseDoubleClickEvent(QMouseEvent *event)
{
    if(event->button()==Qt::LeftButton)
    {
        if(this->itemAt(event->pos())!=nullptr)
        {
            int row=this->row(this->itemAt(event->pos()));
            QColor color=QColorDialog::getColor();
            if(color.isValid())
            {
                setColorByIndex(row,color);
            }
        }
    }
}

void ClassifierSelectorList::mousePressEvent(QMouseEvent *event)
{

    if(this->itemAt(event->pos())==nullptr)
    {
        setCurrentRow(-1);
    }

    QListWidget::mousePressEvent(event);
}

