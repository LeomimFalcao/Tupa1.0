#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QSerialPortInfo>

//https://www.qcustomplot.com

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    connect(ui->actionAcessar_Github,SIGNAL(triggered()), this, SLOT(openGitHub()));


    QList<QSerialPortInfo> ports = QSerialPortInfo::availablePorts();
    foreach (QSerialPortInfo port, ports) {
        ui->comboBoxGate->addItem(port.portName());
    }
    if(ports.size() == 1)
            openSerialComm(ports[0].portName());

    ui->comboBoxBRate->addItem("9600");
    ui->comboBoxBRate->addItem("115200");

    //openSerialComm("COM5");


    initializeCharts();

    connect(&serial, SIGNAL(readyRead()),this,SLOT(dadosRecebidos()));



}

void MainWindow::openGitHub(){
    QDesktopServices::openUrl(QUrl("https://github.com/LeomimFalcao/Tupa1.0"));
}
void MainWindow::dadosRecebidos(){
    QString dados = serial.readAll();

    //qDebug(qUtf8Printable(serialAcum));

    if(dados.contains("}")){

        QString msg = serialAcum + dados.split("}")[0];;
        serialAcum = dados.split("}")[1];

        QStringList valores = msg.split(",");
        double t = valores[0].split(":")[1].toDouble();
        double u = valores[1].split(":")[1].toDouble();
        double p = valores[2].split(":")[1].toDouble();

        qDebug(qUtf8Printable("a: "+valores[0].split(":")[1]));
        qDebug(qUtf8Printable("b: "+valores[1].split(":")[1]));
        qDebug(qUtf8Printable("c: "+valores[2].split(":")[1]));
        qDebug(qUtf8Printable("\n"));

        ui->labelTemp->setText(valores[0].split(":")[1] + " °C");
        ui->labelUmi->setText(valores[1].split(":")[1] + " %");
        ui->labelPre->setText(valores[2].split(":")[1] + " atm");

        //-------------------------------------------------------

        temp_y.push_back(t);
        temp_x.push_back(counter*5);

        if(temp_y.size() > 60){
            temp_y.erase(temp_y.begin(),temp_y.begin()+1);
            temp_x.erase(temp_x.begin(),temp_x.begin()+1);

            ui->chartTemp->xAxis->setRange(counter-60, counter);
        }


        ui->chartTemp->graph(0)->setData(temp_x, temp_y);
        ui->chartTemp->replot();

        //-------------------------------------------------------

        umi_y.push_back(u);
        umi_x.push_back(counter*5);

        if(umi_y.size() > 60){
            umi_y.erase(umi_y.begin(),umi_y.begin()+1);
            umi_x.erase(umi_x.begin(),umi_x.begin()+1);

            ui->chartUmi->xAxis->setRange(counter-60, counter);
        }


        ui->chartUmi->graph(0)->setData(umi_x, umi_y);
        ui->chartUmi->replot();


        //-------------------------------------------------------


        pre_y.push_back(p);
        pre_x.push_back(counter*5);

        if(pre_y.size() > 60){
            pre_y.erase(pre_y.begin(),pre_y.begin()+1);
            pre_x.erase(pre_x.begin(),pre_x.begin()+1);

            ui->chartPre->xAxis->setRange(counter-60, counter);
        }


        ui->chartPre->graph(0)->setData(pre_x, pre_y);
        ui->chartPre->replot();

        counter++;

    }
    else
        serialAcum = serialAcum + dados;



}

void MainWindow::initializeCharts(){

    /************************************************/


    ui->chartTemp->addGraph();
    ui->chartTemp->graph(0)->setBrush(QBrush(QColor(0, 0, 255, 20)));
    ui->chartTemp->xAxis->setLabel("Segundos");
    ui->chartTemp->yAxis->setLabel("°C");

    ui->chartTemp->xAxis->setRange(-1, 100);
    ui->chartTemp->yAxis->setRange(20, 40);

    /************************************************/
    ui->chartPre->addGraph();
    ui->chartPre->graph(0)->setData(pre_x, pre_y);
    ui->chartPre->graph(0)->setBrush(QBrush(QColor(0, 0, 255, 20)));

    ui->chartPre->xAxis->setLabel("Segundos");
    ui->chartPre->yAxis->setLabel(" atm");

    ui->chartPre->xAxis->setRange(-1, 100);
    ui->chartPre->yAxis->setRange(0.7, 1.2 );

    ui->chartPre->replot();

    /************************************************/
    ui->chartUmi->addGraph();
    ui->chartUmi->graph(0)->setData(umi_x, umi_y);
    ui->chartUmi->graph(0)->setBrush(QBrush(QColor(0, 0, 255, 20)));

    ui->chartUmi->xAxis->setLabel("Segundos");
    ui->chartUmi->yAxis->setLabel("%");

    ui->chartUmi->xAxis->setRange(-1, 100);
    ui->chartUmi->yAxis->setRange(40, 95);

    ui->chartUmi->replot();

}



bool MainWindow::openSerialComm(QString portName){

    serial.setPortName(portName);
    serial.setBaudRate(QSerialPort::Baud9600);

    if(serial.open(QIODevice::ReadWrite)){
        return true;
    }
    else
        return false;
}



MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::on_comboBoxGate_currentIndexChanged(const QString &arg1)
{
    /*qDebug(qUtf8Printable(arg1));
    serial.setPortName(arg1);

    serial.open(QIODevice::ReadWrite);*/
}

void MainWindow::on_comboBoxBRate_currentIndexChanged(const QString &arg1)
{
    /*qDebug(qUtf8Printable(arg1));
    if(arg1 == "9600")
        serial.setBaudRate(QSerialPort::Baud9600);
    else if(serial.setBaudRate(QSerialPort::Baud115200))
        serial.setBaudRate(QSerialPort::Baud115200);

    serial.open(QIODevice::ReadWrite);*/
}

void MainWindow::on_pushButton_2_clicked()
{
    QMessageBox msgBox;
    msgBox.setWindowTitle("Conexão");
    msgBox.setText("Conectado à porta " + (ui->comboBoxGate->currentText()));
    msgBox.setStandardButtons(QMessageBox::Ok);
    msgBox.setIcon(QMessageBox::Icon::Information);

    serial.close();

    serial.setPortName(ui->comboBoxGate->currentText());
    QString baudRate = ui->comboBoxBRate->currentText();

    if(baudRate == "9600")
        serial.setBaudRate(QSerialPort::Baud9600);
    else if(baudRate == "115200")
        serial.setBaudRate(QSerialPort::Baud115200);

    if(serial.open(QIODevice::ReadWrite)){
        msgBox.exec();
    }

}

void MainWindow::on_pushButton_clicked()
{
    if(serial.isOpen()){
        alarmeAtivado = !alarmeAtivado;

        if(alarmeAtivado){
            serial.write("0");
            ui->pushButton->setText("Emergência Desativada");
            ui->pushButton->setStyleSheet("color: rgb(0, 0, 255); background-color: rgba(85, 170, 255, 150);");
        }
        else{
            serial.write("1");
            ui->pushButton->setText("Emergência Ativada");
            ui->pushButton->setStyleSheet("color: rgb(255, 0, 0); background-color: rgba(255, 0, 0, 100);");

        }
    }

}
