#include "t_valuesframe.h"
#include "ui_t_valuesframe.h"
#include <QFileDialog>
#include <QFile>

T_ValuesFrame::T_ValuesFrame(QWidget *parent) :
    QFrame(parent),
    ui(new Ui::T_ValuesFrame)
{
    ui->setupUi(this);
    setWindowFlags( Qt::Dialog );
    setWindowModality( Qt::ApplicationModal );
    setFixedSize( 239, 300 );
    setWindowTitle( QString("") );

    // настройка таблицы
    ui->tableWidget->setColumnCount( 2 );
//    ui->tableWidget->setRowCount( 1 );
    ui->tableWidget->setColumnWidth( 1, 100 );
    QTableWidgetItem * Item = new QTableWidgetItem( "X" );
    ui->tableWidget->setHorizontalHeaderItem(0, Item);
    Item = new QTableWidgetItem( "Y" );
    ui->tableWidget->setHorizontalHeaderItem(1, Item);

}

T_ValuesFrame::~T_ValuesFrame()
{
    delete ui;
}

void T_ValuesFrame::SetParamsAndVec( T_FuncParams aParams, QVector<T_Coord> aVec)
{
    Params = aParams;
    CoordVec.clear();
    QTableWidgetItem * Item;
    ClearTable();
    ui->tableWidget->setRowCount( aVec.size() );
    for ( int i = 0; i < aVec.size(); i++ ) {
        Item = new QTableWidgetItem;
        Item->setText( QString("%1").arg( aVec[ i ].x ) );
        ui->tableWidget->setItem( i, 0, Item );
        Item = new QTableWidgetItem;
        Item->setText( QString("%1").arg( aVec[ i ].y ) );
        ui->tableWidget->setItem( i, 1, Item );

        CoordVec.push_back( aVec[ i ] );
    }
    if ( ( aParams.FuncIdx > 3 ) || ( aParams.FuncIdx < 0 ) ) {
        ui->label->setText( QString( "Nothing to show" ) );
    }
    else {
        ui->label->setText( FuncNames[ aParams.FuncIdx ].c_str() );
    }

}

void T_ValuesFrame::on_pushButtonSave_clicked()
{
    QString FileName = QFileDialog::getSaveFileName( this, "Выберите файл для сохранения",
                                                     "");
    SaveToFile( FileName );
}

void T_ValuesFrame::on_pushButtonLoad_clicked()
{
    QString FileName = QFileDialog::getOpenFileName( this, tr("Выберите файл для загрузки"),
                                                     "");
    LoadFromFile( FileName );
    SetParamsAndVec( Params, CoordVec );    // вызов для загрузки значений в таблицу
    emit SendLoadedData( Params, CoordVec );
}

// Сохранение данных в файл из внутренней структуры фрейма
void T_ValuesFrame::SaveToFile(QString aFileName)
{
    T_FileSaveStruct FileSaveStruct;
    FileSaveStruct.A = Params.A;
    FileSaveStruct.B = Params.B;
    FileSaveStruct.C = Params.C;
    FileSaveStruct.From = Params.From;
    FileSaveStruct.To = Params.To;
    FileSaveStruct.Step = Params.Step;
    FileSaveStruct.FuncIdx = Params.FuncIdx;
    FileSaveStruct.CoordQuan = CoordVec.size();
    QFile file( aFileName );
    if ( !file.open( QIODevice::WriteOnly ) ) {
        //todo
        return;
    }
    if ( file.write( (char *)&FileSaveStruct , sizeof( T_FileSaveStruct ) ) != sizeof( T_FileSaveStruct ) ) {
        //todo
        return;
    }
    if ( file.write( (char *)CoordVec.data(), sizeof( CoordVec[ 0 ] ) * FileSaveStruct.CoordQuan ) !=
         sizeof( CoordVec[ 0 ] ) * FileSaveStruct.CoordQuan ) {
        //todo
        return;
    }

}

// Загрузка данных из файла и передача их в главное окно
void T_ValuesFrame::LoadFromFile(QString aFileName)
{
    T_FileSaveStruct FileSaveStruct;
    QFile file( aFileName );
    CoordVec.clear();
    if ( !file.open( QIODevice::ReadOnly ) ) {
        //todo
        return;
    }
    if ( file.read( (char *)&FileSaveStruct , sizeof( T_FileSaveStruct ) ) != sizeof( T_FileSaveStruct ) ) {
        //todo
        return;
    }
    for ( quint64 i = 0; i < FileSaveStruct.CoordQuan; i++ ) {
        T_Coord Coord;
        if ( file.read( (char *)&Coord, sizeof( CoordVec[ i ] ) ) != sizeof ( CoordVec[ i ] ) ) {
            //todo

            return;
        }
        CoordVec.push_back( Coord );
    }
    Params.A = FileSaveStruct.A;
    Params.B = FileSaveStruct.B;
    Params.C = FileSaveStruct.C;
    Params.From = FileSaveStruct.From;
    Params.To = FileSaveStruct.To;
    Params.FuncIdx = FileSaveStruct.FuncIdx;
    Params.Step = FileSaveStruct.Step;
}

void T_ValuesFrame::ClearTable()
{
    ui->tableWidget->clearContents();
    ui->tableWidget->setRowCount( 1 );
}
