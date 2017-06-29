# contrib/uniqueidentifier/Makefile

MODULE_big = uniqueidentifier
OBJS = uniqueidentifier.o

EXTENSION = uniqueidentifier
DATA = uniqueidentifier--1.0.sql uniqueidentifier--unpackaged--1.0.sql
SHLIB_LINK += -luuid

ifdef USE_PGXS
PG_CONFIG = pg_config
PGXS := $(shell $(PG_CONFIG) --pgxs)
include $(PGXS)
else
subdir = contrib/uniqueidentifier
top_builddir = ../..
include $(top_builddir)/src/Makefile.global
include $(top_srcdir)/contrib/contrib-global.mk
endif
