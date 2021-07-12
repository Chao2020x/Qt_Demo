#ifndef HLNCNTSELECTOR_H
#define HLNCNTSELECTOR_H
// HLN means Hidden Layer Neurals
#include <QWidget>

namespace Ui {
class HLNCntSelector;
}

class HLNCntSelector : public QWidget
{
    Q_OBJECT

public:
    explicit HLNCntSelector(int num,QWidget *parent = nullptr);
    ~HLNCntSelector();

    int num() const;
    void setNum(int num);

    int count();
    void setCount(int cnt);

private:
    Ui::HLNCntSelector *ui;
    int mNum;
};

#endif // HLNCNTSELECTOR_H
