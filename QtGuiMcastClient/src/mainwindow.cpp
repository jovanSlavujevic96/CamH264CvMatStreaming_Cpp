#include <iostream>
#include <sstream>

#include "cudp_mcast_client.h"

#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QLineEdit>
#include <QMessageBox>

#define MAX_UDP_PAYLOAD_SIZE 65535u

MainWindow::MainWindow(QWidget* parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    frame(MAX_UDP_PAYLOAD_SIZE),
    package(&frame)
{
    ui->setupUi(this);
    timer = new QTimer(this);
}

MainWindow::~MainWindow()
{
    delete timer;
    delete ui;
}

void MainWindow::on_pushButton_open_webcam_clicked()
{
    //cap.open(0);
    if (decoder.init() < 0)
    {
        std::cerr << "Error while opening H264 FFMPEG decoder.\n";
        QMessageBox::critical(this, tr("Decoder Init Failed"), tr("Error while opening H264 FFMPEG decoder."));
        std::exit(-1);
    }
    MainWindow::initClient();
    connect(timer, SIGNAL(timeout()), this, SLOT(update_window()));
    timer->start(20);
}

void MainWindow::on_pushButton_close_webcam_clicked()
{
    disconnect(timer, SIGNAL(timeout()), this, SLOT(update_window()));

    cv::Mat image = cv::Mat::zeros(cvFrame.size(),CV_8UC3);

    qt_image = QImage((const unsigned char*) (image.data), image.cols, image.rows, QImage::Format_RGB888);
    ui->label->setPixmap(QPixmap::fromImage(qt_image));
    ui->label->resize(ui->label->pixmap().size());

    client.release();
    client = nullptr;
}

void MainWindow::update_window()
{
    static IClient& client_rtp = *client.get();
    static int32_t rcvBytes;
    static int decodeRet;

    cvFrame = cv::Mat();
    try
    {
        rcvBytes = client_rtp >> &package;
    }
    catch (const CSocketException& e)
    {
        std::cerr << e.what() << std::endl;
        QMessageBox::critical(this, tr("Udp Client Receiving Failed"), tr(e.what()));
        goto error_out;
    }
    decodeRet = decoder.decode((uint8_t*)frame.getData(), rcvBytes, cvFrame);
    if (decodeRet < 0)
    {
        std::cout << "Skipped this frame\n";
        return;
    }
    if (cvFrame.empty())
    {
        goto error_out;
    }
    qt_image = QImage(cvFrame.data, cvFrame.cols, cvFrame.rows, QImage::Format_BGR888);
    ui->label->setPixmap(QPixmap::fromImage(qt_image));
    ui->label->resize(ui->label->pixmap().size());
    return;

error_out:
    MainWindow::on_pushButton_close_webcam_clicked();
    return;
}

void MainWindow::initClient() noexcept(false)
{
    if (!client)
    {
        client = std::make_unique<CUdpMcastClient>("224.0.0.251", 9094);;
        try
        {
            client->initClient();
        }
        catch(const CSocketException& e)
        {
            std::cerr << e.what() << std::endl;
            QMessageBox::critical(this, tr("Udp Client Init Failed"), tr(e.what()));
            std::exit(-1);
        }
    }
}
