#ifndef ENUM_H
#define ENUM_H

#endif // ENUM_H

namespace aedModel
{
    class ModuleSelfTest : public QObject
    {
        public:
            enum testResult_t { OK, FAIL_BATTERY, FAIL_CABLE, FAIL_OTHER };
    };
}
