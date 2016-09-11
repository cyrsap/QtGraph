#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "globaldefines.h"
#include <QMetaType>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->comboBoxFunction->addItem( "f(x) = sin(x)" );
    ui->comboBoxFunction->addItem( "f(x) = A*(x*x) + B*x + C" );
    ui->comboBoxFunction->addItem( "f(x) = A * sin(x) + B * cos( C*x )" );
    ui->comboBoxFunction->addItem( "f(x) = A*log( B*x )" );
    ui->comboBoxFunction->addItem( "f(x) = A / ( sin(x*x) * B )" );

    FunctionsVec.push_back( FirstFunc );
    FunctionsVec.push_back( SecondFunc );
    FunctionsVec.push_back( ThirdFunc );
    FunctionsVec.push_back( FourthFunc );
    Calculator = new T_Calculator;
    Scene = new QGraphicsScene;
    FirstSendCoord = true;
    Paused = false;
    IsCalcOnline = false;
    ui->graphicsView->setScene( Scene );
    qRegisterMetaType<T_Coord>( "T_Coord" );
    ValuesFrame = new T_ValuesFrame;

    QObject::connect( this,       SIGNAL(runPlot(T_FuncParams)), Calculator, SLOT(LaunchCalc(T_FuncParams)) );
    QObject::connect( Calculator, SIGNAL(SendCoord(T_Coord)), this, SLOT(onSendCoord(T_Coord)) );
    QObject::connect( Calculator, SIGNAL(SendPercent(quint8)), this, SLOT(onGetPercent(quint8)) );
    QObject::connect( this,       SIGNAL(Pause()), Calculator, SLOT(onPause()) );
    QObject::connect( this,       SIGNAL(Break()), Calculator, SLOT(onBreak()) );
    QObject::connect( this,       SIGNAL(ClearData()), Calculator, SLOT(onClearData()) );
    QObject::connect( this,       SIGNAL(SendLoadedDataToCalculator(T_FuncParams&,QVector<T_Coord>&)),
                      Calculator, SLOT(onRecvLoadedData(T_FuncParams&,QVector<T_Coord>&)) );
    QObject::connect( ValuesFrame, SIGNAL(SendLoadedData(T_FuncParams&,QVector<T_Coord>&)),
                      this,       SLOT(onRecvLoadedData(T_FuncParams&,QVector<T_Coord>&)));
}

MainWindow::~MainWindow()
{
    delete Calculator;
    Scene->clear();
    delete Scene;
    delete ValuesFrame;
    delete ui;
}

void MainWindow::on_pushButtonStart_clicked()
{
    if ( ui->pushButtonStart->text() == "New" ) {
        emit ClearData();
        Scene->clear();
        SetControlAccess( true );
        ui->pushButtonStart->setText( "Start" );
        return;
    }

    T_FuncParams Params;
    bool Ok;
    QString Msg;
    Params.A = ui->lineEditA->text().toDouble( &Ok );
    if ( !Ok ) TellWrongParam( Msg );
    Params.B = ui->lineEditB->text().toDouble( &Ok );
    if ( !Ok ) TellWrongParam( Msg );
    Params.C = ui->lineEditC->text().toDouble( &Ok );
    if ( !Ok ) TellWrongParam( Msg );
    Params.From = ui->lineEditFrom->text().toDouble( &Ok );
    if ( !Ok ) TellWrongParam( Msg );
    Params.To = ui->lineEditTo->text().toDouble( &Ok );
    if ( !Ok ) TellWrongParam( Msg );
    Params.Step = ui->lineEditStep->text().toDouble( &Ok );
    if ( !Ok ) TellWrongParam( Msg );

    if ( Params.To - Params.From <= 0 ) {
        TellWrongParam( Msg );
    }
    if ( Params.To - Params.From < Params.Step ) {
        TellWrongParam( Msg );
    }

    Params.FuncIdx = ui->comboBoxFunction->currentIndex();
    Scene->clear();
    FirstSendCoord = true;

    IsCalcOnline = true;
    ui->pushButtonStart->setEnabled( false );
    SetControlAccess( false );
    emit runPlot( Params );
}

void MainWindow::TellWrongParam(QString &aMesg)
{

}

void MainWindow::SetControlAccess(bool aAccess)
{
    ui->controlLayout->setEnabled( aAccess );
    ui->comboBoxFunction->setEnabled( aAccess );
    ui->lineEditA->setEnabled( aAccess );
    ui->lineEditB->setEnabled( aAccess );
    ui->lineEditC->setEnabled( aAccess );
    ui->lineEditFrom->setEnabled( aAccess );
    ui->lineEditStep->setEnabled( aAccess );
    ui->lineEditTo->setEnabled( aAccess );
}

