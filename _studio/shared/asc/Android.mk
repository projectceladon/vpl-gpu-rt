LOCAL_PATH:= $(call my-dir)

# =============================================================================

include $(CLEAR_VARS)
include $(MFX_HOME)/android/mfx_defs.mk

LOCAL_SRC_FILES := $(addprefix src/, asc_avx2_impl.cpp)

LOCAL_C_INCLUDES := \
    $(MFX_INCLUDES_INTERNAL_HW) \
    $(MFX_HOME)/_studio/mfx_lib/cmrt_cross_platform/include \
    $(MFX_HOME)/_studio/mfx_lib/ext/genx/asc/isa

LOCAL_CFLAGS := \
    $(MFX_CFLAGS_INTERNAL_HW) \
    -mavx2 \
    -Wno-error \
    -Wno-unused-parameter

LOCAL_CFLAGS += -I $(MFX_HOME)/_studio/shared/asc/include/

LOCAL_CFLAGS_32 := $(MFX_CFLAGS_INTERNAL_32)
LOCAL_CFLAGS_64 := $(MFX_CFLAGS_INTERNAL_64)

LOCAL_MODULE_TAGS := optional
LOCAL_MODULE := libmfx_asc_avx2
include $(BUILD_STATIC_LIBRARY)

# =============================================================================

include $(CLEAR_VARS)
include $(MFX_HOME)/android/mfx_defs.mk

LOCAL_SRC_FILES := $(addprefix src/, asc_sse4_impl.cpp)

LOCAL_C_INCLUDES := \
    $(MFX_INCLUDES_INTERNAL_HW) \
    $(MFX_HOME)/_studio/mfx_lib/cmrt_cross_platform/include \
    $(MFX_HOME)/_studio/mfx_lib/ext/genx/asc/isa

LOCAL_CFLAGS := \
    $(MFX_CFLAGS_INTERNAL_HW) \
    -msse4.1 \
    -Wno-error \
    -Wno-unused-parameter

LOCAL_CFLAGS_32 := $(MFX_CFLAGS_INTERNAL_32)
LOCAL_CFLAGS_64 := $(MFX_CFLAGS_INTERNAL_64)

LOCAL_MODULE_TAGS := optional
LOCAL_MODULE := libmfx_asc_sse4
include $(BUILD_STATIC_LIBRARY)

# =============================================================================

include $(CLEAR_VARS)
include $(MFX_HOME)/android/mfx_defs.mk

ASC_SRC_FILES := $(addprefix src/, \
	asc.cpp \
	asc_c_impl.cpp \
	iofunctions.cpp \
	motion_estimation_engine.cpp \
	tree.cpp)

LOCAL_SRC_FILES := $(ASC_SRC_FILES)

LOCAL_C_INCLUDES := \
    $(MFX_INCLUDES_INTERNAL_HW) \
    $(MFX_HOME)/_studio/mfx_lib/cmrt_cross_platform/include \
    $(MFX_HOME)/_studio/mfx_lib/ext/genx/asc/isa

LOCAL_STATIC_LIBRARIES := \
	libmfx_asc_avx2 \
	libmfx_asc_sse4

LOCAL_CFLAGS := \
    $(MFX_CFLAGS_INTERNAL_HW) \
    -msse4.1 \
    -Wno-error \
    -Wno-unused-parameter

LOCAL_CFLAGS_32 := $(MFX_CFLAGS_INTERNAL_32)
LOCAL_CFLAGS_64 := $(MFX_CFLAGS_INTERNAL_64)

LOCAL_WHOLE_STATIC_LIBRARIES := $(LOCAL_STATIC_LIBRARIES)

LOCAL_MODULE_TAGS := optional
LOCAL_MODULE := libmfx_asc

include $(BUILD_STATIC_LIBRARY)
