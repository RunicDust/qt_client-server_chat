#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QByteArray>

//�����������
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
    , m_nNextBlockSize(0)

{
    ui->setupUi(this);
    //������� ����� QTcpSocket ����� ����
    socket = new QTcpSocket(this);
    //��� ��������� �� �������
    connect(socket, SIGNAL(readyRead()), this, SLOT(newConnection()));
    ui->pushButton->setDisabled(true);
    first=0; // ���� ������� ���������/�����������
}
//����������
MainWindow::~MainWindow()
{
//��������� �����
    socket->close();
    delete ui;
}

//������ ���������
void MainWindow::newConnection()
{
    //������ � ��������� �� ������
    QString recvmessage = "";
    QDataStream in(socket);
    in.setVersion(QDataStream::Qt_4_5);
    for (;;) {
        if (!m_nNextBlockSize) {
            if (socket->bytesAvailable() < sizeof(quint16)) {
                break;
            }
            in >> m_nNextBlockSize;
        }

        if (socket->bytesAvailable() < m_nNextBlockSize) {
            break;
        }
        QString str;
        in >> str;

        recvmessage += str;
        m_nNextBlockSize = 0;
    }
    if(first==0)
    {
        QString clientID=recvmessage.mid(recvmessage.indexOf('=')+1,recvmessage.length()); // ������� ID �� ������
        ui->lineEdit_4->setText(clientID); // ���������� ID � label Your ID
        first=1;
    }
    else
    {
        ui->plainTextEdit->appendPlainText(recvmessage); // ���������� ������ � plainTextEdit
    }
    //*****************************
}

//��� ������� ������ "send"
void MainWindow::on_pushButton_clicked()
{
    //���������� �����
    QString sendmesage;
    sendmesage= ui->lineEdit_3->text() + ":" + ui->lineEdit_2->text(); //�������� � ���� ID:Text

    QByteArray  arrBlock;
    QDataStream out(&arrBlock, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_4_5);
    out << quint16(0) << sendmesage;
    out.device()->seek(0);
    out << quint16(arrBlock.size() - sizeof(quint16));

    socket->write(arrBlock);
}

void MainWindow::on_pushButton_2_clicked()
{
    if(ui->lineEdit->text().length()>5)
    {
        //������������ � �������� IP ������
        socket->connectToHost(ui->lineEdit->text(), 4563);
        ui->pushButton->setDisabled(false);
    }
}
