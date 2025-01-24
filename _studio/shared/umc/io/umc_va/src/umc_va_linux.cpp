// Copyright (c) 2006-2024 Intel Corporation
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

#include <umc_va_base.h>

#include "umc_defs.h"
#include "umc_decrypt.h"
#include "umc_va_linux.h"
#include "umc_va_video_processing.h"
#include "mfx_trace.h"
#include "umc_frame_allocator.h"
#include "mfxstructures.h"
#include <va/va_dec_vvc.h>
#include "va_protected_content_private.h"

#define UMC_VA_NUM_OF_COMP_BUFFERS       8
#define UMC_VA_DECODE_STREAM_OUT_ENABLE  2

UMC::Status va_to_umc_res(VAStatus va_res)
{
    UMC::Status umcRes = UMC::UMC_OK;

    switch (va_res)
    {
    case VA_STATUS_SUCCESS:
        umcRes = UMC::UMC_OK;
        break;
    case VA_STATUS_ERROR_ALLOCATION_FAILED:
        umcRes = UMC::UMC_ERR_ALLOC;
        break;
    case VA_STATUS_ERROR_ATTR_NOT_SUPPORTED:
    case VA_STATUS_ERROR_UNSUPPORTED_PROFILE:
    case VA_STATUS_ERROR_UNSUPPORTED_ENTRYPOINT:
    case VA_STATUS_ERROR_UNSUPPORTED_RT_FORMAT:
    case VA_STATUS_ERROR_UNSUPPORTED_BUFFERTYPE:
    case VA_STATUS_ERROR_FLAG_NOT_SUPPORTED:
    case VA_STATUS_ERROR_RESOLUTION_NOT_SUPPORTED:
        umcRes = UMC::UMC_ERR_UNSUPPORTED;
        break;
    case VA_STATUS_ERROR_INVALID_DISPLAY:
    case VA_STATUS_ERROR_INVALID_CONFIG:
    case VA_STATUS_ERROR_INVALID_CONTEXT:
    case VA_STATUS_ERROR_INVALID_SURFACE:
    case VA_STATUS_ERROR_INVALID_BUFFER:
    case VA_STATUS_ERROR_INVALID_IMAGE:
    case VA_STATUS_ERROR_INVALID_SUBPICTURE:
        umcRes = UMC::UMC_ERR_INVALID_PARAMS;
        break;
    case VA_STATUS_ERROR_INVALID_PARAMETER:
        umcRes = UMC::UMC_ERR_INVALID_PARAMS;
        break;
    case VA_STATUS_ERROR_DECODING_ERROR:
        umcRes = UMC::UMC_ERR_DEVICE_FAILED;
        break;
    case VA_STATUS_ERROR_HW_BUSY:
        umcRes = UMC::UMC_ERR_GPU_HANG;
        break;
    default:
        umcRes = UMC::UMC_ERR_FAILED;
        break;
    }
    return umcRes;
}

VAEntrypoint umc_to_va_entrypoint(uint32_t umc_entrypoint)
{
    VAEntrypoint va_entrypoint = (VAEntrypoint)-1;

    switch (umc_entrypoint)
    {
    case UMC::VA_VLD:
    case UMC::VA_VLD | UMC::VA_PROFILE_444:
    case UMC::VA_VLD | UMC::VA_PROFILE_10:
    case UMC::VA_VLD | UMC::VA_PROFILE_10 | UMC::VA_PROFILE_444:
    case UMC::VA_VLD | UMC::VA_PROFILE_REXT:
    case UMC::VA_VLD | UMC::VA_PROFILE_REXT | UMC::VA_PROFILE_10:
    case UMC::VA_VLD | UMC::VA_PROFILE_REXT | UMC::VA_PROFILE_422:
    case UMC::VA_VLD | UMC::VA_PROFILE_REXT | UMC::VA_PROFILE_444:
    case UMC::VA_VLD | UMC::VA_PROFILE_REXT | UMC::VA_PROFILE_10 | UMC::VA_PROFILE_422:
    case UMC::VA_VLD | UMC::VA_PROFILE_REXT | UMC::VA_PROFILE_10 | UMC::VA_PROFILE_444:
    case UMC::VA_VLD | UMC::VA_PROFILE_12:
    case UMC::VA_VLD | UMC::VA_PROFILE_12 | UMC::VA_PROFILE_444:
    case UMC::VA_VLD | UMC::VA_PROFILE_REXT | UMC::VA_PROFILE_12:
    case UMC::VA_VLD | UMC::VA_PROFILE_REXT | UMC::VA_PROFILE_12 | UMC::VA_PROFILE_422:
    case UMC::VA_VLD | UMC::VA_PROFILE_REXT | UMC::VA_PROFILE_12 | UMC::VA_PROFILE_444:
    case UMC::VA_VLD | UMC::VA_PROFILE_SCC:
    case UMC::VA_VLD | UMC::VA_PROFILE_SCC  | UMC::VA_PROFILE_10:
    case UMC::VA_VLD | UMC::VA_PROFILE_SCC  | UMC::VA_PROFILE_444:
    case UMC::VA_VLD | UMC::VA_PROFILE_SCC  | UMC::VA_PROFILE_444 | UMC::VA_PROFILE_10:
        va_entrypoint = VAEntrypointVLD;
        break;
    default:
        va_entrypoint = (VAEntrypoint)-1;
        break;
    }
    return va_entrypoint;
}

// profile priorities for codecs
uint32_t g_Profiles[] =
{
    UMC::MPEG2_VLD,
    UMC::H264_VLD,
    UMC::H265_VLD,
    UMC::VC1_VLD,
    UMC::VP8_VLD,
    UMC::VP9_VLD,
    UMC::JPEG_VLD,
#if defined(MFX_ENABLE_AV1_VIDEO_DECODE)
    UMC::AV1_VLD,
#endif
#if defined(MFX_ENABLE_VVC_VIDEO_DECODE)
    UMC::VVC_VLD,
#endif
};

// va profile priorities for different codecs
VAProfile g_Mpeg2Profiles[] =
{
    VAProfileMPEG2Main, VAProfileMPEG2Simple
};

VAProfile g_H264Profiles[] =
{
    VAProfileH264High, VAProfileH264Main, VAProfileH264ConstrainedBaseline
};

VAProfile g_H265Profiles[] =
{
    VAProfileHEVCMain
    //VAProfileHEVCMain444,
};

VAProfile g_H26510BitsProfiles[] =
{
    VAProfileHEVCMain10
    //VAProfileHEVCMain422_10,
    //VAProfileHEVCMain444_10,
};

VAProfile g_VC1Profiles[] =
{
    VAProfileVC1Advanced, VAProfileVC1Main, VAProfileVC1Simple
};

VAProfile g_VP8Profiles[] =
{
    VAProfileVP8Version0_3
};

VAProfile g_VP9Profiles[] =
{
    VAProfileVP9Profile1, // chroma subsampling: 4:2:0, 4:2:2, 4:4:4
    VAProfileVP9Profile0  // chroma subsampling: 4:2:0
};

VAProfile g_VP910BitsProfiles[] =
{
    VAProfileVP9Profile3, // chroma subsampling: 4:2:0, 4:2:2, 4:4:4
    VAProfileVP9Profile2  // chroma subsampling: 4:2:0
};

#if defined(MFX_ENABLE_AV1_VIDEO_DECODE)
VAProfile g_AV1Profiles[] =
{
    VAProfileAV1Profile0
};
VAProfile g_AV110BitsPProfiles[] =
{
    VAProfileAV1Profile0
};
#endif

VAProfile g_JPEGProfiles[] =
{
    VAProfileJPEGBaseline
};

#if defined(MFX_ENABLE_VVC_VIDEO_DECODE)
VAProfile g_VVCProfiles[] =
{
    (VAProfile)VAProfileVVCMain10
};
#endif

