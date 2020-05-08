#
# Copyright (c) 2020 Fabio Belavenuto
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY#  without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>.
#
# Advanced auto dependency generation used from:
# http://make.mad-scientist.net/papers/advanced-auto-dependency-generation/
#

prefix ?= /usr/local
BINDIR ?= $(prefix)/bin

CROSS_COMPILE ?=

SDIR = src
IDIR = inc
ODIR = obj
DEPDIR = $(ODIR)/.deps

CP = cp
RM = rm -rf
MD = mkdir
CXX = $(CROSS_COMPILE)g++
LINKER = $(CXX)

CXXFLAGS = -std=c++17 -g -I$(IDIR) -fpermissive
DEPFLAGS = -MT $@ -MMD -MP -MF $(DEPDIR)/$*.d
LDFLAGS =
LIBS = -lpthread -lSDL2 -lSDL2main

SRCS = $(shell ls src/ | xargs)
OBJS = $(patsubst %.cpp,$(ODIR)/%.o,$(SRCS))
DEPFILES = $(SRCS:%.cpp=$(DEPDIR)/%.d)

all: $(ODIR) $(DEPDIR) tk2000

tk2000: $(OBJS)
	$(LINKER) $(LDFLAGS) -o $@ $(OBJS) $(LIBS)

pch: $(IDIR)/pch.h
	$(CXX) $(CXXFLAGS) -x c++-header -o $<.gch -c $(SDIR)/pch.cpp

.PHONY: clean

clean:
	$(RM) $(ODIR) tk2000

$(ODIR):
	$(MD) $@

$(DEPDIR):
	$(MD) $@

$(DEPFILES):

include $(wildcard $(DEPFILES))

install: all
	for d in $(BINDIR); do [ -d $$d ] || mkdir -p $$d || exit 1; done
	$(CP) tk2000 $(BINDIR)/.

uninstall:
	$(RM) $(BINDIR)/tk2000

$(ODIR)/%.o: $(SDIR)/%.cpp $(DEPDIR)/%.d | $(DEPDIR)
	$(CXX) $(CXXFLAGS) $(DEPFLAGS) -c -o $@ $<
