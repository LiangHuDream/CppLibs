#include <QCoreApplication>
#include <QDebug>
#include <QTimer>

#include "TestFunction.h"


int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    MutualExclusionTester tester;
    QTimer::singleShot(500, &tester, &MutualExclusionTester::startTest);

    return a.exec();
}

