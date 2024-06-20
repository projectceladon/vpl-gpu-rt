
LOCAL_PATH:= $(MFX_HOME)/_studio

# =============================================================================

MFX_LOCAL_DECODERS := h265 h264 vc1 mjpeg vp8 vp9 av1
MFX_LOCAL_ENCODERS := h264 mpeg2 mjpeg vp9

# Setting subdirectories to march thru
MFX_LOCAL_DIRS := \
    scheduler/linux \

MFX_LOCAL_DIRS_IMPL := \
    $(addprefix decode/, $(MFX_LOCAL_DECODERS)) \
    vpp

MFX_LOCAL_DIRS_HW := \
    $(addprefix encode_hw/, $(MFX_LOCAL_ENCODERS)) \
    mctf_package/mctf \
    cmrt_cross_platform

#scheduler/linux/src
MFX_LOCAL_SRC_FILES := \
    $(patsubst $(LOCAL_PATH)/%, %, $(foreach dir, $(MFX_LOCAL_DIRS), $(wildcard $(LOCAL_PATH)/mfx_lib/$(dir)/src/*.cpp)))

#decode/h265 h264 vc1 mjpeg vp8 vp9 av1/src
MFX_LOCAL_SRC_FILES_IMPL := \
    $(patsubst $(LOCAL_PATH)/%, %, $(foreach dir, $(MFX_LOCAL_DIRS_IMPL), $(wildcard $(LOCAL_PATH)/mfx_lib/$(dir)/src/*.cpp)))

#encode_hw/h264 mpeg2 mjpeg vp9/src
MFX_LOCAL_SRC_FILES_HW := \
    $(MFX_LOCAL_SRC_FILES_IMPL) \
    $(patsubst $(LOCAL_PATH)/%, %, $(foreach dir, $(MFX_LOCAL_DIRS_HW), $(wildcard $(LOCAL_PATH)/mfx_lib/$(dir)/src/*.cpp)))

#mpeg2 hw decoder
MFX_LOCAL_SRC_FILES_HW += $(addprefix mfx_lib/decode/mpeg2/hw/src/, \
    mfx_mpeg2_decode.cpp)

MFX_LOCAL_SRC_FILES_HW += $(addprefix mfx_lib/ext/genx/h264_encode/isa/, \
    genx_simple_me_gen12lp_isa.cpp \
    genx_histogram_gen12lp_isa.cpp)

MFX_LOCAL_SRC_FILES_HW += \
    mfx_lib/encode_hw/av1/av1ehw_disp.cpp \
    mfx_lib/encode_hw/av1/agnostic/av1ehw_base.cpp \
    mfx_lib/encode_hw/av1/linux/base/av1ehw_base_lin.cpp \
    mfx_lib/encode_hw/av1/linux/base/av1ehw_base_va_lin.cpp \
    mfx_lib/encode_hw/av1/linux/base/av1ehw_base_va_packer_lin.cpp \
    mfx_lib/encode_hw/av1/linux/base/av1ehw_base_max_frame_size_lin.cpp \
    mfx_lib/encode_hw/av1/agnostic/Xe_HPM/av1ehw_xe_hpm_segmentation.cpp \
    mfx_lib/encode_hw/av1/linux/Xe_HPM/av1ehw_xe_hpm_lin.cpp \
    mfx_lib/encode_hw/av1/linux/Xe_LPM_plus/av1ehw_xe_lpm_plus_lin.cpp \
    mfx_lib/encode_hw/av1/agnostic/base/av1ehw_base_alloc.cpp \
    mfx_lib/encode_hw/av1/agnostic/base/av1ehw_base_constraints.cpp \
    mfx_lib/encode_hw/av1/agnostic/base/av1ehw_base_qmatrix.cpp \
    mfx_lib/encode_hw/av1/agnostic/base/av1ehw_base_enctools_com.cpp \
    mfx_lib/encode_hw/av1/agnostic/base/av1ehw_base_general.cpp \
    mfx_lib/encode_hw/av1/agnostic/base/av1ehw_base_general_defaults.cpp \
    mfx_lib/encode_hw/av1/agnostic/base/av1ehw_base_impl.cpp \
    mfx_lib/encode_hw/av1/agnostic/base/av1ehw_base_packer.cpp \
    mfx_lib/encode_hw/av1/agnostic/base/av1ehw_base_query_impl_desc.cpp \
    mfx_lib/encode_hw/av1/agnostic/base/av1ehw_base_segmentation.cpp \
    mfx_lib/encode_hw/av1/agnostic/base/av1ehw_base_task.cpp \
    mfx_lib/encode_hw/av1/agnostic/base/av1ehw_base_tile.cpp \
    mfx_lib/encode_hw/av1/agnostic/base/av1ehw_base_encoded_frame_info.cpp \
    mfx_lib/encode_hw/av1/agnostic/base/av1ehw_base_max_frame_size.cpp \
    mfx_lib/encode_hw/av1/agnostic/base/av1ehw_base_enctools.cpp \
    mfx_lib/encode_hw/av1/agnostic/base/av1ehw_base_hdr.cpp \

MFX_LOCAL_SRC_FILES_HW += \
    mfx_lib/encode_hw/hevc/hevcehw_disp.cpp \
    mfx_lib/encode_hw/hevc/agnostic/hevcehw_base.cpp \
    mfx_lib/encode_hw/hevc/agnostic/base/hevcehw_base_impl.cpp \
    mfx_lib/encode_hw/hevc/agnostic/base/hevcehw_base_alloc.cpp \
    mfx_lib/encode_hw/hevc/agnostic/base/hevcehw_base_enctools_com.cpp \
    mfx_lib/encode_hw/hevc/agnostic/base/hevcehw_base_constraints.cpp \
    mfx_lib/encode_hw/hevc/agnostic/base/hevcehw_base_dirty_rect.cpp \
    mfx_lib/encode_hw/hevc/agnostic/base/hevcehw_base_encoded_frame_info.cpp \
    mfx_lib/encode_hw/hevc/agnostic/base/hevcehw_base_encode_stats.cpp \
    mfx_lib/encode_hw/hevc/agnostic/base/hevcehw_base_enctools.cpp \
    mfx_lib/encode_hw/hevc/agnostic/base/hevcehw_base_ext_brc.cpp \
    mfx_lib/encode_hw/hevc/agnostic/base/hevcehw_base_hdr_sei.cpp \
    mfx_lib/encode_hw/hevc/agnostic/base/hevcehw_base_hrd.cpp \
    mfx_lib/encode_hw/hevc/agnostic/base/hevcehw_base_scc.cpp \
    mfx_lib/encode_hw/hevc/agnostic/base/hevcehw_base_interlace.cpp \
    mfx_lib/encode_hw/hevc/agnostic/base/hevcehw_base_legacy.cpp \
    mfx_lib/encode_hw/hevc/agnostic/base/hevcehw_base_legacy_defaults.cpp \
    mfx_lib/encode_hw/hevc/agnostic/base/hevcehw_base_max_frame_size.cpp \
    mfx_lib/encode_hw/hevc/agnostic/base/hevcehw_base_packer.cpp \
    mfx_lib/encode_hw/hevc/agnostic/base/hevcehw_base_parser.cpp \
    mfx_lib/encode_hw/hevc/agnostic/base/hevcehw_base_recon_info.cpp \
    mfx_lib/encode_hw/hevc/agnostic/base/hevcehw_base_rext.cpp \
    mfx_lib/encode_hw/hevc/agnostic/base/hevcehw_base_roi.cpp \
    mfx_lib/encode_hw/hevc/agnostic/base/hevcehw_base_task.cpp \
    mfx_lib/encode_hw/hevc/agnostic/base/hevcehw_base_query_impl_desc.cpp \
    mfx_lib/encode_hw/hevc/agnostic/base/hevcehw_base_weighted_prediction.cpp \
    mfx_lib/encode_hw/hevc/agnostic/g12/hevcehw_g12_caps.cpp \
    mfx_lib/encode_hw/hevc/linux/base/hevcehw_base_interlace_lin.cpp \
    mfx_lib/encode_hw/hevc/linux/base/hevcehw_base_lin.cpp \
    mfx_lib/encode_hw/hevc/linux/base/hevcehw_base_dirty_rect_lin.cpp \
    mfx_lib/encode_hw/hevc/linux/base/hevcehw_base_max_frame_size_lin.cpp \
    mfx_lib/encode_hw/hevc/linux/base/hevcehw_base_enctools_qmatrix_lin.cpp \
    mfx_lib/encode_hw/hevc/linux/base/hevcehw_base_roi_lin.cpp \
    mfx_lib/encode_hw/hevc/linux/base/hevcehw_base_va_lin.cpp \
    mfx_lib/encode_hw/hevc/linux/base/hevcehw_base_rext_lin.cpp \
    mfx_lib/encode_hw/hevc/linux/base/hevcehw_base_va_packer_lin.cpp \
    mfx_lib/encode_hw/hevc/linux/base/hevcehw_base_qp_modulation_lin.cpp \
    mfx_lib/encode_hw/hevc/linux/base/hevcehw_base_weighted_prediction_lin.cpp \
    mfx_lib/encode_hw/hevc/linux/g12/hevcehw_g12_lin.cpp \
    mfx_lib/encode_hw/hevc/linux/xe_hpm/hevcehw_xe_hpm_lin.cpp \
    mfx_lib/encode_hw/hevc/linux/xe_lpm_plus/hevcehw_xe_lpm_plus_lin.cpp \
    mfx_lib/encode_hw/hevc/agnostic/base/hevcehw_base_extddi.cpp \
    mfx_lib/encode_hw/hevc/agnostic/base/hevcehw_base_caps.cpp \
    mfx_lib/encode_hw/shared/ehw_resources_pool.cpp \
    mfx_lib/encode_hw/shared/ehw_task_manager.cpp \
    mfx_lib/encode_hw/shared/ehw_device_vaapi.cpp \
    mfx_lib/encode_hw/shared/ehw_utils_vaapi.cpp \
    mfx_lib/ext/cmrt_cross_platform/src/cm_mem_copy.cpp \
    mfx_lib/ext/cmrt_cross_platform/src/cmrt_cross_platform.cpp \
    mfx_lib/ext/cmrt_cross_platform/src/cmrt_utility.cpp \
    mfx_lib/ext/asc/src/asc_cm.cpp \
    mfx_lib/ext/genx/h264_encode/src/genx_simple_me_proto.cpp \

#scheduler/linux/include
MFX_LOCAL_INCLUDES := \
    $(foreach dir, $(MFX_LOCAL_DIRS), $(wildcard $(LOCAL_PATH)/mfx_lib/$(dir)/include))

#decode/h265 h264 vc1 mjpeg vp8 vp9 av1/include
MFX_LOCAL_INCLUDES_IMPL := \
    $(MFX_LOCAL_INCLUDES) \
    $(foreach dir, $(MFX_LOCAL_DIRS_IMPL), $(wildcard $(LOCAL_PATH)/mfx_lib/$(dir)/include))

#decode/mpeg2/hw/include
MFX_LOCAL_INCLUDES_IMPL += \
    $(MFX_HOME)/_studio/mfx_lib/decode/mpeg2/hw/include

#encode_hw/h264 mpeg2 mjpeg vp9/include
MFX_LOCAL_INCLUDES_IMPL += \
    $(foreach dir, $(MFX_LOCAL_DIRS_HW), $(wildcard $(LOCAL_PATH)/mfx_lib/$(dir)/include))

MFX_LOCAL_INCLUDES_HW := \
    $(MFX_LOCAL_INCLUDES_IMPL) \
    $(MFX_HOME)/_studio/mfx_lib/ext/asc/include \
    $(MFX_HOME)/_studio/mfx_lib/ext/genx/h264_encode/isa \
    $(MFX_HOME)/_studio/mfx_lib/ext/genx/field_copy/isa \
    $(MFX_HOME)/_studio/mfx_lib/ext/genx/copy_kernels/isa \
    $(MFX_HOME)/_studio/mfx_lib/ext/cmrt_cross_platform/include \
    $(MFX_HOME)/_studio/mfx_lib/ext/genx/mctf/isa \
    $(MFX_HOME)/_studio/mfx_lib/ext/genx/asc/isa \
    $(MFX_HOME)/_studio/mfx_lib/ext/h264/include \
    $(MFX_HOME)/_studio/mfx_lib/ext/mpeg2/include \
    $(MFX_HOME)/_studio/mfx_lib/encode_hw/av1 \
    $(MFX_HOME)/_studio/mfx_lib/encode_hw/av1/agnostic \
    $(MFX_HOME)/_studio/mfx_lib/encode_hw/av1/agnostic/base \
    $(MFX_HOME)/_studio/mfx_lib/encode_hw/av1/agnostic/Xe_HPM \
    $(MFX_HOME)/_studio/mfx_lib/encode_hw/av1/agnostic/Xe_LPM_plus \
    $(MFX_HOME)/_studio/mfx_lib/encode_hw/av1/linux \
    $(MFX_HOME)/_studio/mfx_lib/encode_hw/av1/linux/base \
    $(MFX_HOME)/_studio/mfx_lib/encode_hw/av1/linux/Xe_HPM \
    $(MFX_HOME)/_studio/mfx_lib/encode_hw/av1/linux/Xe_LPM_plus \
    $(MFX_HOME)/_studio/mfx_lib/encode_hw/hevc \
    $(MFX_HOME)/_studio/mfx_lib/encode_hw/hevc/agnostic \
    $(MFX_HOME)/_studio/mfx_lib/encode_hw/hevc/agnostic/base \
    $(MFX_HOME)/_studio/mfx_lib/encode_hw/hevc/agnostic/g12 \
    $(MFX_HOME)/_studio/mfx_lib/encode_hw/hevc/linux \
    $(MFX_HOME)/_studio/mfx_lib/encode_hw/hevc/linux/base \
    $(MFX_HOME)/_studio/mfx_lib/encode_hw/hevc/linux/g12 \
    $(MFX_HOME)/_studio/mfx_lib/encode_hw/hevc/linux/xe_hpm \
    $(MFX_HOME)/_studio/mfx_lib/encode_hw/hevc/linux/xe_lpm_plus \
    $(MFX_HOME)/_studio/mfx_lib/encode_hw/shared \
    $(MFX_HOME)/_studio/mfx_lib/shared/include/feature_blocks \
    $(MFX_HOME)/_studio/mfx_lib/scheduler/linux/include \
    $(MFX_HOME)/_studio/shared/asc/include

MFX_LOCAL_STATIC_LIBRARIES_HW := \
    libmfx_core_hw \
    libumc_codecs_hw \
    libumc_brc \
    libumc_va \
    libumc_core_hw \
    libmfx_gen_trace \
    libmfx_asc \
    libmfx_logging

MFX_LOCAL_LDFLAGS_HW := \
    $(MFX_LDFLAGS) \
    -Wl,--version-script=$(LOCAL_PATH)/mfx_lib/libmfx-gen.map

# =============================================================================

UMC_DIRS := \
    h264_enc \
    brc

UMC_DIRS_IMPL := \
    h265_dec h264_dec mpeg2_dec vc1_dec jpeg_dec vp9_dec av1_dec \
    vc1_common jpeg_common color_space_converter

UMC_LOCAL_INCLUDES := \
    $(foreach dir, $(UMC_DIRS), $(wildcard $(MFX_HOME)/_studio/shared/umc/codec/$(dir)/include))

UMC_LOCAL_INCLUDES_IMPL := \
    $(UMC_LOCAL_INCLUDES) \
    $(foreach dir, $(UMC_DIRS_IMPL), $(wildcard $(MFX_HOME)/_studio/shared/umc/codec/$(dir)/include))

UMC_LOCAL_INCLUDES_HW := \
    $(UMC_LOCAL_INCLUDES_IMPL)

# =============================================================================

MFX_SHARED_FILES_IMPL := $(addprefix mfx_lib/shared/src/, \
    mfx_feature_blocks_base.cpp \
    mfx_brc_common.cpp \
    mfx_common_decode_int.cpp \
    mfx_common_int.cpp \
    mfx_enc_common.cpp \
    mfx_log.cpp \
    mfx_enc_enctools_common.cpp \
    mfx_mpeg2_dec_common.cpp \
    mfx_vc1_dec_common.cpp \
    mfx_ddi_enc_dump.cpp \
    mfx_vpx_dec_common.cpp)

MFX_SHARED_FILES_HW := \
    $(MFX_SHARED_FILES_IMPL)

MFX_SHARED_FILES_HW += $(addprefix mfx_lib/ext/genx/asc/isa/, \
    genx_scd_gen12lp_isa.cpp)

MFX_SHARED_FILES_HW += $(addprefix mfx_lib/ext/genx/copy_kernels/isa/, \
    genx_copy_kernel_gen12lp_isa.cpp)

MFX_SHARED_FILES_HW += $(addprefix mfx_lib/ext/genx/field_copy/isa/, \
    genx_fcopy_gen12lp_isa.cpp)

MFX_SHARED_FILES_HW += $(addprefix mfx_lib/ext/genx/mctf/isa/, \
    genx_me_gen12lp_isa.cpp \
    genx_mc_gen12lp_isa.cpp \
    genx_sd_gen12lp_isa.cpp)

MFX_SHARED_FILES_HW += $(addprefix mfx_lib/ext/mpeg2/src/, \
    mfx_mpeg2_encode_debug_hw.cpp \
    mfx_mpeg2_encode_full_hw.cpp \
    mfx_mpeg2_encode_hw.cpp \
    mfx_mpeg2_encode_utils_hw.cpp \
    mfx_mpeg2_encode_vaapi.cpp \
    mfx_mpeg2_encode_factory.cpp \
    mfx_mpeg2_enc_common_hw.cpp)

MFX_SHARED_FILES_HW += $(addprefix mfx_lib/ext/h264/src/, \
    mfx_h264_encode_cm.cpp)

MFX_LIB_SHARED_FILES_1 := $(addprefix mfx_lib/shared/src/, \
    libmfxsw.cpp \
    libmfxsw_async.cpp \
    libmfxsw_decode.cpp \
    libmfxsw_decode_vp.cpp \
    libmfxsw_functions.cpp \
    libmfxsw_enc.cpp \
    libmfxsw_encode.cpp \
    libmfxsw_pak.cpp \
    libmfxsw_plugin.cpp \
    libmfxsw_query.cpp \
    libmfxsw_session.cpp \
    libmfxsw_vpp.cpp \
    mfx_session.cpp \
    mfx_critical_error_handler.cpp)

MFX_LIB_SHARED_FILES_2 := $(addprefix shared/src/, \
    fast_copy.cpp \
    fast_copy_c_impl.cpp \
    fast_copy_sse4_impl.cpp \
    mfx_vpp_vaapi.cpp \
    mfx_vpp_helper.cpp \
    libmfx_allocator.cpp \
    libmfx_allocator_vaapi.cpp \
    libmfx_core.cpp \
    libmfx_core_hw.cpp \
    libmfx_core_factory.cpp \
    libmfx_core_vaapi.cpp \
    mfx_umc_alloc_wrapper.cpp \
    mfx_umc_mjpeg_vpp.cpp)

# =============================================================================

include $(CLEAR_VARS)
include $(MFX_HOME)/android/mfx_defs.mk

LOCAL_SRC_FILES := \
    $(MFX_LOCAL_SRC_FILES) \
    $(MFX_LOCAL_SRC_FILES_HW) \
    $(MFX_SHARED_FILES_HW)

LOCAL_C_INCLUDES := \
    $(MFX_LOCAL_INCLUDES_HW) \
    $(UMC_LOCAL_INCLUDES_HW) \
    $(MFX_INCLUDES_INTERNAL_HW)

LOCAL_CPPFLAGS += -std=c++14

LOCAL_CFLAGS := \
    $(MFX_CFLAGS_INTERNAL_HW) \
    -Wno-error -Wno-unused-parameter -Wno-implicit-fallthrough

LOCAL_CFLAGS_32 := $(MFX_CFLAGS_INTERNAL_32)
LOCAL_CFLAGS_64 := $(MFX_CFLAGS_INTERNAL_64)

LOCAL_MODULE_TAGS := optional
LOCAL_MODULE := libmfx_core_hw
LOCAL_SHARED_LIBRARIES := liblog libcutils

include $(BUILD_STATIC_LIBRARY)

# =============================================================================

include $(CLEAR_VARS)
include $(MFX_HOME)/android/mfx_defs.mk

LOCAL_SRC_FILES := $(MFX_LIB_SHARED_FILES_1) $(MFX_LIB_SHARED_FILES_2)

LOCAL_C_INCLUDES := \
    $(MFX_LOCAL_INCLUDES_HW) \
    $(UMC_LOCAL_INCLUDES_HW) \
    $(MFX_INCLUDES_INTERNAL_HW)

LOCAL_CFLAGS := \
    $(MFX_CFLAGS_INTERNAL_HW) \
    -Wno-error -Wno-unused-parameter -Wno-implicit-fallthrough

LOCAL_CFLAGS_32 := $(MFX_CFLAGS_INTERNAL_32)
LOCAL_CFLAGS_64 := $(MFX_CFLAGS_INTERNAL_64)

LOCAL_LDFLAGS := $(MFX_LOCAL_LDFLAGS_HW)

LOCAL_CPPFLAGS += -std=c++14

LOCAL_WHOLE_STATIC_LIBRARIES := $(MFX_LOCAL_STATIC_LIBRARIES_HW)
LOCAL_SHARED_LIBRARIES := libva liblog libcutils libdrm libva-android

LOCAL_MODULE_TAGS := optional
LOCAL_MODULE := libmfx-gen

include $(BUILD_SHARED_LIBRARY)
