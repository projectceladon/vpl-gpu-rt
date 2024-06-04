# Copyright(c) 2019 Intel Corporation
# 
# Permission is hereby granted, free of charge, to any person obtaining a
# copy of this software and associated documentation files(the "Software"),
# to deal in the Software without restriction, including without limitation
# the rights to use, copy, modify, merge, publish, distribute, sublicense,
# and / or sell copies of the Software, and to permit persons to whom the
# Software is furnished to do so, subject to the following conditions:
# 
# The above copyright notice and this permission notice shall be included
# in all copies or substantial portions of the Software.
# 
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
# OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
# THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR
# OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
# ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
# OTHER DEALINGS IN THE SOFTWARE.
# 
# library_static, library_shared, binary 
cc_@module {
    name: "@name",

    vendor: true,

    defaults: [
@defaults
    ],

    srcs: [
@srcs
    ],

    cflags: [
@cflags
    ],

    cppflags: [
        "-O3",
        "-DNDEBUG",
        "-fPIC",
        "-Wno-deprecated-declarations",
        "-Wno-unknown-pragmas",
        "-Wno-unused",
        "-std=gnu++14",
        "-DLINUX",
        "-DLINUX32",
        "-DLINUX64",
        "-DMEDIA_VERSION_STR=24.1.5",
        "-DMFX_API_VERSION=2.9+",
        "-DMFX_BUILD_INFO=Ubuntu | GNU 11.4.0 | glibc 2.35",
        "-DMFX_DEPRECATED_OFF",
        "-DMFX_GIT_COMMIT=9c1340b5",
        "-DMFX_ONEVPL",
        "-DMFX_VERSION_USE_LATEST",
        "-DMSDK_BUILD=",
        "-DNDEBUG",
        "-DONEVPL_EXPERIMENTAL",
        "-DSYNCHRONIZATION_BY_VA_SYNC_SURFACE",
        "-D_FILE_OFFSET_BITS=64",
        "-D__USE_LARGEFILE64",
    ],

    local_include_dirs: [
@local_include_dirs
    ],

    shared_libs: [
@shared_libs
    ],

    static_libs: [
@static_libs
    ],

}
