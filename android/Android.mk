LOCAL_PATH:= $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE := libmfx_gen_headers
LOCAL_EXPORT_C_INCLUDE_DIRS :=  \
    $(MFX_HOME)/api/vpl \
    $(MFX_HOME)/api/vpl/private \
    $(MFX_HOME)/api/mediasdk_structures \
    $(MFX_HOME)/android/include 

include $(BUILD_HEADER_LIBRARY)
