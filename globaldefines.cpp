#include "globaldefines.h"
#include <qmath.h>

QVector< T_Function > FunctionsVec;

qreal SinFunc( qreal aA, qreal aB, qreal aC, qreal aX )
{
    return qSin( aX );
}

qreal FirstFunc( qreal aA, qreal aB, qreal aC, qreal aX )
{
    return aA * (aX * aX) + aB * aX + aC;
}

qreal SecondFunc( qreal aA, qreal aB, qreal aC, qreal aX )
{
    return aA * qSin( aX ) + aB * qCos( aC * aX );
}

qreal ThirdFunc( qreal aA, qreal aB, qreal aC, qreal aX )
{
    (void)aC;
    return aA * qLn( aB * aX );   // в задании не указано, какой логарифм, пусть будет натуральный
}

qreal FourthFunc( qreal aA, qreal aB, qreal aC, qreal aX )
{
    (void)aC;
    return aA / ( qSin( aX * aX ) * aB );
}
