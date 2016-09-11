#include "T_Calculator.h"
#include <QtConcurrentRun>
#include <QTest>

// Конструктор
T_Calculator::T_Calculator()
{

    CalcStopped = false;
    NeedStop = false;
}

// Деструктор
T_Calculator::~T_Calculator()
{
    CoordVec.clear();
}

// Отрисовка графика
void T_Calculator::DrawFunc(T_FuncParams aParams)
{
    T_Coord CurrentCoord;
    T_FuncParams Params;
    qreal CurrentX;
    quint8 Percent = 0;

    if ( !CalcStopped ) {   // Всё рисуется с нуля
        CoordVec.clear();
        Params = aParams;
        if ( Params.FuncIdx >= FunctionsVec.size() ) {
            return;
        }
        CurrentX = Params.From - Params.Step;
    }
    else {  // Отрисовка была остановлена
        Params = SavedParams;
        CalcStopped = false;
        CurrentX = CoordVec.back().x;
    }

    do {
        CurrentX += Params.Step;
        CurrentCoord.x = CurrentX;
        CurrentCoord.y = FunctionsVec[ Params.FuncIdx ](Params.A, Params.B, Params.C, CurrentX);
        CoordVec.push_back( CurrentCoord );
        emit SendCoord( CurrentCoord );
        Percent = ( CurrentX - Params.From ) * 100 / ( Params.To - Params.From );
        if ( ( Percent == 100 ) && CurrentX < Params.To ) {
            Percent = 99;
        }
        if ( GetStop() ) {
            SetStop( false );
            CalcStopped = true;
            break;
        }
        emit SendPercent( Percent );
        // сделано для более наглядного отображения построения графика, в идеале - nanosleep()
        QTest::qSleep(1);
    }  while ( CurrentX < Params.To );
    SavedParams = Params;
}

void T_Calculator::LaunchCalc(T_FuncParams aParams)
{
    QFuture<void> Future = QtConcurrent::run( this, &T_Calculator::DrawFunc, aParams );
}

void T_Calculator::onPause()
{
    SetStop( true );
}

void T_Calculator::onBreak()
{
    SetStop( true );
    CalcStopped = false;    // заканчиваем подсчёт, при следующем запуске все данные сотрутся
}

void T_Calculator::onClearData()
{
    CoordVec.clear();
    CalcStopped = false;
    SetStop( false );
}

void T_Calculator::onRecvLoadedData(T_FuncParams &aParams, QVector<T_Coord> &aCoordVec)
{
    // Все старые данные удаляются
    CoordVec.clear();

    if ( !aCoordVec.size() ) {
        return;
    }

    // запись новых данных
    SavedParams = aParams;

    for ( int i = 0; i < aCoordVec.size(); i++ ) {
        CoordVec.push_back( aCoordVec[ i ] );
    }

    if ( CoordVec.back().x >= aParams.To ) {    // вычисления закончены
        CalcStopped = false;
        SetStop( false );
    }
    else {  // вычисления ещё не закончены, имитируем остановку
        CalcStopped = true;
        SetStop( false );
    }
}
