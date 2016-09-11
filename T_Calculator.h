#ifndef QPLOT_H
#define QPLOT_H

#include <QObject>
#include <QVector>
#include "globaldefines.h"
#include <QMutex>
#include <QMutexLocker>
class T_Calculator : public QObject
{
    Q_OBJECT
private:

    QVector< T_Coord > CoordVec;    // набор координат

    bool CalcStopped;               // Флаг того, что было прервано вычисление
    bool NeedStop;
    QMutex Lock;
    T_FuncParams SavedParams;

    void SetStop( bool aStop ) {
        QMutexLocker Locker( &Lock );
        NeedStop = aStop;
    }

    bool GetStop() {
        QMutexLocker Locker( &Lock );
        return NeedStop;
    }

    void DrawFunc(T_FuncParams aParams);
public:
    T_Calculator();
   ~T_Calculator();
    const QVector< T_Coord > & GetCoordVec() const {
        return CoordVec;
    }
    const T_FuncParams & GetParams() const {
        return SavedParams;
    }

signals:
    void finished();
    void SendCoord( T_Coord );
    void SendPercent( quint8 );

public slots:
    void LaunchCalc( T_FuncParams aParams );
    void onPause();
    void onBreak();
    void onClearData();
    void onRecvLoadedData( T_FuncParams &aParams, QVector<T_Coord> &aCoordVec );

};

#endif // QPLOT_H
