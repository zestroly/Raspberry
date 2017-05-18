#include <QCoreApplication>

    QCoreApplication a(argc, argv);
    QSettings *ConfigInt = new QSettings("./camera.ini", QSettings::IniFormat);
    ConfigInt->setValue("size", 100);
    ConfigInt->setValue("pos",20);
    delete ConfigInt;

