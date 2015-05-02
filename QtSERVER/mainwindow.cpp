#include "mainwindow.h"
#include "ui_mainwindow.h"

//�����������
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
    , m_nNextBlockSize(0)
{
    ui->setupUi(this);

    // ������� ������
    // ��������� �������� ���������� � ���������� ����� �������.
    // ������ ����� ��������� ������ newConnection() ������ ���, ����� ������ �������� ����������� � ��������.
    // ����� ����, ��� ����� ����������� ����� �������� � �������� ������������ QTcpSocket
    server = new QTcpServer(this);
    //���������� ��������
    client_i=0;

}

//����������
MainWindow::~MainWindow()
{
    //��������� �����
    server->close();
    delete ui;
}

//������ �����������
void MainWindow::newConnection()
{

    //��������� ���������� �������� �� �������
    client_i++;
    //������� ����� QTcpSocket ����� ���� ��� �������
    socket[client_i] = new QTcpSocket(this);
    //qDebug() << "ID = " <<client_i;
    socket[client_i]=server->nextPendingConnection(); // ���������� ��������� �� ��������� QTcpSocket
    QString ClientID="ID="+QString::number(client_i); // ������ ����� ���� ��� ID=N

    QByteArray  arrBlock;
    QDataStream out(&arrBlock, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_4_5);
    out << quint16(0) << ClientID;

    out.device()->seek(0);
    out << quint16(arrBlock.size() - sizeof(quint16));

    socket[client_i]->write(arrBlock);// ���������� � ����� ���������� ���������

    //��� ��������� �� ����
    connect(socket[client_i], SIGNAL(readyRead()), this, SLOT(readMessage()));
}

//������ ���������
void MainWindow::readMessage()
{
    int otpr=0; // id �����������
    QString recvmessage = "";
    QString clientID;
    for(int i=1;i<=client_i;i++)
    {
        QDataStream in(socket[i]);
        in.setVersion(QDataStream::Qt_4_5);
        for (;;) {
            if (!m_nNextBlockSize) {
                if (socket[i]->bytesAvailable() < sizeof(quint16)) {
                    break;
                }
                in >> m_nNextBlockSize;
            }

            if (socket[i]->bytesAvailable() < m_nNextBlockSize) {
                break;
            }
            QString str;
            in >> str;

            recvmessage += str;
            otpr=i;
            m_nNextBlockSize = 0;

            //qDebug() <<"2 str = "<<recvmessage;
            clientID=recvmessage.mid(0,recvmessage.indexOf(':')); // ������ �� ������ ID
            //qDebug() <<"3";
            recvmessage=recvmessage.mid(recvmessage.indexOf(':')+1,recvmessage.length()); // ������ �� ������ ����������
            //qDebug() <<"4 ID = " <<clientID;

            //*******************************
            if(clientID.toInt()>0 && clientID.toInt()<=client_i)
            {
                QByteArray  arrBlock;
                QDataStream out(&arrBlock, QIODevice::WriteOnly);
                out.setVersion(QDataStream::Qt_4_5);
                out << quint16(0) << recvmessage;

                out.device()->seek(0);
                out << quint16(arrBlock.size() - sizeof(quint16));

                socket[clientID.toInt()]->write(arrBlock);// ���������� � ����� ���������� ���������
            }
        }

    }
	
	
    //���������� �� ���� � ���� ���������
    if(clientID.toInt()>0 && clientID.toInt()<=client_i)
    {
    ui->plainTextEdit_2->appendPlainText(QString::number(otpr)+" -> "+clientID + "::" + recvmessage);
    }

}

//��� ������� ������ "start server"
void MainWindow::on_pushButton_clicked()
{
    //������� ������������ SIGNAL SLOT ���� ����������� �������
    connect(server,SIGNAL(newConnection()),this, SLOT(newConnection()));
    //��������� ������ (����������� ����� � ������ IP �� ���� 4563)
    if(!server->listen(QHostAddress::Any,4563))
    {
        ui->plainTextEdit->appendPlainText("NO STARTED");
    }
    else
    {
        ui->plainTextEdit->appendPlainText("STARTED");
        ui->plainTextEdit_2->appendPlainText("WAIT MESSAGE");
    }
}
