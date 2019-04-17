
#include "ui_app_aes.h"
#include "QFileDialog"
#include "QFile"
#include "QDebug"
#include "QMessageBox"
#include "QDataStream"
#include "QTextCodec"
#include "QByteArray"
#include "QFileInfo"
#include <QRandomGenerator>
#include "QTime"
#include <QClipboard>




#include "app_aes.h"

#define Utf(br) QString::fromUtf8(br)

extern "C" void AES_ECB_encrypt(const uint8_t* input, const uint8_t* key, uint8_t *output, const uint32_t length);
extern "C" void AES_ECB_decrypt(const uint8_t* input, const uint8_t* key, uint8_t *output, const uint32_t length);
extern "C" void AES_CBC_encrypt_buffer(uint8_t* output, uint8_t* input, uint32_t length, const uint8_t* key, const uint8_t* iv);
extern "C" void AES_CBC_decrypt_buffer(uint8_t* output, uint8_t* input, uint32_t length, const uint8_t* key, const uint8_t* iv);
extern "C" void AES_init(uint8_t AES256);

QByteArray buf;
QByteArray ba;
uint filesize;
uint filesize1;
QTime midnight(0,0,0);
static QString key_str, iv_str;
uchar key_[32];
uchar iv_[16];             //initialization vector (IV)


APP_AES::APP_AES(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::APP_AES)
{
    ui->setupUi(this);
    ui->radioButton_256bit->setChecked(true);
    ui->radioButton_CBC->setChecked(true);
}

APP_AES::~APP_AES()
{
    delete ui;
}

static void test_encrypt_ecb(void)
{

  //uint8_t buffer[16];

  AES_ECB_encrypt( (uchar *)ba.data(), key_, (uchar *)buf.data(), filesize1);

 }

static void test_decrypt_ecb(void)
{

  AES_ECB_decrypt((uchar *)ba.data(), key_, (uchar *)buf.data(), filesize1);

}

static void test_decrypt_cbc(void)
{

  //uint8_t  iv[]  = { 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f };

  AES_CBC_decrypt_buffer((uchar *)buf.data(), (uchar *)ba.data(), filesize1, key_, iv_);

}




static void test_encrypt_cbc(void){


      //uint8_t iv[]  = { 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f };

      //uint8_t buffer[64];

      AES_CBC_encrypt_buffer((uchar *)buf.data(), (uchar *)ba.data(), filesize1, key_, iv_);

    }



