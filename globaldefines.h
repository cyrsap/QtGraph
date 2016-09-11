#ifndef GLOBALDEFINES_H
#define GLOBALDEFINES_H

#include <QVector>

typedef qreal(*T_Function)(qreal, qreal, qreal, qreal);

#pragma pack(push, 1)
typedef struct {
    int   FuncIdx; // Индекс функции в прерванном вычислении
    qreal A;
    qreal B;
    qreal C;
    qreal From;
    qreal Step;
    qreal To;
} T_FuncParams;
#pragma pack(pop)

typedef struct {
    qreal x;
    qreal y;
} T_Coord;

extern QVector< T_Function > FunctionsVec;

qreal SinFunc( qreal aA, qreal aB, qreal aC, qreal aX );
qreal FirstFunc( qreal aA, qreal aB, qreal aC, qreal aX );
qreal SecondFunc( qreal aA, qreal aB, qreal aC, qreal aX );
qreal ThirdFunc( qreal aA, qreal aB, qreal aC, qreal aX );
qreal FourthFunc( qreal aA, qreal aB, qreal aC, qreal aX );


#endif // GLOBALDEFINES_H
