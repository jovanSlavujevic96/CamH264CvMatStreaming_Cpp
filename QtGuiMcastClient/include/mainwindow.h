#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <memory>
#include <thread>

#include <QMainWindow>
#include <QTimer>

#include <opencv2/opencv.hpp>

#include "iclient.h"

#include "AvH264_Decoder.h"
#include "H264_Frame.hpp"
#include "frame_package.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void on_pushButton_open_webcam_clicked();
    void on_pushButton_close_webcam_clicked();
    void update_window();

private:
    Ui::MainWindow *ui;

    QTimer *timer;

    cv::Mat cvFrame;
    QImage qt_image;

    std::unique_ptr<IClient> client = nullptr;

    AvH264_Decoder decoder;
    bool bDecoderInitDone;
    H264_Frame frame;
    FramePackage package;

    std::thread updateWindowThread;
    bool bThreadRunning;

    void initClient() noexcept(false);
};

#endif // MAINWINDOW_H