void APP_AES::on_pushButton_clicked()
{
    QString str;

    QString fileName_in = QFileDialog::getOpenFileName(this);
    QString fileName_out ;
    QString file_ext, file_name;
    QFile file_in(fileName_in);


    key_str = ui->lineEdit->text();
    if (this->ui->radioButton_256bit->isChecked()) {
        while (key_str.size() < 64) key_str.append("0");
    }
      else key_str.resize(32);
    QString key_str_t = key_str;
#ifdef QT_DEBUG
    qDebug() <<  "key" << key_str;
#endif
    bool ok;
    str.resize(2);
    for (uchar i=0;i<32;i++){
        str = key_str_t.left(2);
        key_str_t.remove(0, 2 );
        uchar temp = (uchar)(str.toInt(&ok, 16));
        key_[i] = uchar (temp);
       }

    if (this->ui->radioButton_CBC->isChecked()) {
    iv_str = ui->lineEdit_IV->text();
    QString iv_str_t = iv_str;
  #ifdef QT_DEBUG
    qDebug() <<  "iv" << iv_str;
  #endif
    str.resize(2);
    for (uchar i=0;i<16;i++){
        str = iv_str_t.left(2);
        iv_str_t.remove(0, 2 );
        uchar temp = (uchar)(str.toInt(&ok, 16));
        iv_[i] = uchar (temp);
       }
    }

    filesize1 = file_in.size();
    ba.resize(filesize1);
    buf.resize(filesize1);

 #ifdef QT_DEBUG
    qDebug() << "size: " << filesize1;
    qDebug() << fileName_in;
 #endif

    if (!file_in.open(QFile::ReadOnly)) {
        QMessageBox::warning(this, tr("Error"),
                             tr("Cannot read file %1:\n%2.")
                             .arg(QDir::toNativeSeparators(fileName_in), file_in.errorString()));
        return;
    }


    if (!fileName_in.isEmpty())
      {

        {
            QDataStream stream_in(&file_in);
            stream_in.setVersion (QDataStream::Qt_5_9) ;
            stream_in.readRawData(ba.data(), filesize1);
            //stream >> ba  ;
         #ifdef QT_DEBUG
            if(stream_in.status() != QDataStream::Ok)
            {
                qDebug() << Utf("Error reading from file");
            }
         #endif
        file_in.close();
        }

        if (filesize1 & 0x0f) {
        ba.append(0x10 - (filesize1 & 0x0f), 0); //filesize1 must be evenly divisible by 16bytes(0x10) (str_len % 16 == 0), else insert n-bytes
        filesize1 +=  0x10 - (filesize1 & 0x0f);
        }


        QFileInfo info(file_in);
        file_ext = info.completeSuffix();
        file_name = info.completeBaseName();
        int size_only_name_ptr = fileName_in.size() - file_ext.size() - 1;
        file_ext.remove(0,(file_ext.size() - 3));
        int cmp =  QString::compare(file_ext,"aes");

        if (ui->radioButton_256bit->isChecked()) AES_init(1);
           else AES_init(0);
        if (!(cmp))
         {
            if (ui->radioButton_CBC->isChecked()) test_decrypt_cbc();
               else test_decrypt_ecb();
            fileName_out =  fileName_in.left(fileName_in.size()-4) ;
         }
         else
          {
            if (ui->radioButton_CBC->isChecked()) test_encrypt_cbc();
              else test_encrypt_ecb();
            fileName_out =  fileName_in + ".aes";
          }

        QFile file_out;
        file_out.setFileName(fileName_out);

        if (file_out.exists())
        {

            QMessageBox mb("AES","File already exists. Overwrite it?",
                            QMessageBox::Question,
                            QMessageBox::Yes | QMessageBox::Default,
                            QMessageBox::No | QMessageBox::Escape,
                            QMessageBox::NoButton);
          int ret = mb.exec();
          switch (ret) {
          case QMessageBox::Yes:
               // Here we write event handling Yes

               break;
            case QMessageBox::No:
               for(uchar i=1;i<254;i++){
                fileName_out.insert(size_only_name_ptr,  "1");
                file_out.setFileName(fileName_out);
                if  (!(file_out.exists())) break;
                }
               break;
            default:
               // Here we write event handling by default
               break;
          }
        }


        file_out.setFileName(fileName_out);

        if(!file_out.open(QIODevice::WriteOnly))
        {
            QMessageBox::warning(this, tr("Error"),
                                 tr("Error open file for writing."));
        }
        else {


        QDataStream stream_out(&file_out);
        //stream_out.setVersion (QDataStream::Qt_5_9) ;
        stream_out.writeRawData(buf.data(), filesize1);  //stream_out.writeRawData((char *)buf, filesize1);
      #ifdef QT_DEBUG
        qDebug() << "FILE_OUT" << file_out.fileName();
        if(stream_out.status() != QDataStream::Ok)
        {
            qDebug() << Utf("Writing file error");
        }
      #endif
        file_out.close();
        QString msg_srt = "Saved to " + fileName_out;
        /*QDialog dialog;
        QLayout* layoutWidget = new QVBoxLayout(&dialog);
        layoutWidget ->addWidget(new QTextEdit);
        QStatusBar* statusBar = new QStatusBar;
        layoutWidget ->addWidget(statusBar );
        statusBar->showMessage(msg_srt,5000);*/
        ui->label_2->setText(msg_srt);
      #ifdef QT_DEBUG
        qDebug() << "End";
      #endif
      }
    }
}




void APP_AES::on_gen_key_iv_clicked()
{
    uint32_t key_rnd[8];
    uint32_t  iv_rnd[4];
    QRandomGenerator generator =  QRandomGenerator::securelySeeded();
    generator.fillRange(key_rnd);
    QString s;


        uint8_t N = sizeof(key_rnd) / sizeof(key_rnd[0]);
        if (!(this->ui->radioButton_256bit->isChecked())) {
            N = N/2;
            s.resize(32);
        }
        s.clear();

        for (uint8_t i = 0; i < N; ++i)
        {
            QString str = QString("%1").arg(key_rnd[i], 8, 16, QChar('0'));
            s += str;

        }
       ui->lineEdit->setText(s);

       QRandomGenerator generator1 =  QRandomGenerator::securelySeeded();
       generator1.fillRange(iv_rnd);

        N = sizeof(iv_rnd) / sizeof(iv_rnd[0]);
        s.clear();
        for (uint8_t i = 0; i < N; ++i)
        {
            QString str = QString("%1").arg(iv_rnd[i], 8, 16, QChar('0'));
            s += str;
        }
        s.resize(32);
        ui->lineEdit_IV->setText(s);

}

void APP_AES::on_commandLinkButton_clicked()
{
    QClipboard *clipboard = QApplication::clipboard();
    QString s = "key - ";
    s += ui->lineEdit->text() + "\n\r";
     if (this->ui->radioButton_CBC->isChecked()) s += "iv  - " + ui->lineEdit_IV->text() + "\n\r";
    clipboard->setText(s);
}
