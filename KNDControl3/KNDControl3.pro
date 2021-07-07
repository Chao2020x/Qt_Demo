# -------------------------------------------------
#   建议将main.cpp从项目中删除
#   将KNDControl.pro后缀改成.pri嵌入工程文件中
#   配置文件兼容，不需要改
#   输入控制参数依旧是 len 也就是 len_start~len_end 之间shu
#   重复报警的，会延长声光报警时间，最长不超过2分钟
#   超过5分钟没有操作成功的，不再操作
#
#   康奈德使用的IP必须是连续的，不应该存在空挡
#   或者说在配置文件中将继电器的个数全部设置为count_grounp=15
#   作用:防止康奈德有使用的IP，但程序读配置时没有保存
# -------------------------------------------------


QT       += core gui network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

# The following define makes your compiler emit warnings if you use
# any Qt feature that has been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    class_knd_control.cpp \
    class_knd_control_close.cpp \
    class_knd_control_open.cpp \
    main.cpp \
    kndwidget.cpp

HEADERS += \
    class_knd_control.h \
    define_knd_struct.h \
    kndwidget.h

FORMS += \
    kndwidget.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
