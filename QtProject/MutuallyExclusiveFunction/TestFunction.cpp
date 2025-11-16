#include "TestFunction.h"

// TestFunction implementation
TestFunction::TestFunction(int functionId, const QString& name, QObject* parent)
    : QObject(parent), m_functionId(functionId), m_isOpen(false), m_name(name)
{
}

bool TestFunction::checkFunction()
{
    qDebug() << "Checking function" << m_name << "status:" << (m_isOpen ? "Open" : "Closed");
    return m_isOpen;
}

bool TestFunction::closeFunction()
{
    if (m_isOpen) {
        m_isOpen = false;
        qDebug() << "Closing function:" << m_name;
        emit functionStateChanged(m_functionId, false);
        return true;
    }
    qDebug() << "Function" << m_name << "is already closed";
    return false;
}

bool TestFunction::openFunction()
{
    if (!m_isOpen) {
        m_isOpen = true;
        qDebug() << "Opening function:" << m_name;
        emit functionStateChanged(m_functionId, true);
        return true;
    }
    qDebug() << "Function" << m_name << "is already open";
    return false;
}

// MutualExclusionTester implementation
MutualExclusionTester::MutualExclusionTester(QObject* parent)
    : QObject(parent), m_testStep(0)
{
    // Create three test functions
    m_func1 = new TestFunction(1, "Function1-Map Display", this);
    m_func2 = new TestFunction(2, "Function2-Measurement Tool", this);
    m_func3 = new TestFunction(3, "Function3-Navigation Mode", this);

    // Register functions with the mutual exclusion manager
    registerFunctions();

    // Set up mutual exclusion relationships: Function1 and Function2 are mutually exclusive,
    // Function2 and Function3 are mutually exclusive
    setupMutualExclusions();
}

void MutualExclusionTester::startTest()
{
    qDebug() << "=== Starting Functional Mutual Exclusion Test ===";
    QTimer::singleShot(1000, this, &MutualExclusionTester::runNextTest);
}

void MutualExclusionTester::runNextTest()
{
    m_testStep++;
    qDebug() << "\n--- Test Step" << m_testStep << "---";

    switch (m_testStep) {
    case 1:
        testBasicFunction();
        break;
    case 2:
        testMutualExclusion();
        break;
    case 3:
        testMultipleExclusions();
        break;
    case 4:
        testReopenFunction();
        break;
    case 5:
        testNonExclusiveFunctions();
        break;
    case 6:
        testUnregisterFunction();
        break;
    case 7:
        testUnregisterRelationship();
        break;
    default:
        qDebug() << "=== Test Completed ===";
        QCoreApplication::quit();
        return;
    }

    QTimer::singleShot(2000, this, &MutualExclusionTester::runNextTest);
}

void MutualExclusionTester::registerFunctions()
{
    auto& handle = FunctionalMutualExclusionHandleIns();

    FunctionInfo info1;
    info1.receiver = m_func1;
    info1.functionID = 1;
    info1.checkFunction = [this]() { return m_func1->checkFunction(); };
    info1.closeFunction = [this]() { return m_func1->closeFunction(); };
    info1.openFunction = [this]() { return m_func1->openFunction(); };
    handle.RegisterFunction(info1);

    FunctionInfo info2;
    info2.receiver = m_func2;
    info2.functionID = 2;
    info2.checkFunction = [this]() { return m_func2->checkFunction(); };
    info2.closeFunction = [this]() { return m_func2->closeFunction(); };
    info2.openFunction = [this]() { return m_func2->openFunction(); };
    handle.RegisterFunction(info2);

    FunctionInfo info3;
    info3.receiver = m_func3;
    info3.functionID = 3;
    info3.checkFunction = [this]() { return m_func3->checkFunction(); };
    info3.closeFunction = [this]() { return m_func3->closeFunction(); };
    info3.openFunction = [this]() { return m_func3->openFunction(); };
    handle.RegisterFunction(info3);

    qDebug() << "Three test functions registered successfully";
}

