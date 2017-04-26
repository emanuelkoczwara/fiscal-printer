TARGET = fiscal-printer-tester

TEMPLATE = app

INCLUDEPATH += . ..

SOURCES += main.cpp MainWindow.cpp

HEADERS += MainWindow.hpp

FORMS += MainWindow.ui

LIBS += -L../fiscal-printer -lfiscal-printer -lboost_regex -lboost_system
