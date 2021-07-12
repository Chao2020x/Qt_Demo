#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
class QAbstractButton;
class ClassifierController;
class QDoubleSpinBox;
namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    int getClassifierNum();
    double getLearnRate();
    QColor getClassifierColor(int type);
    int getHiddenLayerCnt();
    int getHiddenNodeCntByNum(int num);
    int getClassifierCnt();
    bool trainByTime();
    int getTrainTime();
    double getTrainErr();
    void setCurrentTime(int time);
    void setCurrentErr(double err);
    ClassifierController* getController();
    QDoubleSpinBox *getSbLearnRate();

private:
    Ui::MainWindow *ui;
    void setHiddenLayerCnt(int);
    void preStartTrain();
    void postEndTrain();
    QPalette mDefBtnPalette;
    QPalette mHighlightBtnPalette;

signals:
    void sigResetTrainCnt();
    void sigPause(int);
    void sigStop();
    void sigResetSurface();
    void sigClearSamples();

public slots:
    void hiddenLayerCountChanged(int i);
    void trainSettingBtnClicked(QAbstractButton*);
    void onStart();
    void onStartByCond();
    void onClassCntChanged(int cnt);
    void onPause();
    void onStop();
    void onResetSurface();
    void onResetTrainCnt();
    void afterStopTrain();
    void onClearSamples();
    void onActionSaveTrigged();
    void onActionOpenTrigged();


};

#endif // MAINWINDOW_H
