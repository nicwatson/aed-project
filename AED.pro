QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets  printsupport

CONFIG += c++11

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    aedGui/CompressionsToggleButton.cpp \
    aedGui/LCDDisplay.cpp \
    aedGui/LampButton.cpp \
    aedGui/LampWidget.cpp \
    aedModel/AED.cpp \
    aedModel/AED_setup.cpp \
    aedModel/AED_slots.cpp \
    aedModel/CPRHelpAdult.cpp \
    aedModel/CPRHelpChild.cpp \
    aedModel/CPRHelpStrategy.cpp \
    aedModel/ModuleCPRHelp.cpp \
    aedModel/ModuleECGAssessment.cpp \
    aedModel/ModuleSelfTest.cpp \
    aedModel/ModuleShock.cpp \
    aedModel/ModuleStartupAdvice.cpp \
    event/AdviceEvent.cpp \
    event/EventSequence.cpp \
    event/PictogramAdviceEvent.cpp \
    event/SequencedEvent.cpp \
    main.cpp \
    mainwindow.cpp \
    qcustomplot.cpp

HEADERS += \
    aedGui/CompressionsToggleButton.h \
    aedGui/LCDDisplay.h \
    aedGui/LampButton.h \
    aedGui/LampWidget.h \
    aedGui/strings.h \
    aedGui/styles.h \
    aedModel/AED.h \
    aedModel/CPRHelpAdult.h \
    aedModel/CPRHelpChild.h \
    aedModel/CPRHelpStrategy.h \
    aedModel/ModuleCPRHelp.h \
    aedModel/ModuleECGAssessment.h \
    aedModel/ModuleSelfTest.h \
    aedModel/ModuleShock.h \
    aedModel/ModuleStartupAdvice.h \
    aedconst.h \
    event/AdviceEvent.h \
    event/EventSequence.h \
    event/PictogramAdviceEvent.h \
    event/SequencedEvent.h \
    mainwindow.h \
    qcustomplot.h

FORMS += \
    mainwindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    assets/aed.qrc
