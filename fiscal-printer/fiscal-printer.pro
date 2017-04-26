CONFIG -= qt

TARGET = fiscal-printer

TEMPLATE = lib

INCLUDEPATH += ..

DEFINES += DEBUG_FISCAL_PRINTER

SOURCES += FiscalPrinter.cpp

HEADERS += Common.hpp FiscalPrinter.hpp
