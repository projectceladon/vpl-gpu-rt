# Purpose:
#   Defines include paths, compilation flags, etc. to build Media SDK
# internal targets (libraries, test applications, etc.).
#
# Defined variables:
#   MFX_CFLAGS_INTERNAL - all flags needed to build MFX targets
#   MFX_CFLAGS_INTERNAL_HW - all flags needed to build MFX HW targets
#   MFX_INCLUDES_INTERNAL - all include paths needed to build MFX targets
#   MFX_INCLUDES_INTERNAL_HW - all include paths needed to build MFX HW targets

MFX_CFLAGS_INTERNAL := $(MFX_CFLAGS)
MFX_CFLAGS_INTERNAL_HW := \
  $(MFX_CFLAGS_INTERNAL) \
  -DMFX_VA \
  -DMFX_ONEVPL \
  -DMFX_VERSION_USE_LATEST \
  -DMFX_DEPRECATED_OFF \
  -DONEVPL_EXPERIMENTAL \
  -DSYNCHRONIZATION_BY_VA_SYNC_SURFACE \
  -D_FILE_OFFSET_BITS=64 \
  -D__USE_LARGEFILE64 \
  -DMFX_GIT_COMMIT=\"c6573984\" \
  -DMFX_API_VERSION=\"2.9+\" \
  -DNDEBUG \
  -DMSDK_BUILD=\"\"

MFX_CFLAGS_INTERNAL_32 := -DLINUX32
MFX_CFLAGS_INTERNAL_64 := -DLINUX32 -DLINUX64

MFX_INCLUDES_INTERNAL :=  \
    $(MFX_INCLUDES) \
    $(MFX_HOME)/_studio/shared/include \
    $(MFX_HOME)/_studio/shared/umc/codec/av1_dec/include \
    $(MFX_HOME)/_studio/shared/umc/codec/h265_dec/include \
    $(MFX_HOME)/_studio/shared/umc/codec/h264_dec/include \
    $(MFX_HOME)/_studio/shared/umc/codec/vp9_dec/include \
    $(MFX_HOME)/_studio/shared/umc/core/umc/include \
    $(MFX_HOME)/_studio/shared/umc/core/vm/include \
    $(MFX_HOME)/_studio/shared/umc/core/vm_plus/include \
    $(MFX_HOME)/_studio/shared/umc/io/umc_va/include \
    $(MFX_HOME)/_studio/shared/mfx_logging/include \
    $(MFX_HOME)/_studio/shared/mfx_trace/include \
    $(MFX_HOME)/_studio/shared/mfx_trace/include/sys \
    $(MFX_HOME)/_studio/mfx_lib/shared/include \
    $(MFX_HOME)/_studio/shared/include \
    $(MFX_HOME)/_studio/enctools/aenc/include \
    $(MFX_HOME)/_studio/enctools/include \
    $(MFX_HOME)/contrib/ipp/include

MFX_INCLUDES_INTERNAL_HW := \
    $(MFX_INCLUDES_INTERNAL) \
    -I system/logging/liblog/include
