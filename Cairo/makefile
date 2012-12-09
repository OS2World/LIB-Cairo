#
# Makefile for Cairo to be compiled to OS/2 with OpenWatcom and GCC
#

# Version section:
# Define the version numbers reported by the library!

CAIRO_VERSION_MAJOR = 1
CAIRO_VERSION_MINOR = 6
CAIRO_VERSION_MICRO = 4
FC_CACHE_VERSION_STRING="\"v1.1_for_Cairo\""

# DLL Base name, usually 'cairo', or shortened if needed
CAIRO_DLL_BASENAME = cairo

# DLL name to have as base of compatibility
COMPATIBILITY_ATTIC_DLL_NAME = cairo146.dll

#
# Configuration section:
# Define to 1 the pieces you want to include in Cairo!
#

CAIRO_INCLUDE_PS_SURFACE = 1
CAIRO_INCLUDE_PDF_SURFACE = 1
CAIRO_INCLUDE_SVG_SURFACE = 1
CAIRO_INCLUDE_OS2_SURFACE = 1
CAIRO_INCLUDE_FT_FONT = 1
CAIRO_INCLUDE_PNG_FUNCTIONS = 1

#
# Directory section:
# Tell the system the relative path to the different packages
# used to build cairo!
#

CAIRO_DIR_BASE = cairo-1.6.4
CAIRO_DIR_LIBPIXMAN = pixman-0.10.0
CAIRO_DIR_FONTCONFIG = fontconfig-os2
CAIRO_DIR_FREETYPE = freetype-2.1.10
CAIRO_DIR_LIBPNG = libpng-1.2.8-config
CAIRO_DIR_ZLIB = zlib-1.2.3

#
# ----------------------------------------------------------------------------
#
#
# From here everything should be automatic, and no modification should be 
# required.
#
#

CAIRO_EXTRA_DEFINES = -DHAVE_CONFIG_H -DBUILD_CAIRO_DLL -DUSE_MMX -DFC_CACHE_VERSION_STRING=$(FC_CACHE_VERSION_STRING)

!ifeq CAIRO_INCLUDE_PS_SURFACE 1
CAIRO_PS_OBJS = cairo-ps-surface.obj
CAIRO_FONT_SUBSET_OBJS =  &
 cairo-scaled-font-subsets.obj &
 cairo-scaled-font.obj &
 cairo-font-face.obj &
 cairo-font-options.obj &
 cairo-truetype-subset.obj &
 cairo-type1-fallback.obj &
 cairo-type1-subset.obj
CAIRO_EXTRA_DEFINES += -DCAIRO_HAS_PS_SURFACE=1
!endif

!ifeq CAIRO_INCLUDE_PDF_SURFACE 1
CAIRO_PDF_OBJS = &
	cairo-pdf-surface.obj &
	cairo-pdf-operators.obj
CAIRO_FONT_SUBSET_OBJS =  &
 cairo-scaled-font-subsets.obj &
 cairo-scaled-font.obj &
 cairo-font-face.obj &
 cairo-font-options.obj &
 cairo-truetype-subset.obj &
 cairo-type1-fallback.obj &
 cairo-type1-subset.obj
CAIRO_EXTRA_DEFINES += -DCAIRO_HAS_PDF_SURFACE=1
!endif

!ifeq CAIRO_INCLUDE_SVG_SURFACE 1
CAIRO_OS2_OBJS = cairo-svg-surface.obj
CAIRO_FONT_SUBSET_OBJS =  &
 cairo-scaled-font-subsets.obj &
 cairo-scaled-font.obj &
 cairo-font-face.obj &
 cairo-font-options.obj &
 cairo-truetype-subset.obj &
 cairo-type1-fallback.obj &
 cairo-type1-subset.obj
CAIRO_EXTRA_DEFINES += -DCAIRO_HAS_SVG_SURFACE=1
!endif

!ifeq CAIRO_INCLUDE_OS2_SURFACE 1
CAIRO_OS2_OBJS = cairo-os2-surface.obj
CAIRO_FONT_SUBSET_OBJS =  &
 cairo-scaled-font-subsets.obj &
 cairo-scaled-font.obj &
 cairo-font-face.obj &
 cairo-font-options.obj &
 cairo-truetype-subset.obj &
 cairo-type1-fallback.obj &
 cairo-type1-subset.obj
CAIRO_EXTRA_DEFINES += -DCAIRO_HAS_OS2_SURFACE=1
!endif

