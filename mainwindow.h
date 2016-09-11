#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QThread>
#include "T_Calculator.h"
#include "globaldefines.h"
#include <QGraphicsScene>
#include <QMouseEvent>
#include "t_valuesframe.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void on_pushButtonStart_clicked();

    void on_pushButtonPause_clicked();

    void on_pushButtonBreak_clicked();

private:
    Ui::MainWindow *ui;
    T_Calculator *Calculator;
    QGraphicsScene *Scene;
    T_Coord PrevCoord;
    T_ValuesFrame * ValuesFrame;
    bool FirstSendCoord;    // Флаг того, что от калькулятора должны прийти первые координаты
    bool Paused;            // Флаг того, что вычисления приостановлены
    bool IsCalcOnline;      // Флаг того, что вычисления ведутся (могут быть приостановлены)

    qreal MaxY;
    qreal MinY;

    void TellWrongParam(QString aMesg );
    void SetControlAccess( bool aAccess );
    virtual void mousePressEvent(QMouseEvent *event);
    void setFinishedState();
    void setPausedState();
    void DrawFromVec( const QVector<T_Coord> &aCoordVec );
    void DrawCoord();
    void resizeEvent( QResizeEvent * event );
signals:
    void runPlot( T_FuncParams aParams );
    void Pause();
    void Break();
    void ClearData();
    void SendLoadedDataToCalculator( T_FuncParams &aParams, QVector<T_Coord> &aCoordVec );

public slots:
    void onSendCoord( T_Coord aCoord );
    void onGetPercent( quint8 aPercent );
    void onRecvLoadedData( T_FuncParams &aParams, QVector<T_Coord> &aCoordVec );

};

#endif // MAINWINDOW_H
