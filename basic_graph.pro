QT += widgets

TARGET = basic_graph

QMAKE_CXXFLAGS += -mfpmath=sse -fstack-protector-all -g -W -Wall -Wextra -Wunused -Wcast-align -Werror -pedantic -pedantic-errors -Wfloat-equal -Wpointer-arith -Wformat-security -Wmissing-format-attribute -Wformat=1 -Wwrite-strings -Wno-long-long -Woverloaded-virtual -Wnon-virtual-dtor -Wcast-qual -Wno-suggest-attribute=format -O3

HEADERS = window.h \
          functions.h \
          sliding_polynomial.h \
          spline.h

SOURCES = main.cpp \
          window_utils.cpp \
          paintEvent.cpp \
          keyPressEvent.cpp \
          methods_draw.cpp \
          erros.cpp \
          functions.cpp \
          sliding_polynomial.cpp \
          spline.cpp