!ifeq CAIRO_INCLUDE_FT_FONT 1
CAIRO_FT_OBJS = cairo-ft-font.obj &
 autofit.obj &
 ftapi.obj &
 ftbase.obj &
 ftbbox.obj &
 ftbdf.obj &
 ftbitmap.obj &
 ftdebug.obj &
 ftglyph.obj &
 ftinit.obj &
 ftmm.obj &
 ftotval.obj &
 ftpfr.obj &
 ftstroke.obj &
 ftsynth.obj &
 ftsystem.obj &
 fttype1.obj &
 ftwinfnt.obj &
 ftxf86.obj &
 bdf.obj &
 ftcache.obj &
 cff.obj &
 type1cid.obj &
 ftgzip.obj &
 ftlzw.obj &
 otvalid.obj &
 pcf.obj &
 pfr.obj &
 psaux.obj &
 pshinter.obj &
 psnames.obj &
 raster.obj &
 sfnt.obj &
 smooth.obj &
 truetype.obj &
 type1.obj &
 type42.obj &
 winfnt.obj
CAIRO_EXTRA_DEFINES += -DCAIRO_HAS_FT_FONT=1
!endif

!ifeq CAIRO_INCLUDE_PNG_FUNCTIONS 1
CAIRO_PNG_OBJS = cairo-png.obj
CAIRO_EXTRA_DEFINES += -DCAIRO_HAS_PNG_FUNCTIONS=1
CAIRO_LIBPNG_OBJS = &
 png.obj &
 pngerror.obj &
 pnggccrd.obj &
 pngget.obj &
 pngmem.obj &
 pngpread.obj &
 pngread.obj &
 pngrio.obj &
 pngrtran.obj &
 pngrutil.obj &
 pngset.obj &
 pngtrans.obj &
 pngvcrd.obj &
 pngwio.obj &
 pngwrite.obj &
 pngwtran.obj &
 pngwutil.obj

CAIRO_ZLIB_OBJS = &
 adler32.obj &
 compress.obj &
 crc32.obj &
 deflate.obj &
 gzio.obj &
 infback.obj &
 inffast.obj &
 inflate.obj &
 inftrees.obj &
 trees.obj &
 uncompr.obj &
 zutil.obj
!endif

CAIRO_LIBPIXMAN_OBJS = &
	pixman-access-accessors.obj &
	pixman-access.obj &
	pixman-combine.obj &
	pixman-compose-accessors.obj &
	pixman-compose.obj &
	pixman-compute-region.obj &
	pixman-edge-accessors.obj &
	pixman-edge.obj &
	pixman-image.obj &
	pixman-mmx.obj &
	pixman-pict.obj &
	pixman-region.obj &
	pixman-source.obj &
	pixman-sse.obj &
	pixman-timer.obj &
	pixman-transformed-accessors.obj &
	pixman-transformed.obj &
	pixman-trap.obj &
	pixman-utils.obj

CAIRO_FONTCONFIG_OBJS = fontconfig.obj

CAIRO_ALL_OBJS = &
 cairo.obj &
 cairo-analysis-surface.obj &
 cairo-arc.obj &
 cairo-array.obj &
 cairo-atomic.obj &
 cairo-base85-stream.obj &
 cairo-bentley-ottmann.obj &
 cairo-cache.obj	&
 cairo-cff-subset.obj &
 cairo-clip.obj &
 cairo-color.obj	&
 cairo-debug.obj &
 cairo-deflate-stream.obj &
 cairo-fixed.obj	&
 cairo-freelist.obj &
 cairo-gstate.obj &
 cairo-hash.obj &
 cairo-hull.obj &
 cairo-image-surface.obj &
 cairo-lzw.obj &
 cairo-matrix.obj &
 cairo-meta-surface.obj &
 cairo-misc.obj &
 cairo-mutex.obj &
 cairo-output-stream.obj &
 cairo-paginated-surface.obj &
 cairo-path.obj	&
 cairo-path-bounds.obj	&
 cairo-path-fill.obj	&
 cairo-path-fixed.obj	&
 cairo-path-stroke.obj	&
 cairo-pattern.obj &
 cairo-pen.obj	&
 cairo-polygon.obj	&
 cairo-rectangle.obj	&
 cairo-region.obj	&
 cairo-skiplist.obj	&
 cairo-slope.obj	&
 cairo-spline.obj	&
 cairo-stroke-style.obj &
 cairo-surface.obj	&
 cairo-surface-fallback.obj &
 cairo-traps.obj	&
 cairo-unicode.obj	&
 cairo-wideint.obj	&
 $(CAIRO_LIBPIXMAN_OBJS)	&
 $(CAIRO_FONTCONFIG_OBJS)	&
 $(CAIRO_ZLIB_OBJS)	&
 $(CAIRO_LIBPNG_OBJS)	&
 $(CAIRO_FT_OBJS)	&
 $(CAIRO_PS_OBJS)	&
 $(CAIRO_PDF_OBJS)	&
 $(CAIRO_FONT_SUBSET_OBJS) &
 $(CAIRO_PNG_OBJS)	&
 $(CAIRO_OS2_OBJS)

