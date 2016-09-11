#ifndef T_VALUESFRAME_H
#define T_VALUESFRAME_H

#include <QFrame>
#include "globaldefines.h"
#include <QVector>

namespace Ui {
class T_ValuesFrame;
}

#pragma pack(push,1)
typedef struct {
    int     FuncIdx;
    qreal A;
    qreal B;
    qreal C;
    qreal From;
    qreal To;
    qreal Step;
    quint64 CoordQuan;
    T_Coord Coords[];
} T_FileSaveStruct;
#pragma pack(pop)

class T_ValuesFrame : public QFrame
{
    Q_OBJECT

public:
    explicit T_ValuesFrame(QWidget *parent = 0);
    ~T_ValuesFrame();
    void SetParamsAndVec(T_FuncParams aParams, QVector<T_Coord> aVec );


private slots:
    void on_pushButtonSave_clicked();

    void on_pushButtonLoad_clicked();

private:
    Ui::T_ValuesFrame *ui;
    T_FuncParams Params;
    QVector< T_Coord > CoordVec;
    void SaveToFile( QString aFileName );
    void LoadFromFile( QString aFileName );
    void ClearTable();

signals:
    void SendLoadedData( T_FuncParams &Params, QVector<T_Coord> &CoordVec );
};

#endif // T_VALUESFRAME_H
