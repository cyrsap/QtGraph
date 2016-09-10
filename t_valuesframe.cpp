#include "t_valuesframe.h"
#include "ui_t_valuesframe.h"

T_ValuesFrame::T_ValuesFrame(QWidget *parent) :
    QFrame(parent),
    ui(new Ui::T_ValuesFrame)
{
    ui->setupUi(this);
}

T_ValuesFrame::~T_ValuesFrame()
{
    delete ui;
}
