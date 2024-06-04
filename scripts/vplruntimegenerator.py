#! /usr/bin/env python3
"""
* Copyright (c) 2019, Intel Corporation
*
* Permission is hereby granted, free of charge, to any person obtaining a
* copy of this software and associated documentation files (the "Software"),
* to deal in the Software without restriction, including without limitation
* the rights to use, copy, modify, merge, publish, distribute, sublicense,
* and/or sell copies of the Software, and to permit persons to whom the
* Software is furnished to do so, subject to the following conditions:
*
* The above copyright notice and this permission notice shall be included
* in all copies or substantial portions of the Software.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
* OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
* THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR
* OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
* ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
* OTHER DEALINGS IN THE SOFTWARE.
"""

import os
import os.path as path
import re
from androidbpgenerator import INDENT, CCDefaults, ModuleInfo, Generator, NOVERBOSE

RUN_CALLABLE = True


class VplRuntimeGenerator(Generator):
    def __init__(self, root):
        self.proj = path.join(root, "onevpl-intel-gpu/")
        super(VplRuntimeGenerator, self).__init__(self.proj, root)
        self.root = root

        self.allmoduleinfo[1] = ModuleInfo("mfx_logging", "mfx_logging.bp",
            "_studio/shared/mfx_logging/CMakeFiles/mfx_logging.dir/", "library_static", "vpl-runtime-defaults",
            # updateflags = {"-Wclobbered" : "", "-Werror" : "", "-mindirect-branch-register" : "", "-Wimplicit-fallthrough=4" : "", "-Wno-noexcept-type" : "", 
            # "-Wno-unused-but-set-variable" : "", },
            )

        self.allmoduleinfo[2] = ModuleInfo("mfx_trace", "mfx_trace.bp",
            "_studio/shared/mfx_trace/CMakeFiles/mfx_trace.dir/", "library_static", "vpl-runtime-defaults",
            # updateflags = {"-Wclobbered" : "", "-Werror" : "", "-mindirect-branch-register" : "", "-Wimplicit-fallthrough=4" : "", "-Wno-noexcept-type" : "", 
            # "-Wno-unused-but-set-variable" : "", },
            )

        self.allmoduleinfo[3] = ModuleInfo("asc", "asc.bp", "_studio/shared/asc/CMakeFiles/asc.dir/",
            "library_static", "vpl-runtime-defaults", 
            # updateflags = {"-Wclobbered" : "", "-Werror" : "", "-mindirect-branch-register" : "", "-Wimplicit-fallthrough=4" : "", "-Wno-noexcept-type" : "", 
            # "-Wno-unused-but-set-variable" : "", },
            )


        self.allmoduleinfo[4] = ModuleInfo("umc_va_hw", "umc_va_hw.bp", "_studio/shared/umc/CMakeFiles/umc_va_hw.dir/",
            "library_static", "vpl-runtime-defaults", 
            # updateflags = {"-Wclobbered" : "", "-Werror" : "", "-mindirect-branch-register" : "", "-Wimplicit-fallthrough=4" : "", "-Wno-noexcept-type" : "", 
            # "-Wno-unused-but-set-variable" : "", },
            )

        self.allmoduleinfo[5] = ModuleInfo("libumc_codecs", "libumc_codecs.bp", "_studio/shared/umc/CMakeFiles/bitrate_control.dir/",
            "library_static", "vpl-runtime-defaults", 
            # updateflags = {"-Wclobbered" : "", "-Werror" : "", "-mindirect-branch-register" : "", "-Wimplicit-fallthrough=4" : "", "-Wno-noexcept-type" : "", 
            # "-Wno-unused-but-set-variable" : "", },
            )

        self.allmoduleinfo[6] = ModuleInfo("vpp", "vpp.bp", "_studio/mfx_lib/vpp/CMakeFiles/vpp_hw.dir/",
            "library_static", "vpl-runtime-defaults", 
            # updateflags = {"-Wclobbered" : "", "-Werror" : "", "-mindirect-branch-register" : "", "-Wimplicit-fallthrough=4" : "", "-Wno-noexcept-type" : "", 
            # "-Wno-unused-but-set-variable" : "", },
            )
        
        self.allmoduleinfo[7] = ModuleInfo("mfx_common_hw", "mfx_common_hw.bp", "_studio/mfx_lib/shared/CMakeFiles/mfx_common_hw.dir/",
            "library_static", "vpl-runtime-defaults", 
            # updateflags = {"-Wclobbered" : "", "-Werror" : "", "-mindirect-branch-register" : "", "-Wimplicit-fallthrough=4" : "", "-Wno-noexcept-type" : "", 
            # "-Wno-unused-but-set-variable" : "", },
            )

        self.allmoduleinfo[8] = ModuleInfo("mfx_ext", "mfx_ext.bp", "_studio/mfx_lib/ext/CMakeFiles/mfx_ext.dir/",
            "library_static", "vpl-runtime-defaults", 
            # updateflags = {"-Wclobbered" : "", "-Werror" : "", "-mindirect-branch-register" : "", "-Wimplicit-fallthrough=4" : "", "-Wno-noexcept-type" : "", 
            # "-Wno-unused-but-set-variable" : "", },
            )

        self.allmoduleinfo[9] = ModuleInfo("encode_hw", "encode_hw.bp", "_studio/mfx_lib/encode_hw/CMakeFiles/encode_hw.dir/",
            "library_static", "vpl-runtime-defaults", 
            # updateflags = {"-Wclobbered" : "", "-Werror" : "", "-mindirect-branch-register" : "", "-Wimplicit-fallthrough=4" : "", "-Wno-noexcept-type" : "", 
            # "-Wno-unused-but-set-variable" : "", },
            )

        self.allmoduleinfo[10] = ModuleInfo("decode_hw", "decode_hw.bp", "_studio/mfx_lib/decode/CMakeFiles/decode_hw.dir/",
            "library_static", "vpl-runtime-defaults", 
            # updateflags = {"-Wclobbered" : "", "-Werror" : "", "-mindirect-branch-register" : "", "-Wimplicit-fallthrough=4" : "", "-Wno-noexcept-type" : "", 
            # "-Wno-unused-but-set-variable" : "", },
            )
        

        self.allmoduledefaults = CCDefaults(self.proj, "vpl-runtime-defaults",
            cflags = ["-Wno-non-virtual-dtor","-fexceptions","-msse4.1","-fstack-protector","-fPIE","-fPIC","-pie","-O2","-D_FORTIFY_SOURCE=2","-Wformat","-Wformat-security",
            "-fexceptions","-frtti","-Wno-unused-command-line-argument","-mavx2","-fexceptions","-Wall","-Werror","-Wextra","-Wno-unused-parameter","-Wno-macro-redefined","-Wno-unused-parameter"],
            cppflags = ["-Wno-error", "-fexceptions", "-msse4.2", "-mavx2", "-D__AVX2__", "-mretpoline", 
            "-Wshorten-64-to-32", "-Wno-extern-c-compat", "-Wno-instantiation-after-specialization", "-DSANITIZER_BUILD", "-Wno-deprecated-register", "-Wno-deprecated-copy",
            "-Wno-deprecated-declarations","-Wno-missing-field-initializers","-Wno-implicit-fallthrough","-DMFX_ONEVPL","-DMFX_VA","-DMFX_VERSION_USE_LATEST","-DMFX_DEPRECATED_OFF",
            "-DONEVPL_EXPERIMENTAL","-DSYNCHRONIZATION_BY_VA_SYNC_SURFACE","-D_FILE_OFFSET_BITS=64","-D__USE_LARGEFILE64","-DMFX_GIT_COMMIT=c6573984","-DMFX_API_VERSION=2.10","-DNDEBUG",
            "-DMSDK_BUILD=","-Wno-unused-command-line-argument","-frtti"],
            include_dirs = ["hardware/intel/external/libva",
                            "vendor/intel/external/onevpl-intel-gpu/android/include/"],
            shared_libs = ["libgmm_umd","libva","libcutils","libdrm"],
            bpfiles = [
                "mfx_logging.bp",
                "mfx_trace.bp",
                "asc.bp",
                "umc_va_hw.bp",
                "libumc_codecs.bp",
                "vpp.bp",
                "mfx_common_hw.bp",
                "mfx_ext.bp",
                "encode_hw.bp",
                "decode_hw.bp",
            ], )

    def getTemplate(self):
        return "onevpl-runtime.tpl"

    def adjustSources(self, mode, all_sources):
        for i, l in enumerate(all_sources):
            all_sources[i] = INDENT * 2 + "\"" + re.sub(r".*?: " + self.allmoduleinfo[mode].Mid_Dir, "",
                re.sub("CMakeFiles/.*?\\.dir/", "", l.replace("__/", "../")))

    def adjustFlags(self, mode, all_flags, is_add = True):
        update_flags = self.allmoduleinfo[mode].Update_Flags
        add_flags = self.allmoduleinfo[mode].Add_Flags

        for i, f in enumerate(update_flags):
            all_flags = re.sub(INDENT * 2 + "\"" + f + "\",\n", update_flags[f], all_flags)

        if is_add:
            all_flags += "\n"

            for i, f in enumerate(add_flags):
                all_flags += INDENT * 2 + "\"" + f + "\",\n"

        return all_flags

    def adjustFiles(self):
        print("It is adjusting some files for vpl-runtime ... ")
        build_dir = self.getBuildDir()
        cmd = "mkdir -p " + path.join(self.proj, "device/include/") + NOVERBOSE
        cmd += "mkdir -p " + path.join(self.proj, "device/built_ins/x64/gen12lp/") + NOVERBOSE
        cmd += "mkdir -p " + path.join(self.proj, "device/built_ins/x64/xe_hpc_core/") + NOVERBOSE
        cmd += "mkdir -p " + path.join(self.proj, "device/built_ins/x64/xe_hpg_core/") + NOVERBOSE
        cmd += "cp -f " + path.join(build_dir, "*.h") + " " + path.join(self.proj, "device/include/") + NOVERBOSE
        cmd += "cp -f " + path.join(build_dir, "bin/built_ins/x64/gen12lp/*.cpp") + " " + path.join(self.proj, "device/built_ins/x64/gen12lp/") + NOVERBOSE
        cmd += "cp -f " + path.join(build_dir, "bin/built_ins/x64/xe_hpc_core/*.cpp") + " " + path.join(self.proj, "device/built_ins/x64/xe_hpc_core/") + NOVERBOSE
        cmd += "cp -f " + path.join(build_dir, "bin/built_ins/x64/xe_hpg_core/*.cpp") + " " + path.join(self.proj, "device/built_ins/x64/xe_hpg_core/") + NOVERBOSE
        os.system(cmd)

class Main:

    def run(self):
        script = path.dirname(__file__)
        print(path)
        root = path.abspath(path.join(script, "../.."))

        print(("script = " + script))
        print(("root = " + root))

        VplRuntimeGenerator(root).generate(to_make = False)


if RUN_CALLABLE:
    m = Main()
    m.run()
