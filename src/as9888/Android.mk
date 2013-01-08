ifneq ($(TARGET_SIMULATOR),true)

LOCAL_PATH:= $(call my-dir)

# dmtd
DMT_LIB=libAS9888

##### DMT daemon ###############################################################
include $(CLEAR_VARS)

LOCAL_C_INCLUDES := \
	$(KERNEL_HEADERS) \
	$(LOCAL_PATH)/$(DMT_LIB)/include \
	$(LOCAL_PATH)/libFST_AS9888

LOCAL_SRC_FILES:= \
	AS9888Driver.c \
	DispMessage.c \
	FileIO.c \
	Measure.c \
	main.c

LOCAL_CFLAGS += \
	-Wall \
	-DENABLE_DMTDEBUG=1 \
	-DOUTPUT_STDOUT=1 \
	-DDBG_LEVEL=2 \

LOCAL_LDFLAGS := $(LOCAL_PATH)/libAS9888.a
LOCAL_MODULE := dmtd
LOCAL_MODULE_TAGS := eng
LOCAL_FORCE_STATIC_EXECUTABLE := false
#LOCAL_STATIC_LIBRARIES := libAS9888
LOCAL_SHARED_LIBRARIES := libc libm libutils libcutils
include $(BUILD_EXECUTABLE)


endif  # TARGET_SIMULATOR != true

