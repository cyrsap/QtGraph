#include "globaldefines.h"
#include <math.h>

QVector< T_Function > FunctionsVec;

const std::string FuncNames[] = {
    "f(x) = A*(x*x) + B*x + C",
    "f(x) = A * sin(x) + B * cos( C*x )",
    "f(x) = A*log( B*x )",
    "f(x) = A / ( sin(x*x) * B )"
};

qreal FirstFunc( qreal aA, qreal aB, qreal aC, qreal aX )
{
    return aA * (aX * aX) + aB * aX + aC;
}

qreal SecondFunc( qreal aA, qreal aB, qreal aC, qreal aX )
{
    return aA * sin( aX ) + aB * cos( aC * aX );
}

qreal ThirdFunc( qreal aA, qreal aB, qreal aC, qreal aX )
{
    (void)aC;
    return aA * log( aB * aX );   // в задании не указано, какой логарифм, пусть будет натуральный
}

qreal FourthFunc( qreal aA, qreal aB, qreal aC, qreal aX )
{
    (void)aC;
    return aA / ( sin( aX * aX ) * aB );
}
