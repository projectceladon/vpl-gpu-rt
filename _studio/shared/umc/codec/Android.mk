LOCAL_PATH:= $(call my-dir)

# Setting subdirectories to march thru
MFX_CODEC_LOCAL_DIRS := \
    h264_enc \
    vc1_common \
    jpeg_common \
    color_space_converter \
    mpeg2_dec \
    h265_dec \
    h264_dec \
    vc1_dec \
    jpeg_dec \
    vp9_dec \
    av1_dec

MFX_CODEC_LOCAL_SRC_FILES := \
  $(patsubst $(LOCAL_PATH)/%, %, $(foreach dir, $(MFX_CODEC_LOCAL_DIRS), $(wildcard $(LOCAL_PATH)/$(dir)/src/*.cpp)))

# =============================================================================

include $(CLEAR_VARS)
include $(MFX_HOME)/android/mfx_defs.mk

LOCAL_SRC_FILES := \
    brc/src/umc_brc.cpp \
    brc/src/umc_h264_brc.cpp \
    brc/src/umc_mpeg2_brc.cpp \
    brc/src/umc_video_brc.cpp

LOCAL_C_INCLUDES := \
    $(LOCAL_PATH)/brc/include \
    $(MFX_INCLUDES_INTERNAL_HW)

LOCAL_CFLAGS := \
    $(MFX_CFLAGS_INTERNAL_HW) \
    -Wno-error \
    -Wno-unused-parameter \
    -Wno-deprecated-declarations

LOCAL_CFLAGS_32 := $(MFX_CFLAGS_INTERNAL_32)
LOCAL_CFLAGS_64 := $(MFX_CFLAGS_INTERNAL_64)

LOCAL_HEADER_LIBRARIES := libmfx_gen_headers
LOCAL_SHARED_LIBRARIES := liblog libcutils

LOCAL_MODULE_TAGS := optional
LOCAL_MODULE := libumc_brc

include $(BUILD_STATIC_LIBRARY)

# =============================================================================

include $(CLEAR_VARS)
include $(MFX_HOME)/android/mfx_defs.mk

LOCAL_SRC_FILES := $(MFX_CODEC_LOCAL_SRC_FILES)

LOCAL_C_INCLUDES := \
    $(LOCAL_PATH)/av1_dec/include \
    $(LOCAL_PATH)/color_space_converter/include \
    $(LOCAL_PATH)/h264_dec/include \
    $(LOCAL_PATH)/h265_dec/include \
    $(LOCAL_PATH)/jpeg_common/include \
    $(LOCAL_PATH)/jpeg_dec/include \
    $(LOCAL_PATH)/mpeg2_dec/include \
    $(LOCAL_PATH)/vc1_common/include \
    $(LOCAL_PATH)/vc1_dec/include \
    $(LOCAL_PATH)/vp9_dec/include \
    $(MFX_INCLUDES_INTERNAL_HW)

LOCAL_CFLAGS := \
    $(MFX_CFLAGS_INTERNAL_HW) \
    -Wno-error \
    -Wno-unused-parameter

LOCAL_CFLAGS_32 := $(MFX_CFLAGS_INTERNAL_32)
LOCAL_CFLAGS_64 := $(MFX_CFLAGS_INTERNAL_64)

LOCAL_HEADER_LIBRARIES := libmfx_gen_headers

LOCAL_SHARED_LIBRARIES := liblog libcutils

LOCAL_MODULE_TAGS := optional
LOCAL_MODULE := libumc_codecs_hw

include $(BUILD_STATIC_LIBRARY)
