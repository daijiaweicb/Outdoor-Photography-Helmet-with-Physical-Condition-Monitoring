#include <QtTest>
#include <QMetaType>
#include "mock_motor_thread.h"
#include "Mode.h"
Q_DECLARE_METATYPE(SystemMode)
/**
 * @class TestableMotorThread
 * @brief A subclass of MotorThread used for unit testing with a mock StepperMotor.
 *
 * This class allows injection of a mocked StepperMotor instance to replace
 * the real hardware interactions. It is used to verify the behavior of the
 * MotorThread::run() logic without requiring actual GPIO hardware.
 */
class MotorThreadTest : public QObject
{
    Q_OBJECT

private slots:
    void initTestCase();
    void testNormalToFatigue();
    void testFatigueToNormal();
};

void MotorThreadTest::initTestCase() {
    qRegisterMetaType<SystemMode>("SystemMode");
}

void MotorThreadTest::testNormalToFatigue()
{
    g_systemMode = SystemMode::Normal;
    TestableMotorThread thread;

    QSignalSpy spy(&thread, SIGNAL(modeChanged(SystemMode)));

    thread.run();

    auto *motor = thread.getMockMotor();
    QVERIFY(motor->start_called);
    QVERIFY(motor->forward_called);
    QVERIFY(motor->cleanup_called);

    QCOMPARE(g_systemMode, SystemMode::FatigueDetection);
    QCOMPARE(spy.count(), 3);
}

void MotorThreadTest::testFatigueToNormal()
{
    g_systemMode = SystemMode::FatigueDetection;
    TestableMotorThread thread;

    QSignalSpy spy(&thread, SIGNAL(modeChanged(SystemMode)));

    thread.run();

    auto *motor = thread.getMockMotor();
    QVERIFY(motor->start_called);
    QVERIFY(motor->backward_called);
    QVERIFY(motor->cleanup_called);

    QCOMPARE(g_systemMode, SystemMode::Normal);
    QCOMPARE(spy.count(), 3);
}

QTEST_MAIN(MotorThreadTest)
#include "test_motorthreadqt.moc"
