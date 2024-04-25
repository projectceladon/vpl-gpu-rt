include $(CLEAR_VARS)
include $(MFX_HOME)/android/mfx_defs.mk

LOCAL_SRC_FILES := $(addprefix src/, aenc.cpp \
					hevc_asc_apq_tree.cpp \
					av1_asc_agop_tree.cpp \
					av1_asc_tree.cpp \
					hevc_asc_agop_tree.cpp \
					av1_asc.cpp)

LOCAL_C_INCLUDES := \
    $(MFX_INCLUDES_INTERNAL_HW) \
    $(MFX_HOME)/_studio/enctools/aenc/include/

LOCAL_CFLAGS := \
    $(MFX_CFLAGS_INTERNAL_HW) \
    -mavx2 \
    -Wno-error \
    -Wno-unused-parameter

LOCAL_CFLAGS_32 := $(MFX_CFLAGS_INTERNAL_32)
LOCAL_CFLAGS_64 := $(MFX_CFLAGS_INTERNAL_64)

LOCAL_MODULE_TAGS := optional
LOCAL_MODULE := libmfx_aenc
include $(BUILD_STATIC_LIBRARY)