void MainWindow::mousePressEvent(QMouseEvent *event)
{
    if ( IsCalcOnline && !Paused ) {
        return;
    }
    QPoint remapped = ui->graphicsView->mapFromParent( event->pos() );
    if ( ui->graphicsView->rect().contains( remapped ) )
    {
        ValuesFrame->SetParamsAndVec( Calculator->GetParams(), Calculator->GetCoordVec() );
        ValuesFrame->show();
    }
}

void MainWindow::setFinishedState()
{
    IsCalcOnline = false;
    Paused = false;
    ui->pushButtonStart->setText( "Start" );
    ui->pushButtonStart->setEnabled( true );
    ui->pushButtonPause->setText( "Pause" );
    SetControlAccess( true );
}

void MainWindow::setPausedState()
{
    IsCalcOnline = true;
    Paused = true;
    ui->pushButtonStart->setText( "Progress: (unkn)" );
    ui->pushButtonStart->setEnabled( false );
    ui->pushButtonPause->setText( "Continue" );
    SetControlAccess( false );
}

void MainWindow::DrawFromVec(const QVector<T_Coord> &aCoordVec)
{
    if ( !aCoordVec.size() ) {
        //todo
        return;
    }
    PrevCoord = aCoordVec.front();
    Scene->clear();

    if ( aCoordVec.size() < 2 ) {
        return;
    }
    QPen pen(Qt::red);
    for ( int i = 1; i < aCoordVec.size(); i++ ) {
        Scene->addLine( PrevCoord.x, PrevCoord.y, aCoordVec[ i ].x, aCoordVec[ i ].y, pen );
        PrevCoord = aCoordVec[ i ];
    }
}

void MainWindow::onSendCoord(T_Coord aCoord)
{
    QPen pen(Qt::red);
    if ( FirstSendCoord ) {
        PrevCoord = aCoord;
        FirstSendCoord = false;
        return;
    }

    Scene->addLine( PrevCoord.x, PrevCoord.y, aCoord.x, aCoord.y, pen );
    PrevCoord = aCoord;
}

void MainWindow::onGetPercent(quint8 aPercent)
{
    if ( !IsCalcOnline ) return;
    if ( aPercent == 100 ) {
        ui->pushButtonStart->setText( QString( "Start" ) );
        ui->pushButtonStart->setEnabled( true );
        SetControlAccess( true );
        IsCalcOnline = false;
    }
    else {
        ui->pushButtonStart->setText( QString( "Progress (%1%)" ).arg( aPercent ) );
    }
}

// Приём загруженных данных от фрейма
void MainWindow::onRecvLoadedData(T_FuncParams &aParams, QVector<T_Coord> &aCoordVec)
{
    // Определение, что вообще пришло от фрейма, запись данных в окно
    ui->lineEditA->     setText( QString("%1").arg( aParams.A    ) );
    ui->lineEditB->     setText( QString("%1").arg( aParams.B    ) );
    ui->lineEditC->     setText( QString("%1").arg( aParams.C    ) );
    ui->lineEditFrom->  setText( QString("%1").arg( aParams.From ) );
    ui->lineEditTo->    setText( QString("%1").arg( aParams.To   ) );
    ui->lineEditStep->  setText( QString("%1").arg( aParams.Step ) );

    ui->comboBoxFunction->setCurrentIndex( aParams.FuncIdx );

    // Определение, в каком статусе пришли данные - законченные или нет
    if ( aCoordVec.back().x >= aParams.To ) {   // вычисление закончено
        setFinishedState();
    }
    else {
        // вычисление не закончено
        setPausedState();
    }
    DrawFromVec( aCoordVec );

    // Перенаправление данных в калькулятор
    emit SendLoadedDataToCalculator( aParams, aCoordVec );
}

void MainWindow::on_pushButtonPause_clicked()
{
    // todo обработка активности ввода
    if ( IsCalcOnline ) {
        if ( !Paused ) {
            ui->pushButtonPause->setText( "Continue" );
            Paused = true;
            emit Pause();
        }
        else {
            ui->pushButtonPause->setText( "Pause" );
            Paused = false;
            T_FuncParams FakeParams;
            emit runPlot( FakeParams );
        }
    }
}

void MainWindow::on_pushButtonBreak_clicked()
{
    if ( IsCalcOnline ) {
        emit Break();
        IsCalcOnline = false;
        ui->pushButtonStart->setText( "New" );
        ui->pushButtonStart->setEnabled( true );
    }
}
