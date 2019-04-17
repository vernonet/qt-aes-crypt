#include "app_aes.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    APP_AES w;
    w.show();

    return a.exec();
}