VAProfile get_next_va_profile(uint32_t umc_codec, uint32_t profile)
{
    VAProfile va_profile = (VAProfile)-1;

    switch (umc_codec)
    {
    case UMC::VA_MPEG2:
        if (profile < UMC_ARRAY_SIZE(g_Mpeg2Profiles)) va_profile = g_Mpeg2Profiles[profile];
        break;
    case UMC::VA_H264:
        if (profile < UMC_ARRAY_SIZE(g_H264Profiles)) va_profile = g_H264Profiles[profile];
        break;
    case UMC::VA_H265:
        if (profile < UMC_ARRAY_SIZE(g_H265Profiles)) va_profile = g_H265Profiles[profile];
        break;
    case UMC::VA_H265| UMC::VA_PROFILE_422 | UMC::VA_PROFILE_REXT:
        if (profile < 1) va_profile = VAProfileHEVCMain422_10;
        break;
    case UMC::VA_H265| UMC::VA_PROFILE_444 | UMC::VA_PROFILE_REXT:
        if (profile < 1) va_profile = VAProfileHEVCMain444;
        break;
    case UMC::VA_H265 | UMC::VA_PROFILE_10:
        if (profile < UMC_ARRAY_SIZE(g_H26510BitsProfiles)) va_profile = g_H26510BitsProfiles[profile];
        break;
    case UMC::VA_H265 | UMC::VA_PROFILE_REXT:
    case UMC::VA_H265 | UMC::VA_PROFILE_REXT | UMC::VA_PROFILE_10:
    case UMC::VA_H265 | UMC::VA_PROFILE_REXT | UMC::VA_PROFILE_10 | UMC::VA_PROFILE_422:
        if (profile < 1) va_profile = VAProfileHEVCMain422_10;
        break;
    case UMC::VA_H265 | UMC::VA_PROFILE_REXT | UMC::VA_PROFILE_10 | UMC::VA_PROFILE_444:
        if (profile < 1) va_profile = VAProfileHEVCMain444_10;
        break;
    case UMC::VA_H265 | UMC::VA_PROFILE_REXT | UMC::VA_PROFILE_12:
    case UMC::VA_H265 | UMC::VA_PROFILE_REXT | UMC::VA_PROFILE_12 | UMC::VA_PROFILE_422:
        if (profile < 1) va_profile = VAProfileHEVCMain422_12;
        break;
    case UMC::VA_H265 | UMC::VA_PROFILE_REXT | UMC::VA_PROFILE_12 | UMC::VA_PROFILE_444:
        if (profile < 1) va_profile = VAProfileHEVCMain444_12;
        break;
    case UMC::VA_H265 | UMC::VA_PROFILE_SCC:
        if (profile < 1) va_profile = VAProfileHEVCSccMain;
        break;
    case UMC::VA_H265 | UMC::VA_PROFILE_SCC | UMC::VA_PROFILE_10:
        if (profile < 1) va_profile = VAProfileHEVCSccMain10;
        break;
    case UMC::VA_H265 | UMC::VA_PROFILE_SCC | UMC::VA_PROFILE_444:
        if (profile < 1) va_profile = VAProfileHEVCSccMain444;
        break;
    case UMC::VA_H265 | UMC::VA_PROFILE_SCC | UMC::VA_PROFILE_444 | UMC::VA_PROFILE_10:
        if (profile < 1) va_profile = VAProfileHEVCSccMain444_10;
        break;
    case UMC::VA_VC1:
        if (profile < UMC_ARRAY_SIZE(g_VC1Profiles)) va_profile = g_VC1Profiles[profile];
        break;
    case UMC::VA_VP8:
        if (profile < UMC_ARRAY_SIZE(g_VP8Profiles)) va_profile = g_VP8Profiles[profile];
        break;
    case UMC::VA_VP9:
    case UMC::VA_VP9 | UMC::VA_PROFILE_444:
        if (profile < UMC_ARRAY_SIZE(g_VP9Profiles)) va_profile = g_VP9Profiles[profile];
        break;
    case UMC::VA_VP9 | UMC::VA_PROFILE_10:
    case UMC::VA_VP9 | UMC::VA_PROFILE_444 | UMC::VA_PROFILE_10:
        if (profile < UMC_ARRAY_SIZE(g_VP910BitsProfiles)) va_profile = g_VP910BitsProfiles[profile];
        break;
    case UMC::VA_VP9 | UMC::VA_PROFILE_12:
    case UMC::VA_VP9 | UMC::VA_PROFILE_12 | UMC::VA_PROFILE_444:
        if (profile < UMC_ARRAY_SIZE(g_VP910BitsProfiles)) va_profile = g_VP910BitsProfiles[profile];
        break;
#if defined(MFX_ENABLE_AV1_VIDEO_DECODE)
    case UMC::VA_AV1:
        if (profile < UMC_ARRAY_SIZE(g_AV1Profiles)) va_profile = g_AV1Profiles[profile];
        break;
    case UMC::VA_AV1 | UMC::VA_PROFILE_10:
        if (profile < UMC_ARRAY_SIZE(g_AV110BitsPProfiles)) va_profile = g_AV110BitsPProfiles[profile];
        break;
#endif
    case UMC::VA_JPEG:
        if (profile < UMC_ARRAY_SIZE(g_JPEGProfiles)) va_profile = g_JPEGProfiles[profile];
        break;
#if defined(MFX_ENABLE_VVC_VIDEO_DECODE)
    case UMC::VA_VVC:
    case UMC::VA_VVC | UMC::VA_PROFILE_10:
        if (profile < UMC_ARRAY_SIZE(g_VVCProfiles)) va_profile = g_VVCProfiles[profile];
        break;
#endif
    default:
        va_profile = (VAProfile)-1;
        break;
    }
    return va_profile;
}

