#ifndef MODULESELFTEST_H
#define MODULESELFTEST_H

#include <QObject>

namespace aed
{
    class ModuleSelfTest : public QObject
    {
        Q_OBJECT
    public:
        enum testResult_t { OK, FAIL_BATTERY, FAIL_CABLE, FAIL_OTHER };

        explicit ModuleSelfTest(QObject *parent = nullptr);

    signals:

    };
}

#endif // MODULESELFTEST_H
