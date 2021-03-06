MAINTAINERCLEANFILES = Makefile.in

EDJE_CC = @edje_cc@
EDJE_CC_FLAGS_VERBOSE_0 =
EDJE_CC_FLAGS_VERBOSE_1 = -v
EDJE_CC_FLAGS = $(EDJE_CC_FLAGS_$(V)) -id $(srcdir) -fd $(srcdir)

examplesdir = $(datadir)/$(PACKAGE)/examples

if ENABLE_MULTISENSE
MULTISENSE_BIN_FILE = edje-multisense
MULTISENSE_C_FILE   = edje-multisense.c
MULTISENSE_EDC_FILE = multisense.edc
SND_DIR             = -sd $(srcdir)
endif

#put here all EDCs one needs to the examples
EDCS = \
        basic.edc \
	swallow.edc \
	text.edc \
	table.edc \
	box.edc \
	drag.edc \
	edje_example.edc \
	embryo_custom_state.edc \
	embryo_pong.edc \
	embryo_run_program.edc \
	embryo_set_state.edc \
	embryo_set_text.edc \
	embryo_timer.edc \
	external_elm_anchorblock.edc \
	external_elm_button.edc \
	external_elm_check.edc \
	external_elm_panes.edc \
	external_emotion_elm.edc \
	toggle_using_filter.edc \
	signals-messages.edc \
	color-class.edc \
	perspective.edc \
	animations.edc \
	lua_script.edc \
	sigtest.edc \
	animations2.edc \
	basic2.edc \
	signalsBubble.edc \
	$(MULTISENSE_EDC_FILE)

.edc.edj:
	$(EDJE_CC) $(EDJE_CC_FLAGS) $(SND_DIR) $< $(builddir)/$(@F)


EDJS = $(EDCS:%.edc=%.edj)

filesdir = $(datadir)/$(PACKAGE)/examples
files_DATA = \
        $(EDCS) \
        bubble.png \
	red.png \
	test.png \
	Vera.ttf \
        duck.wav \
	edje_example.c \
	edje-basic.c \
	edje-swallow.c \
	edje-text.c \
	edje-table.c \
	edje-box.c \
	edje-box2.c \
	edje-drag.c \
	edje-signals-messages.c \
	edje-color-class.c \
	edje-perspective.c \
	edje-animations.c \
	sigtest.c \
	animations2.c \
	edje-basic2.c \
	signals2.c \
	edje-swallow2.c \
	$(MULTISENSE_C_FILE)

EXTRA_DIST = $(files_DATA)

if BUILD_EXAMPLES

AM_CPPFLAGS = \
-I. \
-I$(top_srcdir)/src/lib \
-I$(top_srcdir)/src/lib/include \
-DPACKAGE_BIN_DIR=\"$(bindir)\" \
-DPACKAGE_LIB_DIR=\"$(libdir)\" \
-DPACKAGE_DATA_DIR=\"$(pkgdatadir)\" \
@EDJE_CFLAGS@

files_DATA += $(EDJS)

examples_PROGRAMS = \
	edje-animations \
	edje-basic \
	edje-box \
	edje-box2 \
	edje-color-class \
	edje-drag\
	edje-perspective \
	edje-signals-messages \
	edje-swallow \
	edje-table \
	edje-text \
	sigtest \
	animations2 \
	edje-basic2 \
	signals2 \
	edje-swallow2 \
	$(MULTISENSE_BIN_FILE)

LDADD = $(top_builddir)/src/lib/libedje.la @EDJE_LIBS@

endif

clean-local:
	rm -f *.edj
