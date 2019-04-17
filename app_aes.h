#ifndef APP_AES_H
#define APP_AES_H

#include <QWidget>

namespace Ui {
class APP_AES;
}

class APP_AES : public QWidget
{
    Q_OBJECT

public:
    explicit APP_AES(QWidget *parent = 0);
    ~APP_AES();

private slots:
    void on_pushButton_clicked();

    void on_gen_key_iv_clicked();

    void on_commandLinkButton_clicked();

private:
    Ui::APP_AES *ui;
};

#endif // APP_AES_H
