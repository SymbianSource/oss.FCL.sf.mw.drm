TEMPLATE = lib
TARGET = drmuinotifications
CONFIG += hb \
    plugin
INCLUDEPATH += . \
  ../../../inc
DEPENDPATH += .
DESTDIR = $${HB_BUILD_DIR}/plugins/devicedialogs
MOC_DIR = moc
OBJECTS_DIR = obj

# dependencies
HEADERS += inc/drmuidialogsbuilder.h \
    inc/drmuidialogpluginkeys.h \
    inc/drmuidialogplugin.h
SOURCES += src/drmuidialogsbuilder.cpp \
    src/drmuidialogplugin.cpp \
    DRMUINotifications_reg.rss
FORMS += 
symbian:TARGET.UID3 = 0x20026835
RESOURCES += resource/drmuinotifications.qrc
symbian: { 
    SYMBIAN_PLATFORMS = WINSCW \
        ARMV5
    TARGET.EPOCALLOWDLLDATA = 1
    TARGET.CAPABILITY = CAP_GENERAL_DLL
    TARGET.UID3 = 20026835
    hblib.sources = Hb.dll
    hblib.path = \sys\bin
    hblib.depends = "(0xEEF9EA38), 1, 0, 0, {\"Hb\"}"
    pluginstub.sources = drmuinotifications.dll
    pluginstub.path = /resource/plugins/devicedialogs
    DEPLOYMENT += pluginstub
}
!local { 
    target.path = $${HB_PLUGINS_DIR}/devicedialogs
    INSTALLS += target
}
BLD_INF_RULES.prj_exports += "$${LITERAL_HASH}include <platform_paths.hrh>" \
    "rom/drmuinotifications.iby CORE_MW_LAYER_IBY_EXPORT_PATH(drmuinotifications.iby)" \
    "qmakepluginstubs/drmuinotifications.qtplugin /epoc32/data/z/pluginstub/drmuinotifications.qtplugin"
