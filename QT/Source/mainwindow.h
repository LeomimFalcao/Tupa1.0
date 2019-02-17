#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#include <QSerialPort>
#include <QTime>

#include <QMainWindow>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

     bool openSerialComm(QString* portName);

     QSerialPort serial;

     bool openSerialComm(QString portName);
     void initializeCharts();


     QVector<double> temp_x, temp_y,
                     pre_x, pre_y,
                     umi_x, umi_y;

    double           temp_max = -999, temp_min = 999,
                     pre_max = -999, pre_min = 999,
                     umi_max = -999, umi_min = 999;

     bool alarmeAtivado;

     int counter = 0;
public:
     QString serialAcum = "";


    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:

       void dadosRecebidos();

       void on_comboBoxGate_currentIndexChanged(const QString &arg1);
       void on_comboBoxBRate_currentIndexChanged(const QString &arg1);

       void openGitHub();
       void on_pushButton_2_clicked();

       void on_pushButton_clicked();

private:
    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
