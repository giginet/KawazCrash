LOCAL_PATH := $(call my-dir)
ADX2_PATH := $(LOCAL_PATH)/../../adx2

include $(CLEAR_VARS)

LOCAL_MODULE := cocos2dcpp_shared

LOCAL_MODULE_FILENAME := libcocos2dcpp

LOCAL_SRC_FILES := hellocpp/main.cpp \
                   ../../Classes/AppDelegate.cpp \
                   ../../Classes/LicenseScene.cpp \
                   ../../Classes/TitleScene.cpp \
                   ../../Classes/Cookie.cpp \
                   ../../Classes/MainScene.cpp \
                   ../../Classes/ADX2Manager/ADX2Manager/Manager.cpp \
                   ../../Classes/ADX2Manager/ADX2Manager/CueSheet.cpp \
                   ../../Classes/external/SpriteStudio/Players/Cocos2d-x_v3/SS5Player.cpp \

LOCAL_C_INCLUDES := $(LOCAL_PATH)/../../Classes \
                    $(ADX2_PATH)/cocos2dx/include/android \
                    $(LOCAL_PATH)/../../Classes/ADX2Manager/ADX2Manager \
                    $(LOCAL_PATH)/../../Classes/external/SpriteStudio/Players/Cocos2d-x_v3

LOCAL_WHOLE_STATIC_LIBRARIES := cocos2dx_static
LOCAL_WHOLE_STATIC_LIBRARIES += cocosdenshion_static

# LOCAL_WHOLE_STATIC_LIBRARIES += box2d_static
# LOCAL_WHOLE_STATIC_LIBRARIES += cocosbuilder_static
# LOCAL_WHOLE_STATIC_LIBRARIES += spine_static
LOCAL_WHOLE_STATIC_LIBRARIES += cocostudio_static
# LOCAL_WHOLE_STATIC_LIBRARIES += cocos_network_static
# LOCAL_WHOLE_STATIC_LIBRARIES += cocos_extension_static

LOCAL_LDLIBS += \
        -L$(call host-path, $(ADX2_PATH)/cocos2dx/libs/android/armeabi-v7a) \
        -lcri_atom_android \
        -lcri_file_system_android \
        -lcri_base_android \
        -llog \
        -lOpenSLES

include $(BUILD_SHARED_LIBRARY)

$(call import-module,.)
$(call import-module,audio/android)

# $(call import-module,Box2D)
# $(call import-module,editor-support/cocosbuilder)
# $(call import-module,editor-support/spine)
$(call import-module,editor-support/cocostudio)
# $(call import-module,network)
# $(call import-module,extensions)