void MutualExclusionTester::setupMutualExclusions()
{
    auto& handle = FunctionalMutualExclusionHandleIns();

    // Function1 and Function2 are mutually exclusive
    handle.RegisterMutuallyExclusiveRelationship(1, {2});
    handle.RegisterMutuallyExclusiveRelationship(2, {1});

    // Function2 and Function3 are mutually exclusive
    handle.RegisterMutuallyExclusiveRelationship(2, {3});
    handle.RegisterMutuallyExclusiveRelationship(3, {2});

    qDebug() << "Mutual exclusion relationships established: Function1<->Function2, Function2<->Function3";
}

void MutualExclusionTester::testBasicFunction()
{
    qDebug() << "Test 1: Basic function open/close operations";
    auto& handle = FunctionalMutualExclusionHandleIns();

    handle.FunctionOpen(1, true);  // Open Function1
    handle.FunctionOpen(1, false); // Close Function1
}

void MutualExclusionTester::testMutualExclusion()
{
    qDebug() << "Test 2: Mutual exclusion test - Open Function2 after Function1 is open";
    auto& handle = FunctionalMutualExclusionHandleIns();

    handle.FunctionOpen(1, true);  // Open Function1
    handle.FunctionOpen(2, true);  // Attempt to open Function2 (should automatically close Function1)
}

void MutualExclusionTester::testMultipleExclusions()
{
    qDebug() << "Test 3: Multiple exclusions test - Function2 affects both Function1 and Function3";
    auto& handle = FunctionalMutualExclusionHandleIns();

    // First open Function1 and Function3
    handle.FunctionOpen(1, true);
    handle.FunctionOpen(3, true);

    qDebug() << "Now opening Function2, which should close both Function1 and Function3";
    handle.FunctionOpen(2, true);
}

void MutualExclusionTester::testReopenFunction()
{
    qDebug() << "Test 4: Reopen previously closed function";
    auto& handle = FunctionalMutualExclusionHandleIns();

    // Current state: Function2 is open, Function1 and Function3 are closed
    qDebug() << "Attempting to reopen Function1";
    handle.FunctionOpen(1, true);  // Should close Function2 and open Function1
}

void MutualExclusionTester::testNonExclusiveFunctions()
{
    qDebug() << "Test 5: Non-exclusive functions test - Function1 and Function3 can be open simultaneously";
    auto& handle = FunctionalMutualExclusionHandleIns();

    // Function1 and Function3 have no direct mutual exclusion relationship
    handle.FunctionOpen(1, true);
    handle.FunctionOpen(3, true);  // Should be able to open simultaneously
}

void MutualExclusionTester::testUnregisterFunction()
{
    qDebug() << "Test 6: Unregister function test";
    auto& handle = FunctionalMutualExclusionHandleIns();

    FunctionInfo info;
    info.receiver = m_func1;
    info.functionID = 1;
    info.checkFunction = [this]() { return m_func1->checkFunction(); };
    info.closeFunction = [this]() { return m_func1->closeFunction(); };
    info.openFunction = [this]() { return m_func1->openFunction(); };

    handle.UnRegisterFunction(info);
    qDebug() << "Function1 unregistered, subsequent operations should have no effect";
    handle.FunctionOpen(1, true);
}

void MutualExclusionTester::testUnregisterRelationship()
{
    qDebug() << "Test 7: Unregister mutual relationship test";
    auto& handle = FunctionalMutualExclusionHandleIns();

    handle.UnregisterMutuallyExclusiveRelationship(2);
    qDebug() << "Mutual exclusion relationships for Function2 have been unregistered";

    // Now Function1 and Function2 should be able to open simultaneously
    handle.FunctionOpen(1, true);
    handle.FunctionOpen(2, true);
    handle.FunctionOpen(3, true);
}
