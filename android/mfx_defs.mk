# Purpose:
#   Defines include paths, compilation flags, etc. to build Media SDK targets.
#
# Defined variables:
#   MFX_CFLAGS - common flags for all targets
#   MFX_CFLAGS_LIBVA - LibVA support flags (to build apps with or without LibVA support)
#   MFX_INCLUDES - common include paths for all targets
#   libva_headers - include paths to LibVA headers
#   MFX_LDFLAGS - common link flags for all targets

# =============================================================================
# Common definitions

MFX_CFLAGS := -DANDROID

#Media Version
MEDIA_VERSION := 24.1.5
MEDIA_VERSION_EXTRA := ""
MEDIA_VERSION_ALL := $(MEDIA_VERSION).pre$(MEDIA_VERSION_EXTRA)

MFX_CFLAGS += -DMEDIA_VERSION_STR=\"\\\"${MEDIA_VERSION}\\\"\"
MFX_CFLAGS += -DONEVPL_EXPERIMENTAL

# Android version preference:
ifneq ($(filter 14 14.% U% ,$(PLATFORM_VERSION)),)
  MFX_ANDROID_VERSION:= MFX_U
endif
ifneq ($(filter 13 13.% T% ,$(PLATFORM_VERSION)),)
  MFX_ANDROID_VERSION:= MFX_T
endif
ifneq ($(filter 12 12.% S ,$(PLATFORM_VERSION)),)
  MFX_ANDROID_VERSION:= MFX_S
endif
ifneq ($(filter 11 11.% R ,$(PLATFORM_VERSION)),)
  MFX_ANDROID_VERSION:= MFX_R
endif
ifneq ($(filter 10 10.% Q ,$(PLATFORM_VERSION)),)
  MFX_ANDROID_VERSION:= MFX_Q
endif
ifneq ($(filter 9 9.% P ,$(PLATFORM_VERSION)),)
  MFX_ANDROID_VERSION:= MFX_P
endif
ifneq ($(filter 8.% O ,$(PLATFORM_VERSION)),)
  ifneq ($(filter 8.0.%,$(PLATFORM_VERSION)),)
    MFX_ANDROID_VERSION:= MFX_O
  else
    MFX_ANDROID_VERSION:= MFX_O_MR1
  endif
endif

# Passing Android-dependency information to the code
MFX_CFLAGS += \
  -DMFX_ANDROID_VERSION=$(MFX_ANDROID_VERSION) \
  -include mfx_android_config.h

MFX_CFLAGS += \
  -DMFX_VERSION=2009

MFX_CFLAGS += \
  -DMFX_FILE_VERSION=\"`echo $(MFX_VERSION) | cut -f 1 -d.``date +.%-y.%-m.%-d`\" \
  -DMFX_PRODUCT_VERSION=\"$(MFX_VERSION)\"

#  Security
MFX_CFLAGS += \
  -fstack-protector \
  -fPIE -fPIC -pie \
  -O2 -D_FORTIFY_SOURCE=2 \
  -Wformat -Wformat-security \
  -fexceptions -frtti -msse4.1 \
  -Wno-non-virtual-dtor \
  -Wunused-command-line-argument \
  -mavx2

# Enable feature with output decoded frames without latency regarding
# SPS.VUI.max_num_reorder_frames
ifeq ($(ENABLE_MAX_NUM_REORDER_FRAMES_OUTPUT),)
    ENABLE_MAX_NUM_REORDER_FRAMES_OUTPUT:= true
endif

ifeq ($(ENABLE_MAX_NUM_REORDER_FRAMES_OUTPUT),true)
  MFX_CFLAGS += -DENABLE_MAX_NUM_REORDER_FRAMES_OUTPUT
endif

# LibVA support.
MFX_CFLAGS_LIBVA := -DLIBVA_SUPPORT -DLIBVA_ANDROID_SUPPORT

ifneq ($(filter $(MFX_ANDROID_VERSION), MFX_O),)
  MFX_CFLAGS_LIBVA += -DANDROID_O
endif

# Setting usual paths to include files
MFX_INCLUDES := $(LOCAL_PATH)/include

LOCAL_HEADER_LIBRARIES := libmfx_gen_headers libva_headers libva_cp_headers

# Setting usual link flags
MFX_LDFLAGS := \
  -z noexecstack \
  -z relro -z now

# Setting vendor
LOCAL_MODULE_OWNER := intel

# Moving executables to proprietary location
LOCAL_PROPRIETARY_MODULE := true

LOCAL_CPPFLAGS := -Wno-deprecated-declarations \
	          -Wno-missing-field-initializers \
		  -Wno-implicit-fallthrough


# =============================================================================

# Definitions specific to Media SDK internal things (do not apply for samples)
include $(MFX_HOME)/android/mfx_defs_internal.mk
