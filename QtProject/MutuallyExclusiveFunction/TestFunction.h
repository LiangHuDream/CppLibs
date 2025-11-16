#ifndef TESTFUNCTION_H
#define TESTFUNCTION_H

#include <QCoreApplication>
#include <QDebug>
#include <QTimer>
#include "FunctionalMutualExclusionHandle.h"

// Simulated function class
class TestFunction : public QObject
{
    Q_OBJECT

private:
    int m_functionId;
    bool m_isOpen;
    QString m_name;

public:
    TestFunction(int functionId, const QString& name, QObject* parent = nullptr);

    bool isOpen() const { return m_isOpen; }
    int functionId() const { return m_functionId; }
    QString name() const { return m_name; }

    bool checkFunction();
    bool closeFunction();
    bool openFunction();

signals:
    void functionStateChanged(int functionId, bool isOpen);
};

// Test case class
class MutualExclusionTester : public QObject
{
    Q_OBJECT

private:
    TestFunction* m_func1;
    TestFunction* m_func2;
    TestFunction* m_func3;
    int m_testStep;

public:
    MutualExclusionTester(QObject* parent = nullptr);
    void startTest();

private slots:
    void runNextTest();

private:
    void registerFunctions();
    void setupMutualExclusions();
    void testBasicFunction();
    void testMutualExclusion();
    void testMultipleExclusions();
    void testReopenFunction();
    void testNonExclusiveFunctions();
    void testUnregisterFunction();
    void testUnregisterRelationship();
};

#endif // TESTFUNCTION_H
