$(eval $(call start_directory,sys))
$(eval $(call std_header))
$(eval $(call add_to_sys_include_path))

SUB_OBJS := 

KOBJS := $(call mkobjs,$(SUB_OBJS))
SUBDIRS := boot mm fs driver util task ipc device net os arch
$(eval $(call include_subdirs))

OBJS += $(KOBJS)
KERNEL +=$(call mkobjs,kernel)


KERNELFLAGS := -T src/sys/kernel.ld
$(do)kernel: $(KOBJS)
	@echo "linking kernel" $^
	@$(LD) $(LDFLAGS) $(KERNELFLAGS) -o $@ $^

$(eval $(call std_footer))
