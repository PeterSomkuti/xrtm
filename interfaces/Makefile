include ../make.inc
include ../common.inc

SUBDIRS =

OBJECTS = xrtm_int_cpp.o         \
          xrtm_int_f77.o         \
          xrtm_int_f77_util.o    \
          xrtm_int_f90.o         \
          xrtm_int_f90_util.o    \
          xrtm_fd_int_cpp.o      \
          xrtm_fd_int_f77.o      \
          xrtm_fd_int_f77_util.o \
          xrtm_fd_int_f90.o      \
          xrtm_fd_int_f90_util.o

BINARIES =
ifdef make_int_idl
BINARIES += xrtm_int_idl.so
endif
ifdef make_int_python
BINARIES += xrtm.so
endif

ifeq ($(shell uname -s),Darwin)
BINARIES += XRTM.dylib
else
BINARIES += XRTM.so
endif

PRODUCTS =

EXTRA_CLEANS = write_int_def

all: libxrtm_interfaces.a ${BINARIES}

libxrtm_interfaces.a: $(OBJECTS)
	ar -rs libxrtm_interfaces.a $(OBJECTS)

xrtm_int_idl.so: xrtm_int_idl.o ../src/libxrtm.a ../misc/libxrtm_misc.a
	$(CFMIX_LD) $(CCFLAGS) -shared -o xrtm_int_idl.so xrtm_int_idl.o ../src/libxrtm.a ../misc/libxrtm_misc.a \
        $(LIBDIRS) $(LINKS) $(CFMIX_LDLINKS)

xrtm.so: xrtm_int_py.o ../src/libxrtm.a ../misc/libxrtm_misc.a
	$(CFMIX_LD) $(CFMIX_LDFLAGS) -shared -o xrtm.so xrtm_int_py.o ../src/libxrtm.a ../misc/libxrtm_misc.a \
        $(LIBDIRS) $(LINKS)

XRTM.so: xrtm_int_jl_util.o ../src/libxrtm.a ../misc/libxrtm_misc.a
	$(CFMIX_LD) $(CFMIX_LDFLAGS) -shared -o XRTM.so xrtm_int_jl_util.o -Wl,--whole-archive ../src/libxrtm.a ../misc/libxrtm_misc.a -Wl,--no-whole-archive \
        $(LIBDIRS) $(LINKS)

XRTM.dylib: xrtm_int_jl_util.o ../src/libxrtm.a ../misc/libxrtm_misc.a
	$(CFMIX_LD) $(CFMIX_LDFLAGS) -shared -o XRTM.dylib xrtm_int_jl_util.o -all_load ../src/libxrtm.a ../misc/libxrtm_misc.a \
        $(LIBDIRS) $(LINKS)

bindx2: write_int_def \
        xrtm_int_cpp.cpp \
        xrtm_int_f77.f xrtm_int_f77_util.c \
        xrtm_int_f90.f90 xrtm_int_f90_util.c \
        xrtm_fd_int_cpp.cpp \
        xrtm_fd_int_f77.f xrtm_fd_int_f77_util.c \
        xrtm_fd_int_f90.f90 xrtm_fd_int_f90_util.c \
        xrtm_int_idl.c \
        XRTM.jl \
        xrtm_int_py.c

write_int_def: write_int_def.o ../src/libxrtm.a ../misc/libxrtm_misc.a
	$(CFMIX_LD) $(CFMIX_LDFLAGS) -o write_int_def write_int_def.o ../src/libxrtm.a ../misc/libxrtm_misc.a \
        $(LIBDIRS) $(LINKS)
	write_int_def

xrtm_int_cpp.cpp: xrtm_int_defs.def xrtm_interface.def
	${HOME}/bindx2/bindx \
        -int_def_in xrtm_int_defs.def \
        -int_def_in xrtm_interface.def \
        -cpp xrtm_int_cpp xrtm_int_cpp.h xrtm_int_cpp.cpp

xrtm_int_f77.f xrtm_int_f77_util.c: xrtm_int_defs.def xrtm_interface.def
	~/bindx2/bindx \
        -int_def_in xrtm_int_defs.def \
        -int_def_in xrtm_interface.def \
        -f77 xrtm_int_f77 xrtm_int_f77_util.c xrtm_int_f77.inc xrtm_int_f77.f

xrtm_int_f90.f90 xrtm_int_f90_util.c: xrtm_int_defs.def xrtm_interface.def
	~/bindx2/bindx \
        -int_def_in xrtm_int_defs.def \
        -int_def_in xrtm_interface.def \
        -f90 xrtm_int_f90 xrtm_int_f90_util.c xrtm_int_f90.f90

xrtm_fd_int_cpp.cpp: xrtm_fd_int_defs.def xrtm_fd_interface.def
	${HOME}/bindx2/bindx \
        -int_def_in xrtm_fd_int_defs.def \
        -int_def_in xrtm_fd_interface.def \
        -cpp xrtm_fd_int_cpp xrtm_fd_int_cpp.h xrtm_fd_int_cpp.cpp

xrtm_fd_int_f77.f xrtm_fd_int_f77_util.c: xrtm_fd_int_defs.def xrtm_fd_interface.def
	~/bindx2/bindx \
        -int_def_in xrtm_fd_int_defs.def \
        -int_def_in xrtm_fd_interface.def \
        -f77 xrtm_fd_int_f77 xrtm_fd_int_f77_util.c xrtm_fd_int_f77.inc xrtm_fd_int_f77.f \

xrtm_fd_int_f90.f90 xrtm_fd_int_f90_util.c: xrtm_fd_int_defs.def xrtm_fd_interface.def
	~/bindx2/bindx \
        -int_def_in xrtm_fd_int_defs.def \
        -int_def_in xrtm_fd_interface.def \
        -f90 xrtm_fd_int_f90 xrtm_fd_int_f90_util.c xrtm_fd_int_f90.f90

xrtm_int_idl.c: xrtm_int_defs.def xrtm_interface.def
	~/bindx2/bindx \
        -int_def_in xrtm_int_defs.def \
        -int_def_in xrtm_interface.def \
        -idl xrtm_int_idl xrtm_int_idl.c xrtm_int_idl.dlm

XRTM.jl: xrtm_int_defs.def xrtm_interface.def
	~/bindx2/bindx \
        -int_def_in xrtm_int_defs.def \
        -int_def_in xrtm_interface.def \
        -jl XRTM xrtm_int_jl_util.c XRTM.jl

xrtm_int_py.c: xrtm_int_defs.def xrtm_interface.def
	~/bindx2/bindx \
        -int_def_in xrtm_int_defs.def \
        -int_def_in xrtm_interface.def \
        -py xrtm_int_py xrtm_int_py.c


include depend.inc