CFLAGS = -Zomf -Zmt -O2 -march=pentium -mmmx $(CAIRO_EXTRA_DEFINES)

.before:
 @set C_INCLUDE_PATH=.\;$(CAIRO_DIR_BASE)\src;&
$(CAIRO_DIR_LIBPIXMAN)\pixman;&
$(CAIRO_DIR_FONTCONFIG);$(CAIRO_DIR_FONTCONFIG)\src;&
$(CAIRO_DIR_ZLIB);&
$(CAIRO_DIR_LIBPNG);&
$(CAIRO_DIR_FREETYPE)\include;&
$(%PATH_EMXPGCC)\include;

.extensions:
.extensions: .obj .c .h

all: $(CAIRO_DLL_BASENAME)$(CAIRO_VERSION_MAJOR)$(CAIRO_VERSION_MINOR)$(CAIRO_VERSION_MICRO).dll forwarders .symbolic

$(CAIRO_DLL_BASENAME)$(CAIRO_VERSION_MAJOR)$(CAIRO_VERSION_MINOR)$(CAIRO_VERSION_MICRO).dll: .\cairo-features.h cairo.def os2_patch cairo_print_building_objects $(CAIRO_ALL_OBJS) cairo-objs.txt
 @echo Linking DLL...
 gcc -Zdll -Zomf -Zmt -Zmap cairo.def @cairo-objs.txt -o $(CAIRO_DLL_BASENAME)$(CAIRO_VERSION_MAJOR)$(CAIRO_VERSION_MINOR)$(CAIRO_VERSION_MICRO).dll
 @echo Calling IMPLIB.EXE to create .lib from .dll...
 @implib $(CAIRO_DLL_BASENAME)$(CAIRO_VERSION_MAJOR)$(CAIRO_VERSION_MINOR)$(CAIRO_VERSION_MICRO).lib $(CAIRO_DLL_BASENAME)$(CAIRO_VERSION_MAJOR)$(CAIRO_VERSION_MINOR)$(CAIRO_VERSION_MICRO).dll
 
.\cairo-features.h: makefile
 @echo Creating 'cairo-features.h' file based on settings of 'makefile'!
 @echo /* This file was automatically generated by the Watcom Makefile */ >$^@
 @echo /* based on the settings set inside the makefile!               */ >>$^@
 @echo $#ifndef CAIRO_FEATURES_H >>$^@
 @echo $#define CAIRO_FEATURES_H >>$^@
 @echo $#ifdef  __cplusplus >>$^@
 @echo $# define CAIRO_BEGIN_DECLS  extern "C" { >>$^@
 @echo $# define CAIRO_END_DECLS    } >>$^@
 @echo $#else >>$^@
 @echo $# define CAIRO_BEGIN_DECLS >>$^@
 @echo $# define CAIRO_END_DECLS >>$^@
 @echo $#endif >>$^@
 @echo $#ifdef __OS2__ >>$^@
 @echo $# ifdef BUILD_CAIRO_DLL >>$^@
 @echo $#  define cairo_public __declspec(dllexport) >>$^@
 @echo $# else >>$^@
 @echo $#  define cairo_public >>$^@
 @echo $# endif >>$^@
 @echo $# define cairo_call _System >>$^@
 @echo $#else >>$^@
 @echo $# define cairo_public >>$^@
 @echo $# define cairo_call >>$^@
 @echo $#endif >>$^@
 @echo $#define CAIRO_VERSION_MAJOR $(CAIRO_VERSION_MAJOR) >>$^@
 @echo $#define CAIRO_VERSION_MINOR $(CAIRO_VERSION_MINOR) >>$^@
 @echo $#define CAIRO_VERSION_MICRO $(CAIRO_VERSION_MICRO) >>$^@
 @echo $#define CAIRO_VERSION_STRING "$(CAIRO_VERSION_MAJOR).$(CAIRO_VERSION_MINOR).$(CAIRO_VERSION_MICRO)" >>$^@
 @echo $#define PACKAGE_VERSION "$(CAIRO_VERSION_MAJOR).$(CAIRO_VERSION_MINOR).$(CAIRO_VERSION_MICRO)" >>$^@
!ifeq CAIRO_INCLUDE_PS_SURFACE 1
 @echo $#define CAIRO_HAS_PS_SURFACE 1 >>$^@
!endif
!ifeq CAIRO_INCLUDE_PDF_SURFACE 1
 @echo $#define CAIRO_HAS_PDF_SURFACE 1 >>$^@
!endif
!ifeq CAIRO_INCLUDE_SVG_SURFACE 1
 @echo $#define CAIRO_HAS_SVG_SURFACE 1 >>$^@
