MFX_HOME:= $(call my-dir)

# Recursively call sub-folder Android.mk
include $(call all-subdir-makefiles)
