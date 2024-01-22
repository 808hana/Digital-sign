#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "ui_mainwindow.h"
#include <QFile>

/*QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    Ui::MainWindow *ui;

private slots:
    void open();
    void saveKey();
    void hashFun();
    void checkHash();
};
#endif // MAINWINDOW_H*/
class MainWindow : public QMainWindow, private Ui::MainWindow {
    Q_OBJECT
    QString fileContent;

public:
    MainWindow(QWidget *parent = nullptr);
    QString OpenFile(QString dialogName, QString fileType);
    QString OpenKeys(QString dialogName, QString fileType);
    QString SaveFile(QString dialogName, QString fileType);
    void WriteToFile(QFile file, std::string content);

private slots:
    void open();
    void saveKey();
    void hashFun();
    void checkHash();
};

#endif // MAINWINDOW_H