!endif
!ifeq CAIRO_INCLUDE_OS2_SURFACE 1
 @echo $#define CAIRO_HAS_OS2_SURFACE 1 >>$^@
!endif
!ifeq CAIRO_INCLUDE_FT_FONT 1
 @echo $#define CAIRO_HAS_FT_FONT 1 >>$^@
!endif
!ifeq CAIRO_INCLUDE_PNG_FUNCTIONS 1
 @echo $#define CAIRO_HAS_PNG_FUNCTIONS 1 >>$^@
!endif
 @echo $#endif >>$^@
 @copy cairo-features.h $(CAIRO_DIR_BASE)\src\cairo-features.h
 @echo The features file has been created, and was also copied into Cairo folder.

cairo_print_building_objects: .symbolic
 @echo Compiling sources, please wait...

os2_patch: os2_patch.exe .SYMBOLIC
 @echo Patching source code...
 @os2_patch.exe $(CAIRO_DIR_BASE) $(CAIRO_DIR_FREETYPE)

os2_patch.exe: os2_patch.c
 @gcc -o os2_patch.exe os2_patch.c

.c : $(CAIRO_DIR_BASE)\src;$(CAIRO_DIR_ZLIB);$(CAIRO_DIR_LIBPNG);&
$(CAIRO_DIR_FONTCONFIG)\src;&
$(CAIRO_DIR_LIBPIXMAN)\pixman;&
!ifeq CAIRO_INCLUDE_FT_FONT 1
$(CAIRO_DIR_FREETYPE)\src\autofit;&
$(CAIRO_DIR_FREETYPE)\src\autohint;&
$(CAIRO_DIR_FREETYPE)\src\base;&
$(CAIRO_DIR_FREETYPE)\src\bdf;&
$(CAIRO_DIR_FREETYPE)\src\cache;&
$(CAIRO_DIR_FREETYPE)\src\cff;&
$(CAIRO_DIR_FREETYPE)\src\cid;&
$(CAIRO_DIR_FREETYPE)\src\gzip;&
$(CAIRO_DIR_FREETYPE)\src\lzw;&
$(CAIRO_DIR_FREETYPE)\src\otvalid;&
$(CAIRO_DIR_FREETYPE)\src\pcf;&
$(CAIRO_DIR_FREETYPE)\src\pfr;&
$(CAIRO_DIR_FREETYPE)\src\psaux;&
$(CAIRO_DIR_FREETYPE)\src\pshinter;&
$(CAIRO_DIR_FREETYPE)\src\psnames;&
$(CAIRO_DIR_FREETYPE)\src\raster;&
$(CAIRO_DIR_FREETYPE)\src\sfnt;&
$(CAIRO_DIR_FREETYPE)\src\smooth;&
$(CAIRO_DIR_FREETYPE)\src\truetype;&
$(CAIRO_DIR_FREETYPE)\src\type1;&
$(CAIRO_DIR_FREETYPE)\src\type42;&
$(CAIRO_DIR_FREETYPE)\src\winfonts;
!endif

.c.obj :
	@gcc -c $(cflags) $[*.c -o $*.obj

cairo.def: makefile
 @echo Creating module definition file...
 @echo LIBRARY $(CAIRO_DLL_BASENAME)$(CAIRO_VERSION_MAJOR)$(CAIRO_VERSION_MINOR)$(CAIRO_VERSION_MICRO) INITINSTANCE TERMINSTANCE >$^@
 @echo DESCRIPTION 'Cairo v$(CAIRO_VERSION_MAJOR).$(CAIRO_VERSION_MINOR).$(CAIRO_VERSION_MICRO) - A Vector Graphics Library' >>$^@
 @echo OLD 'dll_attic\$(COMPATIBILITY_ATTIC_DLL_NAME)' >>$^@
 @echo Def. file created!
 
cairo-objs.txt: makefile
 @if exist $^@ @del $^@
 @for %i in ($(CAIRO_ALL_OBJS)) do @echo %i >>$^@

forwarders: .symbolic
 @echo Creating cairo124.dll forwarder dll...
 @gcc -Zomf -Zdll -o cairo124 cairo124.def -s >nul
 @echo Creating cairo146.dll forwarder dll...
 @gcc -Zomf -Zdll -o cairo146 cairo146.def -s >nul
 @echo Creating cair1414.dll forwarder dll...
 @gcc -Zomf -Zdll -o cair1414 cairo1414.def -s >nul

clean: .symbolic
        @if exist *.obj @del *.obj
        @if exist cairo.def @del cairo.def
        @if exist cairo-objs.txt @del cairo-objs.txt
        @if exist *.map @del *.map
        @if exist *.err @del *.err
