#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "globaldefines.h"
#include <QMetaType>
#include <QMessageBox>

#define MAX_Y_DEFAULT 1
#define MIN_Y_DEFAULT -1

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    // Добавление функций в комбобокс
    ui->comboBoxFunction->addItem( FuncNames[ 0 ].c_str() );
    ui->comboBoxFunction->addItem( FuncNames[ 1 ].c_str() );
    ui->comboBoxFunction->addItem( FuncNames[ 2 ].c_str() );
    ui->comboBoxFunction->addItem( FuncNames[ 3 ].c_str() );

    // добавление функций в общий список
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
    ui->graphicsView->scale( 1, -1 );
    qRegisterMetaType<T_Coord>( "T_Coord" );
    ValuesFrame = new T_ValuesFrame;
    setWindowTitle( QString("") );

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
    if ( !Ok ) {
        ShowMessage( QString("Bad A param") );
        return;
    }
    Params.B = ui->lineEditB->text().toDouble( &Ok );
    if ( !Ok ) {
        ShowMessage( QString("Bad B param") );
        return;
    }
    Params.C = ui->lineEditC->text().toDouble( &Ok );
    if ( !Ok ) {
        ShowMessage( QString("Bad C param") );
        return;
    }
    Params.From = ui->lineEditFrom->text().toDouble( &Ok );
    if ( !Ok ) {
        ShowMessage( QString("Bad From param") );
        return;
    }
    Params.To = ui->lineEditTo->text().toDouble( &Ok );
    if ( !Ok ) {
        ShowMessage( QString("Bad To param") );
        return;
    }
    Params.Step = ui->lineEditStep->text().toDouble( &Ok );
    if ( !Ok ) {
        ShowMessage( QString("Bad Step param") );
        return;
    }

    if ( Params.To - Params.From <= 0 ) {
        ShowMessage( QString("To must be greater than From") );
        return;
    }

    Params.FuncIdx = ui->comboBoxFunction->currentIndex();

    if ( Params.FuncIdx == 2 ) {   // Функция с логарифмом
        if ( Params.From < 0 || Params.To < 0 ) {
            ShowMessage( QString("Logarithmic function must accept only positive X arguments") );
            return;
        }
    }
    Scene->clear();
    MinY = MIN_Y_DEFAULT;
    MaxY = MAX_Y_DEFAULT;
    FirstSendCoord = true;

    IsCalcOnline = true;
    ui->pushButtonStart->setEnabled( false );
    SetControlAccess( false );
    emit runPlot( Params );
}

void MainWindow::ShowMessage(QString aMesg)
{
    QMessageBox Msg(this);
    Msg.setText( aMesg );
    Msg.exec();
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
        ShowMessage( "Received empty Vector in DrawFromVec, must not exec" );
        return;
    }
    PrevCoord = aCoordVec[ 0 ];
    MaxY = MAX_Y_DEFAULT;
    MinY = MIN_Y_DEFAULT;
    Scene->clear();
//    return;
//    DrawCoord();

    if ( aCoordVec.size() < 2 ) {
        return;
    }
    QPen pen(Qt::red);
    for ( int i = 1; i < aCoordVec.size(); i++ ) {
        Scene->addLine( PrevCoord.x, PrevCoord.y, aCoordVec[ i ].x, aCoordVec[ i ].y, pen );
        MaxY = qMax( aCoordVec[ i ].y, MaxY );
        MinY = qMin( aCoordVec[ i ].y, MinY );
        PrevCoord = aCoordVec[ i ];
    }
    DrawCoord();
}

void MainWindow::DrawCoord()
{
    qreal MinX = ui->lineEditFrom->text().toDouble();
    qreal MaxX = ui->lineEditTo->text().toDouble();
    if ( MinX > -1 ) {
        MinX = -1;
    }
    if ( MaxX < 1 ) {
        MaxX = 1;
    }

    Scene->addLine( MinX, 0, MaxX, 0 );
    Scene->addLine( 0, MinY, 0, MaxY );
    Scene->setSceneRect( MinX, MinY, MaxX-MinX, MaxY-MinY );
    ui->graphicsView->fitInView( Scene->sceneRect() );
}

void MainWindow::resizeEvent(QResizeEvent *event)
{
    (void)event;
    ui->graphicsView->fitInView( Scene->sceneRect() );
}

void MainWindow::onSendCoord(T_Coord aCoord)
{
    QPen pen(Qt::red);

    if ( FirstSendCoord ) {
        PrevCoord = aCoord;
        FirstSendCoord = false;
        MaxY = MAX_Y_DEFAULT;
        MinY = MIN_Y_DEFAULT;
        return;
    }

    Scene->addLine( PrevCoord.x, PrevCoord.y, aCoord.x, aCoord.y, pen );
    MinY = qMin( MinY, PrevCoord.y );
    MaxY = qMax( MaxY, PrevCoord.y );
    DrawCoord();
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
    // Вычисление процента для кнопки Progress
    int Percent = ( PrevCoord.x - ui->lineEditFrom->text().toDouble() ) * 100 /
            ( ui->lineEditTo->text().toDouble() - ui->lineEditFrom->text().toDouble());
    if ( Percent != 100 ) {
        ui->pushButtonStart->setText( QString( "Progress (%1%)" ).arg( Percent )  );
    }

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