namespace UMC
{

VACompBuffer::VACompBuffer(void)
{
    m_NumOfItem = 0;
    m_index     = -1;
    m_id        = -1;
    m_bDestroy  = false;
}

VACompBuffer::~VACompBuffer(void)
{
}

Status VACompBuffer::SetBufferInfo(int32_t _type, int32_t _id, int32_t _index)
{
    type  = _type;
    m_id    = _id;
    m_index = _index;
    return UMC_OK;
}

Status VACompBuffer::SetDestroyStatus(bool _destroy)
{
    UNREFERENCED_PARAMETER(_destroy);
    m_bDestroy = true;
    return UMC_OK;
}

LinuxVideoAccelerator::LinuxVideoAccelerator(void)
    : m_sDecodeTraceStart("")
    , m_sDecodeTraceEnd("")
{
    m_dpy        = NULL;
    m_pContext   = NULL;
    m_pConfigId  = NULL;
    m_pKeepVAState = NULL;
    m_FrameState = lvaBeforeBegin;

    m_pCompBuffers  = NULL;
    m_uiCompBuffersNum  = 0;
    m_uiCompBuffersUsed = 0;

#if defined(ANDROID)
    m_isUseStatuReport  = false;
#else
    m_isUseStatuReport  = true;
#endif

    m_bH264MVCSupport   = false;
    m_protectedSessionID = VA_INVALID_ID;
    m_heci_sessionID = VA_INVALID_ID;
    m_key_session = -1;
    memset(&m_guidDecoder, 0 , sizeof(GUID));
}

LinuxVideoAccelerator::~LinuxVideoAccelerator(void)
{
    Close();
}

Status LinuxVideoAccelerator::Init(VideoAcceleratorParams* pInfo)
{
    MFX_AUTO_LTRACE(MFX_TRACE_LEVEL_HOTSPOTS, "LinuxVideoAccelerator::Init");
    Status         umcRes = UMC_OK;
    VAStatus       va_res = VA_STATUS_SUCCESS;
    VAConfigAttrib va_attributes[UMC_VA_LINUX_ATTRIB_SIZE];

    LinuxVideoAcceleratorParams* pParams = DynamicCast<LinuxVideoAcceleratorParams>(pInfo);
    int32_t width = 0, height = 0;
    bool needRecreateContext = false;

    // checking errors in input parameters
    if ((UMC_OK == umcRes) && (NULL == pParams))
        umcRes = UMC_ERR_NULL_PTR;
    if ((UMC_OK == umcRes) && (NULL == pParams->m_pVideoStreamInfo))
        umcRes = UMC_ERR_INVALID_PARAMS;
    if ((UMC_OK == umcRes) && (NULL == pParams->m_Display))
        umcRes = UMC_ERR_INVALID_PARAMS;
    if ((UMC_OK == umcRes) && (NULL == pParams->m_pConfigId))
        umcRes = UMC_ERR_INVALID_PARAMS;
    if ((UMC_OK == umcRes) && (NULL == pParams->m_pContext))
        umcRes = UMC_ERR_INVALID_PARAMS;
    if ((UMC_OK == umcRes) && (NULL == pParams->m_pKeepVAState))
        umcRes = UMC_ERR_INVALID_PARAMS;
    if ((UMC_OK == umcRes) && (NULL == pParams->m_allocator))
        umcRes = UMC_ERR_INVALID_PARAMS;
    if ((UMC_OK == umcRes) && (pParams->m_iNumberSurfaces < 0))
        umcRes = UMC_ERR_INVALID_PARAMS;

    // filling input parameters
    if (UMC_OK == umcRes)
    {
        m_dpy               = pParams->m_Display;
        m_pKeepVAState      = pParams->m_pKeepVAState;
        width               = pParams->m_pVideoStreamInfo->clip_info.width;
        height              = pParams->m_pVideoStreamInfo->clip_info.height;
        m_allocator         = pParams->m_allocator;
        m_FrameState        = lvaBeforeBegin;
        m_secure            = pParams->m_secure;

        // profile or stream type should be set
        if (UNKNOWN == (m_Profile & VA_CODEC))
        {
            umcRes = UMC_ERR_INVALID_PARAMS;
        }
    }
    if ((UMC_OK == umcRes) && (UNKNOWN == m_Profile))
        umcRes = UMC_ERR_INVALID_PARAMS;

    SetTraceStrings(m_Profile & VA_CODEC);

    // display initialization
    if (UMC_OK == umcRes)
    {
        int32_t i,j;
        int va_max_num_profiles      = vaMaxNumProfiles   (m_dpy);
        int va_max_num_entrypoints   = vaMaxNumEntrypoints(m_dpy);
        int va_num_profiles          = 0;
        int va_num_entrypoints       = 0;
        VAProfile*    va_profiles    = NULL;
        VAEntrypoint* va_entrypoints = NULL;
        VAProfile     va_profile     = (VAProfile)-1;
        VAEntrypoint  va_entrypoint  = (VAEntrypoint)-1;

        if (UMC_OK == umcRes)
        {
            if ((va_max_num_profiles <= 0) || (va_max_num_entrypoints <= 0))
                umcRes = UMC_ERR_FAILED;
        }
        if (UMC_OK == umcRes)
        {
            va_profiles    = new VAProfile[va_max_num_profiles];
            va_entrypoints = new VAEntrypoint[va_max_num_entrypoints];
            PERF_UTILITY_AUTO("vaQueryConfigProfiles", PERF_LEVEL_DDI);
            va_res = vaQueryConfigProfiles(m_dpy, va_profiles, &va_num_profiles);
            umcRes = va_to_umc_res(va_res);
        }

        if (UMC_OK == umcRes)
        {
            // checking support of some profile
            for (i = 0; (va_profile = get_next_va_profile(m_Profile & (VA_PROFILE | VA_CODEC), i)) != -1; ++i)
            {
                for (j = 0; j < va_num_profiles; ++j)
                {
                    if (va_profile == va_profiles[j])
                        break;
                }
                if (j < va_num_profiles)
                {
                    break;
                }
                else
                {
                    va_profile = (VAProfile)-1;
                    continue;
                }
            }
            if (va_profile < 0) umcRes = UMC_ERR_FAILED;

            if ((m_Profile & VA_CODEC) == UMC::VA_VC1)
            {
                if ((VC1_VIDEO_RCV == pInfo->m_pVideoStreamInfo->stream_subtype)
                  || (WMV3_VIDEO == pInfo->m_pVideoStreamInfo->stream_subtype))
                    va_profile = VAProfileVC1Main;
                else
                    va_profile = VAProfileVC1Advanced;
            }
        }
        if (UMC_OK == umcRes)
        {
            PERF_UTILITY_AUTO("vaQueryConfigEntrypoints", PERF_LEVEL_DDI);
            va_res = vaQueryConfigEntrypoints(m_dpy, va_profile, va_entrypoints, &va_num_entrypoints);
            umcRes = va_to_umc_res(va_res);
        }
        if (UMC_OK == umcRes)
        {
            uint32_t k = 0, profile = UNKNOWN;

            for (k = 0; k < UMC_ARRAY_SIZE(g_Profiles); ++k)
            {
                if (!(m_Profile & VA_ENTRY_POINT))
                {
                    // entrypoint is not specified, we may chose
                    if (m_Profile != (g_Profiles[k] & VA_CODEC)) continue;
                    profile = g_Profiles[k];
                }
                else
                {
                    // codec and entrypoint are specified, we just need to check validity
                    profile = m_Profile;
                }
                va_entrypoint = umc_to_va_entrypoint(profile & VA_ENTRY_POINT);
                for (i = 0; i < va_num_entrypoints; ++i) if (va_entrypoint == va_entrypoints[i]) break;
                if (i < va_num_entrypoints) break;
                else
                {
                    va_entrypoint = (VAEntrypoint)-1;
                    if (m_Profile == profile) break;
                    else continue;
                }
            }
            m_Profile = (UMC::VideoAccelerationProfile)profile;
            if (va_entrypoint == (VAEntrypoint)-1) umcRes = UMC_ERR_FAILED;
        }
        if (UMC_OK == umcRes)
        {
            int nattr = 0;
            // Assuming finding VLD, find out the format for the render target
            va_attributes[nattr++].type = VAConfigAttribRTFormat;

            va_attributes[nattr].type = VAConfigAttribDecSliceMode;
            va_attributes[nattr].value = VA_DEC_SLICE_MODE_NORMAL;
            nattr++;

            va_attributes[nattr++].type = VAConfigAttribDecProcessing;

            va_attributes[nattr++].type = VAConfigAttribEncryption;

            PERF_UTILITY_AUTO("vaGetConfigAttributes", PERF_LEVEL_DDI);
            va_res = vaGetConfigAttributes(m_dpy, va_profile, va_entrypoint, va_attributes, nattr);
            umcRes = va_to_umc_res(va_res);
        }

        int32_t attribsNumber = 4;
        // int32_t attribsNumber = 2;
        if (UMC_OK == umcRes)
        {
            umcRes = SetAttributes(va_profile, pParams, va_attributes, &attribsNumber);
            for (int i = 0; i < UMC_VA_LINUX_ATTRIB_SIZE; i++)
            {
                if (va_attributes[i].type == VAConfigAttribEncryption)
                {
                    MFX_LTRACE_MSG(MFX_TRACE_LEVEL_EXTCALL, "set VAConfigAttribEncryption = VA_ENCRYPTION_TYPE_SUBSAMPLE_CTR");
                    va_attributes[i].value = VA_ENCRYPTION_TYPE_SUBSAMPLE_CTR;
                }
            }
        }

        if (UMC_OK == umcRes)
        {
            m_pConfigId = pParams->m_pConfigId;
            if (*m_pConfigId == VA_INVALID_ID)
            {
                PERF_UTILITY_AUTO("vaCreateConfig", PERF_LEVEL_DDI);
                va_res = vaCreateConfig(m_dpy, va_profile, va_entrypoint, va_attributes, attribsNumber, m_pConfigId);
                umcRes = va_to_umc_res(va_res);
                needRecreateContext = true;
            }
        }

        delete[] va_profiles;
        delete[] va_entrypoints;
    }

    // creating context
    if (UMC_OK == umcRes)
    {
        m_pContext = pParams->m_pContext;
        if ((*m_pContext != VA_INVALID_ID) && needRecreateContext)
        {
            vaDestroyContext(m_dpy, *m_pContext);
            *m_pContext = VA_INVALID_ID;
        }
        if (*m_pContext == VA_INVALID_ID)
        {
            MFX_AUTO_LTRACE(MFX_TRACE_LEVEL_EXTCALL, "vaCreateContext");

            assert(!pParams->m_surf && "render targets tied to the context shoul be NULL");
            PERF_UTILITY_AUTO("vaCreateContext", PERF_LEVEL_DDI);
            va_res = vaCreateContext(m_dpy, *m_pConfigId, width, height, pParams->m_CreateFlags, NULL, 0, m_pContext);

            umcRes = va_to_umc_res(va_res);
        }

        if (m_secure)
        {
            m_protectedSessionID = CreateProtectedSession(VA_PC_SESSION_MODE_HEAVY,
                                    VA_PC_SESSION_TYPE_DISPLAY, VAEntrypointProtectedContent, EncryptionScheme::kCenc);
            umcRes = AttachProtectedSession(m_protectedSessionID);
        }
    }
    return umcRes;
}

Status LinuxVideoAccelerator::SetAttributes(VAProfile va_profile, LinuxVideoAcceleratorParams* pParams, VAConfigAttrib *attribute, int32_t *attribsNumber)
{
    MFX_CHECK(pParams != nullptr, UMC_ERR_INVALID_PARAMS);
    MFX_CHECK(attribute != nullptr, UMC_ERR_INVALID_PARAMS);
    MFX_CHECK(attribsNumber != nullptr, UMC_ERR_INVALID_PARAMS);

    attribute[1].value = VA_DEC_SLICE_MODE_NORMAL;

    if (pParams->m_needVideoProcessingVA)
    {
        if (attribute[2].value == VA_DEC_PROCESSING)
        {
#ifndef MFX_DEC_VIDEO_POSTPROCESS_DISABLE
            m_videoProcessingVA = new VideoProcessingVA();
#endif
            (*attribsNumber)++;
        }
        // VA_DEC_PROCESSING_NONE returned, but for VAProfileJPEGBaseline
        // current driver doesn't report VAConfigAttribDecProcessing status correctly:
        // decoding and CSC to ARGB in SFC mode works despite VA_DEC_PROCESSING_NONE.
        // Do not create m_videoProcessingVA in this case, because it's not used during jpeg decode.
        else if (va_profile == VAProfileJPEGBaseline)
        {
            (*attribsNumber)++;
        }
        else
        {
            return UMC_ERR_FAILED;
        }
    }

    return UMC_OK;
}

VAProtectedSessionID LinuxVideoAccelerator::CreateProtectedSession(uint32_t session_mode,
                                                                   uint32_t session_type,
                                                                   VAEntrypoint entrypoint,
                                                                   EncryptionScheme encryption_scheme)
{
    MFX_AUTO_LTRACE(MFX_TRACE_LEVEL_HOTSPOTS, "LinuxVideoAccelerator::CreateProtectedSession");
    VAStatus va_status = VA_STATUS_SUCCESS;

    int num_entrypoints = vaMaxNumEntrypoints(m_dpy);
    MFX_CHECK(num_entrypoints > 0, VA_INVALID_ID);

    std::unique_ptr<VAEntrypoint[]> entrypoints(new VAEntrypoint[num_entrypoints]);

    MFX_CHECK(entrypoints, VA_INVALID_ID);

    va_status = vaQueryConfigEntrypoints(m_dpy, VAProfileProtected,
                                        entrypoints.get(), &num_entrypoints);

    MFX_CHECK(VA_STATUS_SUCCESS == va_status, VA_INVALID_ID);

    int entr = 0;
    for (entr = 0; entr < num_entrypoints; entr++) {
        if (entrypoints[entr] == entrypoint)
        break;
    }
    MFX_CHECK(entr != num_entrypoints, VA_INVALID_ID);

    /* CP entrypoint found, find out the types of the crypto session support */
    int attrib_count = 2;
    VAConfigAttrib attrib_cp[7];
    attrib_cp[0].type =
        (VAConfigAttribType)VAConfigAttribProtectedContentSessionMode;
    attrib_cp[1].type =
        (VAConfigAttribType)VAConfigAttribProtectedContentSessionType;
    attrib_cp[2].type =
        (VAConfigAttribType)VAConfigAttribProtectedContentCipherAlgorithm;
    attrib_cp[3].type =
        (VAConfigAttribType)VAConfigAttribProtectedContentCipherBlockSize;
    attrib_cp[4].type =
        (VAConfigAttribType)VAConfigAttribProtectedContentCipherMode;
    attrib_cp[5].type =
        (VAConfigAttribType)VAConfigAttribProtectedContentCipherSampleType;
    attrib_cp[6].type = (VAConfigAttribType)VAConfigAttribProtectedContentUsage;
    attrib_count = 7;

    va_status = vaGetConfigAttributes(m_dpy, VAProfileProtected, VAEntrypointProtectedContent,
                                        attrib_cp, attrib_count);
    MFX_CHECK(VA_STATUS_SUCCESS == va_status, VA_INVALID_ID);

    attrib_cp[0].value = session_mode;
    attrib_cp[1].value = session_type;
    attrib_cp[2].value = VA_PC_CIPHER_AES;
    attrib_cp[3].value = VA_PC_BLOCK_SIZE_128;
    attrib_cp[4].value = VA_PC_CIPHER_MODE_CTR;
    if (EncryptionScheme::kCenc == encryption_scheme)
        attrib_cp[4].value = VA_PC_CIPHER_MODE_CBC;
    attrib_cp[5].value = VA_PC_SAMPLE_TYPE_SUBSAMPLE;
    attrib_cp[6].value = VA_PC_USAGE_DEFAULT;

    VAConfigID config_id;
    va_status = vaCreateConfig(m_dpy, VAProfileProtected, entrypoint, attrib_cp,
                                attrib_count, &config_id);
    if (va_status != VA_STATUS_SUCCESS)
        return VA_INVALID_ID;

    VAProtectedSessionID session = VA_INVALID_ID;
    va_status = vaCreateProtectedSession(m_dpy, config_id, &session);
    if (va_status != VA_STATUS_SUCCESS)
        return VA_INVALID_ID;

    va_status = vaDestroyConfig(m_dpy, config_id);
    if (va_status != VA_STATUS_SUCCESS)
        MFX_TRACE_1("vaDestroyConfig: ", "%d", va_status);

    MFX_CHECK(VA_STATUS_SUCCESS == va_status, VA_INVALID_ID);
    return session;
}

Status LinuxVideoAccelerator::AttachProtectedSession(VAProtectedSessionID session_id)
{
    MFX_AUTO_LTRACE(MFX_TRACE_LEVEL_EXTCALL, "AttachProtectedSession");
    Status umcRes = UMC_OK;

    if (session_id <= 0)
        umcRes = UMC_ERR_NOT_INITIALIZED;

    if (UMC_OK == umcRes) {
        auto va_res = vaAttachProtectedSession(m_dpy, *m_pContext, session_id);
        umcRes = va_to_umc_res(va_res);
    }

    return umcRes;
}

bool LinuxVideoAccelerator::InitKey()
{
    MFX_AUTO_LTRACE(MFX_TRACE_LEVEL_EXTCALL, "LinuxVideoAccelerator::InitKey");
    // Get App id
    uint32_t app_id = 0xFF;
    VABufferID buffer = 0;
    VAProtectedSessionExecuteBuffer execBuff = {0};
    VAStatus va_status;
    execBuff.function_id = VA_TEE_EXEC_GPU_FUNCID_GET_SESSION_ID;
    execBuff.input.data_size = 0;
    execBuff.input.data = nullptr;
    execBuff.output.data_size = sizeof(uint32_t);
    execBuff.output.data = (void*)&app_id;
    va_status = vaCreateBuffer(m_dpy, m_protectedSessionID,
                            VAProtectedSessionExecuteBufferType,
                            sizeof(execBuff), 1, &execBuff, &buffer);
    if (va_status) {
        MFX_TRACE_1("vaCreateBuffer() failed va_sts = ", "%d", va_status);
        return false;
    }

    va_status = vaProtectedSessionExecute(m_dpy, m_protectedSessionID, buffer);
    vaDestroyBuffer(m_dpy, buffer);
    if (va_status) {
        MFX_TRACE_1("vaProtectedSessionExecute fail va_status = ", "%d", va_status);
        return false;
    }
    app_id = app_id & 0x7F;  // remove bit7 for app_type information
    MFX_TRACE_I(app_id);

    // GetWrappedTitleKey
    struct wv20_get_wrapped_title_keys_in cmd_in;
    cmd_in.header.command_id = wv20_get_wrapped_title_keys; // command id
    cmd_in.header.stream_id_42.fields.valid = 1;
    cmd_in.header.stream_id_42.fields.app_type = 0; // pavp::PAVP_APPTYPE_DISPLAYABLE
    cmd_in.header.stream_id_42.fields.stream_id = app_id;
    cmd_in.header.buffer_len = sizeof(wv20_get_wrapped_title_keys_in) - sizeof(pavp_cmd_header_t);
    cmd_in.session_id = m_key_session;
    MFX_TRACE_I(cmd_in.session_id);
    auto pCmd_out = std::make_unique<uint8_t[]>(PAVP_HECI_IO_BUFFER_SIZE + sizeof(wv20_get_wrapped_title_keys_out));
    auto cmd_out = reinterpret_cast<wv20_get_wrapped_title_keys_out*>(pCmd_out.get());
    cmd_out->buffer_size = PAVP_HECI_IO_BUFFER_SIZE;
    PassThrough(&cmd_in, sizeof(wv20_get_wrapped_title_keys_in), cmd_out, PAVP_HECI_IO_BUFFER_SIZE + sizeof(wv20_get_wrapped_title_keys_out));

    MFX_LTRACE_MSG(MFX_TRACE_LEVEL_HOTSPOTS, "get title key +");
    const wrapped_title_key_t* wtk = reinterpret_cast<const wrapped_title_key_t*>(
                                    cmd_out->buffer + cmd_out->title_key_obj_offset);
    MFX_TRACE_I(cmd_out->title_key_obj_offset);
    MFX_TRACE_I(wtk->key_id_size);
    for (uint32_t i = 0; i < cmd_out->num_keys; i++, wtk++)
    {
        if ((uint64_t)wtk->key_id_offset + wtk->key_id_size > cmd_out->buffer_size)
        {
            MFX_LTRACE_MSG(MFX_TRACE_LEVEL_HOTSPOTS, "Offset points past end of buffer");
            return false;
        }
        const uint8_t* key_id_to_compare = cmd_out->buffer + wtk->key_id_offset;
        if (memcmp(m_selectKey.data(), key_id_to_compare, wtk->key_id_size) == 0)
        {
            // key blob
            uint8_t* key_blob_start = cmd_out->buffer + wtk->enc_title_key_offset;
            m_key_blob = std::vector<uint8_t>(key_blob_start, key_blob_start + kDecryptionKeySize);
            return true;
        }
    }

    MFX_LTRACE_MSG(MFX_TRACE_LEVEL_HOTSPOTS, "Cannot find right key blob!");
    return false;
}

bool LinuxVideoAccelerator::PassThrough(void* input, size_t input_size, void* output, size_t output_size)
{
    MFX_AUTO_LTRACE(MFX_TRACE_LEVEL_EXTCALL, "LinuxVideoAccelerator::PassThrough");
    if (VA_INVALID_ID == m_heci_sessionID)
    {
        // create HECI session
        m_heci_sessionID = CreateProtectedSession(VA_PC_SESSION_MODE_NONE, VA_PC_SESSION_TYPE_NONE,
                                            VAEntrypointProtectedTEEComm, EncryptionScheme::kCenc);
        if (m_heci_sessionID == VA_INVALID_ID) {
            MFX_LTRACE_MSG(MFX_TRACE_LEVEL_EXTCALL,"Create HECI session fails");
            return false;
        }
    }

    VABufferID buffer;
    VAProtectedSessionExecuteBuffer execBuff = {0};
    execBuff.function_id = VA_TEE_EXECUTE_FUNCTION_ID_PASS_THROUGH;
    execBuff.input.data_size = input_size;
    execBuff.input.data = input;
    execBuff.output.data_size = output_size;
    execBuff.output.data = output;
    VAStatus va_status = vaCreateBuffer(m_dpy, m_heci_sessionID, VAProtectedSessionExecuteBufferType,
                           sizeof(execBuff), 1, &execBuff, &buffer);
    if (va_status) {
        MFX_TRACE_1("vaCreateBuffer() failed va_sts = ", "%d", va_status);
        return false;
    }
    va_status = vaProtectedSessionExecute(m_dpy, m_heci_sessionID, buffer);
    pavp_cmd_header_t* pIHeader = static_cast<pavp_cmd_header_t*>(input);
    pavp_cmd_header_t* pOHeader = static_cast<pavp_cmd_header_t*>(output);
    vaDestroyBuffer(m_dpy, buffer);
    if (va_status || pOHeader->status) {
        MFX_TRACE_3("PassThrough failed ", "command id = %d, va_status = %d, pOHeader->status = %d",
                pIHeader->command_id, va_status, pOHeader->status);
        return false;
    }
    return true;
}

bool LinuxVideoAccelerator::SetStreamKey()
{
    MFX_AUTO_LTRACE(MFX_TRACE_LEVEL_HOTSPOTS, "LinuxVideoAccelerator::SetStreamKey");
    VABufferID buffer = 0;
    VAProtectedSessionExecuteBuffer execBuff = {0};
    VAStatus va_status;
    struct PAVP_SET_STREAM_KEY_PARAMS SetStreamKeyParams;
    SetStreamKeyParams.StreamType = 0; // PAVP_SET_KEY_DECRYPT = 1
    memcpy(SetStreamKeyParams.EncryptedDecryptKey, m_key_blob.data(), kDecryptionKeySize);
    execBuff = {};
    execBuff.function_id = VA_TEE_EXEC_GPU_FUNCID_SET_STREAM_KEY;
    execBuff.input.data_size = sizeof(SetStreamKeyParams);
    execBuff.input.data = &SetStreamKeyParams;
    execBuff.output.data_size = 0;
    execBuff.output.data = nullptr;
    buffer = 0;
    va_status = vaCreateBuffer(m_dpy, m_protectedSessionID,
                            VAProtectedSessionExecuteBufferType,
                            sizeof(execBuff), 1, &execBuff, &buffer);
    if (va_status) {
        MFX_TRACE_1("FATAL:SetStreamKey: CreateBuffer fail ", "%d", va_status);
        return false;
    }

    va_status = vaProtectedSessionExecute(m_dpy, m_protectedSessionID, buffer);
    vaDestroyBuffer(m_dpy, buffer);
    if (va_status) {
        MFX_TRACE_1("FATAL:SetStreamKey: ProtectedSessionExecute fail ", "%d", va_status);
        return false;
    }
    return true;
}

bool LinuxVideoAccelerator::DecryptCTR(const mfxExtDecryptConfig& decryptConfig, VAEncryptionParameters* pEncryptionParam)
{
    if (VA_INVALID_ID == m_protectedSessionID)
    {
        m_protectedSessionID = CreateProtectedSession(VA_PC_SESSION_MODE_HEAVY,
                                VA_PC_SESSION_TYPE_DISPLAY, VAEntrypointProtectedContent, decryptConfig.encryption_scheme);
        Status umcRes = AttachProtectedSession(m_protectedSessionID);
        if (UMC_OK != umcRes) {
            MFX_LTRACE_MSG(MFX_TRACE_LEVEL_EXTCALL, "AttachProtectedSession failed!");
            MFX_TRACE_I(umcRes);
            return false;
        }
    }

    m_key_session = decryptConfig.session;
    std::vector<uint8_t> hw_key_id(decryptConfig.hw_key_id, decryptConfig.hw_key_id + kDecryptionKeySize);
    if (m_selectKey != hw_key_id)
    {
        MFX_LTRACE_MSG(MFX_TRACE_LEVEL_EXTCALL, "select changed, need to update");
        m_selectKey = hw_key_id;
        m_key_blob.clear();
    }

    if (m_key_blob.empty())
    {
        if (!InitKey())
        {
            MFX_LTRACE_MSG(MFX_TRACE_LEVEL_EXTCALL, "InitKey failed!");
            return false;
        }
    }
    if (!SetStreamKey())
    {
        MFX_LTRACE_MSG(MFX_TRACE_LEVEL_EXTCALL, "SetStreamKey failed!");
        return false;
    }

    memcpy(pEncryptionParam->wrapped_decrypt_blob, m_key_blob.data(), kDecryptionKeySize);
    return true;
}

bool LinuxVideoAccelerator::IsSecure()
{
    return m_secure;
}

Status LinuxVideoAccelerator::Close(void)
{
    MFX_AUTO_LTRACE(MFX_TRACE_LEVEL_HOTSPOTS, "LinuxVideoAccelerator::Close");

    if (NULL != m_pCompBuffers)
    {
        for (uint32_t i = 0; i < m_uiCompBuffersUsed; ++i)
        {
            if (m_pCompBuffers[i]->NeedDestroy() && (NULL != m_dpy))
            {
                VABufferID id = m_pCompBuffers[i]->GetID();
                mfxStatus sts = CheckAndDestroyVAbuffer(m_dpy, id);
                std::ignore = MFX_STS_TRACE(sts);
            }
            UMC_DELETE(m_pCompBuffers[i]);
        }
        delete[] m_pCompBuffers;
        m_pCompBuffers = nullptr;
    }
    if (NULL != m_dpy)
    {
        if ((m_pContext && (*m_pContext != VA_INVALID_ID)) && !(m_pKeepVAState && *m_pKeepVAState))
        {
            MFX_AUTO_LTRACE(MFX_TRACE_LEVEL_EXTCALL, "vaDestroyContext");
            PERF_UTILITY_AUTO("vaDestroyContext", PERF_LEVEL_DDI);
            VAStatus vaSts = vaDestroyContext(m_dpy, *m_pContext);
            std::ignore = MFX_STS_TRACE(vaSts);
            *m_pContext = VA_INVALID_ID;
        }
        if ((m_pConfigId && (*m_pConfigId != VA_INVALID_ID)) && !(m_pKeepVAState && *m_pKeepVAState))
        {
            PERF_UTILITY_AUTO("vaDestroyConfig", PERF_LEVEL_DDI);
            VAStatus vaSts = vaDestroyConfig(m_dpy, *m_pConfigId);
            std::ignore = MFX_STS_TRACE(vaSts);
            *m_pConfigId = VA_INVALID_ID;
        }

        m_dpy = NULL;
    }

#ifndef MFX_DEC_VIDEO_POSTPROCESS_DISABLE
    delete m_videoProcessingVA;
    m_videoProcessingVA = 0;
#endif

    m_FrameState = lvaBeforeBegin;
    m_uiCompBuffersNum  = 0;
    m_uiCompBuffersUsed = 0;

    m_associatedIds.clear();

    return VideoAccelerator::Close();
}

Status LinuxVideoAccelerator::BeginFrame(int32_t FrameBufIndex)
{
    Status umcRes = UMC_OK;

    MFX_CHECK(FrameBufIndex >= 0, UMC_ERR_INVALID_PARAMS);

    VASurfaceID *surface;
    Status sts = m_allocator->GetFrameHandle(FrameBufIndex, &surface);
    MFX_CHECK(sts == UMC_OK, sts);

    if (lvaBeforeBegin == m_FrameState)
    {
        {
            MFX_AUTO_LTRACE(MFX_TRACE_LEVEL_EXTCALL, "vaBeginPicture");
            MFX_LTRACE_2(MFX_TRACE_LEVEL_EXTCALL, m_sDecodeTraceStart, "%d|%d", *m_pContext, 0);
            PERF_UTILITY_AUTO("vaBeginPicture", PERF_LEVEL_DDI);
            VAStatus va_res = vaBeginPicture(m_dpy, *m_pContext, *surface);
            umcRes = va_to_umc_res(va_res);
        }

        if (UMC_OK == umcRes)
        {
            m_FrameState = lvaBeforeEnd;
            m_associatedIds.insert(*surface);
        }
    }

    return umcRes;
}

Status LinuxVideoAccelerator::AllocCompBuffers(void)
{
    Status umcRes = UMC_OK;

    if ((UMC_OK == umcRes) && (m_uiCompBuffersUsed >= m_uiCompBuffersNum))
    {
        if (NULL == m_pCompBuffers)
        {
            m_uiCompBuffersNum = UMC_VA_NUM_OF_COMP_BUFFERS;
            m_pCompBuffers = new VACompBuffer*[m_uiCompBuffersNum];
        }
        else
        {
            uint32_t uiNewCompBuffersNum = 0;
            VACompBuffer** pNewCompBuffers = NULL;

            uiNewCompBuffersNum = m_uiCompBuffersNum + UMC_VA_NUM_OF_COMP_BUFFERS;
            pNewCompBuffers = new VACompBuffer*[uiNewCompBuffersNum];

            MFX_INTERNAL_CPY((uint8_t*)pNewCompBuffers, (const uint8_t*)m_pCompBuffers, m_uiCompBuffersNum*sizeof(VACompBuffer*));
            delete[] m_pCompBuffers;
            m_uiCompBuffersNum = uiNewCompBuffersNum;
            m_pCompBuffers = pNewCompBuffers;
        }
    }
    return umcRes;
}

void* LinuxVideoAccelerator::GetCompBuffer(int32_t buffer_type, UMCVACompBuffer **buf, int32_t size, int32_t index)
{
    uint32_t i;
    VACompBuffer* pCompBuf = NULL;
    void* pBufferPointer = NULL;

    if (NULL != buf) *buf = NULL;

    std::lock_guard<std::mutex> guard(m_SyncMutex);
    for (i = 0; i < m_uiCompBuffersUsed; ++i)
    {
        pCompBuf = m_pCompBuffers[i];
        if ((pCompBuf->GetType() == buffer_type) && (pCompBuf->GetIndex() == index)) break;
    }
    if (i >= m_uiCompBuffersUsed)
    {
        AllocCompBuffers();
        pCompBuf = GetCompBufferHW(buffer_type, size, index);
        if (NULL != pCompBuf)
        {
            m_pCompBuffers[m_uiCompBuffersUsed] = pCompBuf;
            ++m_uiCompBuffersUsed;
        }
    }
    if (NULL != pCompBuf)
    {
        if (NULL != buf) *buf = pCompBuf;
        pBufferPointer = pCompBuf->GetPtr();
    }
    return pBufferPointer;
}

VACompBuffer* LinuxVideoAccelerator::GetCompBufferHW(int32_t type, int32_t size, int32_t index)
{
    MFX_AUTO_LTRACE(MFX_TRACE_LEVEL_INTERNAL, "GetCompBufferHW");
    VAStatus   va_res = VA_STATUS_SUCCESS;
    VABufferID id;
    uint8_t*      buffer = NULL;
    uint32_t     buffer_size = 0;
    VACompBuffer* pCompBuffer = NULL;

    if (VA_STATUS_SUCCESS == va_res)
    {
        VABufferType va_type         = (VABufferType)type;
        unsigned int va_size         = 0;
        unsigned int va_num_elements = 0;

        if (VASliceParameterBufferType == va_type)
        {
            switch (m_Profile & VA_CODEC)
            {
            case UMC::VA_MPEG2:
                va_size         = sizeof(VASliceParameterBufferMPEG2);
                va_num_elements = size/sizeof(VASliceParameterBufferMPEG2);
                break;
            case UMC::VA_H264:
                if (m_bShortSlice)
                {
                    va_size         = sizeof(VASliceParameterBufferBase);
                    va_num_elements = size/sizeof(VASliceParameterBufferBase);
                }
                else
                {
                    va_size         = sizeof(VASliceParameterBufferH264);
                    va_num_elements = size/sizeof(VASliceParameterBufferH264);
                }
                break;
            case UMC::VA_VC1:
                va_size         = sizeof(VASliceParameterBufferVC1);
                va_num_elements = size/sizeof(VASliceParameterBufferVC1);
                break;
            case UMC::VA_VP8:
                va_size         = sizeof(VASliceParameterBufferVP8);
                va_num_elements = size/sizeof(VASliceParameterBufferVP8);
                break;
            case UMC::VA_JPEG:
                va_size         = sizeof(VASliceParameterBufferJPEGBaseline);
                va_num_elements = size/sizeof(VASliceParameterBufferJPEGBaseline);
                break;
            case UMC::VA_VP9:
                va_size         = sizeof(VASliceParameterBufferVP9);
                va_num_elements = size/sizeof(VASliceParameterBufferVP9);
                break;
            case UMC::VA_H265:
            case UMC::VA_H265 | UMC::VA_PROFILE_10:
                va_size         = sizeof(VASliceParameterBufferHEVC);
                va_num_elements = size/sizeof(VASliceParameterBufferHEVC);
                if ((m_Profile & VA_PROFILE_REXT) || (m_Profile & VA_PROFILE_SCC))
                {
                    va_size         = sizeof(VASliceParameterBufferHEVCExtension);
                    va_num_elements = size/sizeof(VASliceParameterBufferHEVCExtension);
                }
                break;
#if defined(MFX_ENABLE_AV1_VIDEO_DECODE)
            case UMC::VA_AV1:
                va_size         = sizeof(VASliceParameterBufferAV1);
                va_num_elements = size/sizeof(VASliceParameterBufferAV1);
                break;
#endif
#if defined(MFX_ENABLE_VVC_VIDEO_DECODE)
            case UMC::VA_VVC:
            case UMC::VA_VVC | UMC::VA_PROFILE_10:
                va_size         = sizeof(VASliceParameterBufferVVC);
                va_num_elements = size/sizeof(VASliceParameterBufferVVC);
                break;
#endif
            default:
                va_size         = 0;
                va_num_elements = 0;
                break;
            }
        }
#if defined(MFX_ENABLE_VVC_VIDEO_DECODE)
        else if((m_Profile & VA_CODEC) == UMC::VA_VVC)
        {
            if( VATileBufferType == va_type)
            {
                va_size         = sizeof(uint16_t);
                va_num_elements = size/sizeof(uint16_t);
            }
            else if( VAAlfBufferType == va_type)
            {
                va_size         = sizeof(VAAlfDataVVC);
                va_num_elements = size/sizeof(VAAlfDataVVC);
            }
            else if( VALmcsBufferType == va_type)
            {
                va_size         = sizeof(VALmcsDataVVC);
                va_num_elements = size/sizeof(VALmcsDataVVC);
            }
            else if( VAIQMatrixBufferType == va_type)
            {
                va_size         = sizeof(VAScalingListVVC);
                va_num_elements = size/sizeof(VAScalingListVVC);
            }
            else if( VASliceStructBufferType == va_type)
            {
                va_size         = sizeof(VASliceStructVVC);
                va_num_elements = size/sizeof(VASliceStructVVC);
            }
            else if(VASubPicBufferType == va_type)
            {
                va_size         = sizeof(VASubPicVVC);
                va_num_elements = size/sizeof(VASubPicVVC);
            }
            else
            {
                va_size         = size;
                va_num_elements = 1;
            }
        }
#endif
        else
        {
            va_size         = size;
            va_num_elements = 1;
        }
        buffer_size = va_size * va_num_elements;

        PERF_UTILITY_AUTO("vaCreateBuffer", PERF_LEVEL_DDI);
        va_res = vaCreateBuffer(m_dpy, *m_pContext, va_type, va_size, va_num_elements, NULL, &id);
    }
    if (VA_STATUS_SUCCESS == va_res)
    {
        va_res = vaMapBuffer(m_dpy, id, (void**)&buffer);
    }
    if (VA_STATUS_SUCCESS == va_res)
    {
        pCompBuffer = new VACompBuffer();
        pCompBuffer->SetBufferPointer(buffer, buffer_size);
        pCompBuffer->SetDataSize(0);
        pCompBuffer->SetBufferInfo(type, id, index);
        pCompBuffer->SetDestroyStatus(true);
    }
    return pCompBuffer;
}

Status
LinuxVideoAccelerator::Execute()
{
    MFX_AUTO_LTRACE(MFX_TRACE_LEVEL_INTERNAL, "Execute");
    Status         umcRes = UMC_OK;
    VAStatus       va_res = VA_STATUS_SUCCESS;
    VAStatus       va_sts = VA_STATUS_SUCCESS;
    VABufferID     id;
    uint32_t       i;
    VACompBuffer*  pCompBuf = NULL;

    std::lock_guard<std::mutex> guard(m_SyncMutex);

    if (UMC_OK == umcRes)
    {
        for (i = 0; i < m_uiCompBuffersUsed; i++)
        {
            pCompBuf = m_pCompBuffers[i];
            id = pCompBuf->GetID();

            if (!m_bShortSlice)
            {
                if (pCompBuf->GetType() == VASliceParameterBufferType)
                {
                    PERF_UTILITY_AUTO("vaBufferSetNumElements", PERF_LEVEL_DDI);
                    va_sts = vaBufferSetNumElements(m_dpy, id, pCompBuf->GetNumOfItem());
                    if (VA_STATUS_SUCCESS == va_res) va_res = va_sts;
                }
            }
            {
                PERF_UTILITY_AUTO("vaUnmapBuffer", PERF_LEVEL_DDI);
                va_sts = vaUnmapBuffer(m_dpy, id);
            }
            if (VA_STATUS_SUCCESS == va_res) va_res = va_sts;


            {
                MFX_AUTO_LTRACE(MFX_TRACE_LEVEL_EXTCALL, "vaRenderPicture");
                PERF_UTILITY_AUTO("vaRenderPicture", PERF_LEVEL_DDI);
                va_sts = vaRenderPicture(m_dpy, *m_pContext, &id, 1);
                if (VA_STATUS_SUCCESS == va_res) va_res = va_sts;
            }
        }
    }

    if (UMC_OK == umcRes)
    {
        umcRes = va_to_umc_res(va_res);
    }
    return umcRes;
}

Status LinuxVideoAccelerator::EndFrame(void*)
{
    MFX_AUTO_LTRACE(MFX_TRACE_LEVEL_INTERNAL, "EndFrame");
    VAStatus va_res = VA_STATUS_SUCCESS;

    std::lock_guard<std::mutex> guard(m_SyncMutex);

    {
        MFX_AUTO_LTRACE(MFX_TRACE_LEVEL_EXTCALL, "vaEndPicture");
        PERF_UTILITY_AUTO("vaEndPicture", PERF_LEVEL_DDI);
        va_res = vaEndPicture(m_dpy, *m_pContext);
        MFX_LTRACE_2(MFX_TRACE_LEVEL_EXTCALL, m_sDecodeTraceEnd, "%d|%d", *m_pContext, 0);
    }
    std::ignore = MFX_STS_TRACE(va_res);
    Status stsRet = va_to_umc_res(va_res);

    m_FrameState = lvaBeforeBegin;

    for (uint32_t i = 0; i < m_uiCompBuffersUsed; ++i)
    {
        if (m_pCompBuffers[i]->NeedDestroy())
        {
            VABufferID id = m_pCompBuffers[i]->GetID();
            mfxStatus sts = CheckAndDestroyVAbuffer(m_dpy, id);
            std::ignore = MFX_STS_TRACE(sts);

            if (sts != MFX_ERR_NONE)
                stsRet = UMC_ERR_FAILED;
        }
        UMC_DELETE(m_pCompBuffers[i]);
    }
    m_uiCompBuffersUsed = 0;

    return stsRet;
}

int32_t LinuxVideoAccelerator::GetSurfaceID(int32_t idx) const
{
    VASurfaceID *surface;
    Status sts = UMC_OK;
    MFX_CHECK(idx >= 0, UMC_ERR_INVALID_PARAMS);

    try {
        sts = m_allocator->GetFrameHandle(idx, &surface);
    } catch (std::exception&) {
        return VA_INVALID_SURFACE;
    }

    if (sts != UMC_OK)
        return VA_INVALID_SURFACE;

    return *surface;
}

uint16_t LinuxVideoAccelerator::GetDecodingError(VASurfaceID *surface)
{
    MFX_AUTO_LTRACE(MFX_TRACE_LEVEL_EXTCALL, "GetDecodingError");
    uint16_t error = 0;

#ifndef ANDROID
    // NOTE: at the moment there is no such support for Android, so no need to execute...
    VAStatus va_sts;

    VASurfaceDecodeMBErrors* pVaDecErr = NULL;
    {
        PERF_UTILITY_AUTO("vaQuerySurfaceError", PERF_LEVEL_DDI);
        MFX_CHECK(surface != nullptr, UMC_ERR_INVALID_PARAMS);
        va_sts = vaQuerySurfaceError(m_dpy, *surface, VA_STATUS_ERROR_DECODING_ERROR, (void**)&pVaDecErr);
    }
    if (VA_STATUS_SUCCESS == va_sts)
    {
        if (NULL != pVaDecErr)
        {
            for (int i = 0; pVaDecErr[i].status != -1; ++i)
            {
                {
                    error = MFX_CORRUPTION_MAJOR;
                }

            }
        }
        else
        {
            error = MFX_CORRUPTION_MAJOR;
        }
    }
#endif
    return error;
}


void LinuxVideoAccelerator::SetTraceStrings(uint32_t umc_codec)
{
    switch (umc_codec)
    {
    case UMC::VA_MPEG2:
        m_sDecodeTraceStart = "A|DECODE|MPEG2|PACKET_START|";
        m_sDecodeTraceEnd = "A|DECODE|MPEG2|PACKET_END|";
        break;
    case UMC::VA_H264:
        m_sDecodeTraceStart = "A|DECODE|H264|PACKET_START|";
        m_sDecodeTraceEnd = "A|DECODE|H264|PACKET_END|";
        break;
    case UMC::VA_H265:
        m_sDecodeTraceStart = "A|DECODE|H265|PACKET_START|";
        m_sDecodeTraceEnd = "A|DECODE|H265|PACKET_END|";
        break;
    case UMC::VA_VC1:
        m_sDecodeTraceStart = "A|DECODE|VC1|PACKET_START|";
        m_sDecodeTraceEnd = "A|DECODE|VC1|PACKET_END|";
        break;
    case UMC::VA_VP8:
        m_sDecodeTraceStart = "A|DECODE|VP8|PACKET_START|";
        m_sDecodeTraceEnd = "A|DECODE|VP8|PACKET_END|";
        break;
    case UMC::VA_VP9:
        m_sDecodeTraceStart = "A|DECODE|VP9|PACKET_START|";
        m_sDecodeTraceEnd = "A|DECODE|VP9|PACKET_END|";
        break;
    case UMC::VA_JPEG:
        m_sDecodeTraceStart = "A|DECODE|JPEG|PACKET_START|";
        m_sDecodeTraceEnd = "A|DECODE|JPEG|PACKET_END|";
        break;
    default:
        m_sDecodeTraceStart = "";
        m_sDecodeTraceEnd = "";
        break;
    }
}

Status LinuxVideoAccelerator::QueryTaskStatus(int32_t FrameBufIndex, void * status, void * error)
{
    MFX_AUTO_LTRACE(MFX_TRACE_LEVEL_HOTSPOTS, "QueryTaskStatus");
    if (FrameBufIndex < 0)
        return UMC_ERR_INVALID_PARAMS;

    VASurfaceID *surface;
    Status sts = m_allocator->GetFrameHandle(FrameBufIndex, &surface);
    if (sts != UMC_OK)
        return sts;

    VASurfaceStatus surface_status;
    VAStatus va_status;
    VAStatus va_sts;
    {
        PERF_UTILITY_AUTO("vaQuerySurfaceStatus", PERF_LEVEL_DDI);
        va_status = vaQuerySurfaceStatus(m_dpy, *surface, &surface_status);
    }
    if ((VA_STATUS_SUCCESS == va_status) && (VASurfaceReady == surface_status))
    {
        {
            PERF_UTILITY_AUTO("vaSyncSurface", PERF_LEVEL_DDI);
            // handle decoding errors
            va_sts = vaSyncSurface(m_dpy, *surface);
        }

        if (error)
        {
            switch (va_sts)
            {
                case VA_STATUS_ERROR_DECODING_ERROR:
                    *(uint16_t*)error = GetDecodingError(surface);
                    break;

                case VA_STATUS_ERROR_HW_BUSY:
                    va_status = VA_STATUS_ERROR_HW_BUSY;
                    break;
            }
        }
    }

    if (NULL != status)
    {
        *(VASurfaceStatus*)status = surface_status; // done or not
    }

    Status umcRes = va_to_umc_res(va_status); // OK or not

    return umcRes;
}

Status LinuxVideoAccelerator::SyncTask(int32_t FrameBufIndex, void *surfCorruption)
{
    Status umcRes = 0;
    MFX_AUTO_LTRACE(MFX_TRACE_LEVEL_HOTSPOTS, "Decode DDIWaitTaskSync");

    if (FrameBufIndex < 0)
        return UMC_ERR_INVALID_PARAMS;

    VASurfaceID *surface;
    umcRes = m_allocator->GetFrameHandle(FrameBufIndex, &surface);
    if (umcRes != UMC_OK)
        return umcRes;

    VAStatus va_sts = VA_STATUS_SUCCESS;
    {
        MFX_AUTO_LTRACE(MFX_TRACE_LEVEL_EXTCALL, "vaSyncSurface");
        PERF_UTILITY_AUTO("vaSyncSurface", PERF_LEVEL_DDI);
        va_sts = vaSyncSurface(m_dpy, *surface);
    }

    TRACE_EVENT(MFX_TRACE_HOTSPOT_DDI_WAIT_TASK_SYNC, EVENT_TYPE_INFO, 0, make_event_data(FrameBufIndex, 0, va_sts));

    if (VA_STATUS_ERROR_DECODING_ERROR == va_sts)
    {
        if (surfCorruption) *(uint16_t*)surfCorruption = GetDecodingError(surface);
        return UMC_OK;
    }
    if (VA_STATUS_ERROR_OPERATION_FAILED == va_sts)
    {
        if (surfCorruption) *(uint16_t*)surfCorruption = MFX_CORRUPTION_MAJOR;
        return UMC_OK;
    }
    umcRes = va_to_umc_res(va_sts);
    return umcRes;
}

}; // namespace UMC

