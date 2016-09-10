#ifndef T_VALUESFRAME_H
#define T_VALUESFRAME_H

#include <QFrame>

namespace Ui {
class T_ValuesFrame;
}

class T_ValuesFrame : public QFrame
{
    Q_OBJECT

public:
    explicit T_ValuesFrame(QWidget *parent = 0);
    ~T_ValuesFrame();

private:
    Ui::T_ValuesFrame *ui;
};

#endif // T_VALUESFRAME_H
