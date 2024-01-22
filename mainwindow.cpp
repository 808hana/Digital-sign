#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "key_functions.h"
#include <QFileDialog>
#include  "QLabel"
#include <QFile>
#include <QFileInfo>
#include <QTextStream>


MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent)
{
    setupUi(this);
    connect(otevri, &QPushButton::clicked, this, &MainWindow::open);
    connect(generuj, &QPushButton::clicked, this, &MainWindow::saveKey);
    connect(hash, &QPushButton::clicked, this, &MainWindow::hashFun);
    connect(check, &QPushButton::clicked, this, &MainWindow::checkHash);
}

QString MainWindow::OpenFile(QString dialogName, QString fileType){
    QFileDialog dialog(this);
    dialog.setWindowTitle(dialogName);
    dialog.setFileMode(QFileDialog::ExistingFile);
    dialog.setNameFilter("All Files (*.*);; My Files " + fileType);
    if (dialog.exec()) {
        QStringList selectedFiles = dialog.selectedFiles();
        if (!selectedFiles.isEmpty()) {
            QString selectedFile = selectedFiles.first();
            return selectedFile;
        }
    }
    return QString();
}

QString MainWindow::SaveFile(QString dialogName, QString fileType){
    QFileDialog dialog(this);
    dialog.setFileMode(QFileDialog::AnyFile);
    dialog.setNameFilter("All Files (*.*);; My Files " + fileType);

    QString selectedFile = dialog.getSaveFileName(this, dialogName, QString(), fileType);

    if (!selectedFile.isEmpty() && !selectedFile.endsWith(fileType, Qt::CaseInsensitive)) {
        selectedFile.append(fileType);
    }

    return selectedFile;
}

void MainWindow::WriteToFile(QFile file, std::string content){
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        std::cerr<<"error"<<std::endl;
        return;
    }
    QTextStream out(&file);
    out << QString::fromStdString(content) << Qt::endl;
    file.close();
}

void MainWindow::open() {
    QString name = OpenFile("Choose File", "(*.txt)");

    if (!name.isEmpty()) {
        QFile file(name);
        if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
            QTextStream in(&file);
            QString data = in.readAll();
            file.close();

            this->fileContent = data;

            textEdit_3->setWordWrap(QTextOption::WrapAtWordBoundaryOrAnywhere);
            textEdit_3->setText("PATH TO FILE YOU CHOSE:\n" + name);            
        }
    }
}

QString MainWindow::OpenKeys(QString dialogName, QString key_type) {
    QString name = OpenFile(dialogName, key_type);

    if (!name.isEmpty()) {
        QFile file(name);
        if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
            QTextStream in(&file);
            QString key = in.readAll();
            file.close();
            return key;
        }
    }
    return "error";
}

void MainWindow::saveKey() {
    key_functions kf;

    boost::multiprecision::cpp_int q = kf.prime();
    std::cerr<<q<<std::endl;
    boost::multiprecision::cpp_int p = kf.prime();
    if(q==p){
        p = kf.prime();
    }
    std::cerr<<p<<std::endl;

    boost::multiprecision::cpp_int n = kf.find_n(q, p);
    std::cerr<<n<<std::endl;

    boost::multiprecision::cpp_int fi = kf.find_fi(q, p);
    std::cerr<<fi<<std::endl;

    boost::multiprecision::cpp_int e = kf.search_e(fi);
    std::cerr<<e<<std::endl;

    boost::multiprecision::cpp_int d = kf.search_d(e, fi);
    std::cerr<<d<<std::endl;

    std::string private_key = kf.encode(d);
    std::cerr<<private_key<<std::endl;
    WriteToFile(SaveFile("Private key", ".priv"), private_key);

    std::string public_key = kf.encode(e);
    std::cerr<<public_key<<std::endl;
    WriteToFile(SaveFile("Public key", ".pub"), public_key);

    std::string shared_key = kf.encode(n);
    std::cerr<<shared_key<<std::endl;
    WriteToFile(SaveFile("Shared key", ".shared"), shared_key);

}

void MainWindow::hashFun() {
    key_functions kf;
    //hash the file content
    QString file_hash = kf.hash(this->fileContent);
    //qDebug()<<file_hash;

    QString private_key = OpenKeys("Private key", ".priv");
    boost::multiprecision::cpp_int decodedPrivKey = kf.decode(private_key);

    QString shared_key = OpenKeys("Shared key", ".shared");
    boost::multiprecision::cpp_int decodedSharedKey = kf.decode(shared_key);

    // hex to padded binary
    QString hash_binary = kf.hexToPaddedBinary(file_hash);

    //binary list of 40
    QList<QString> binary_blocks = kf.splitToBlocks(hash_binary);

    QVector<boost::multiprecision::cpp_int> int_blocks = kf.binToInt(binary_blocks);

    //cipher
    QVector<boost::multiprecision::cpp_int> cipher_list = kf.block_cipher(int_blocks, decodedPrivKey, decodedSharedKey);

    //join list
    QString str_cipher = kf.list_to_string(cipher_list);

    //encode
    std::string encode_cipher = kf.encode_string(str_cipher);

    //save
    WriteToFile(SaveFile("Hash", ".sign"), encode_cipher);

}

void MainWindow::checkHash() {
    key_functions kf;
    QString file_hash = kf.hash(this->fileContent);
    qDebug()<<file_hash;


    QString sign = OpenKeys("Select hash", ".sign");
    //decode hash
    QVector<boost::multiprecision::cpp_int> decoded_sign = kf.decode_sign(sign);

    //public, shared key
    QString public_key = OpenKeys("Public key", ".pub");
    boost::multiprecision::cpp_int decodedPubKey = kf.decode(public_key);

    QString shared_key = OpenKeys("Shared key", ".shared");
    boost::multiprecision::cpp_int decodedSharedKey = kf.decode(shared_key);


    QVector<boost::multiprecision::cpp_int> cipher_list = kf.block_cipher(decoded_sign, decodedPubKey, decodedSharedKey);

    QVector<QString> bin_blocks = kf.IntToBin(cipher_list);
    QString str_bin = kf.concatenateBinaryBlocks(bin_blocks);
    QString str_hex = kf.binToHex(str_bin);

    //compare two hashes
    qDebug()<<str_hex;

    //change the text
    if(file_hash == str_hex){
        vysledek->setText("MATCH");
    }
    else{
        vysledek->setText("NO MATCH");
    }
}

