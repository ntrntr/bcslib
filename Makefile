# The make file for BCSLib

# Detect platform
#--------------------

UNAME := $(shell uname -s)
MACH_TYPE := $(shell uname -m)

ifneq ($(MACH_TYPE), x86_64)
    $(error Only 64-bit platform is supported currently)
endif


# Compiler configuration
#-------------------------

WARNING_FLAGS = -Wall -Wextra -Wconversion -Wformat -Wno-unused-parameter
CPPFLAGS = -I. -isystem $(ARMA_HOME)/include

ifeq ($(UNAME), Linux)
	CXX=g++
	CXXFLAGS = -std=c++0x -pedantic $(WARNING_FLAGS) $(CPPFLAGS)
endif
ifeq ($(UNAME), Darwin)
	CXX=clang++
	CXXFLAGS = -std=c++0x -stdlib=libc++ -pedantic $(WARNING_FLAGS) $(CPPFLAGS)
endif

OFLAGS=-O3 -ffast-math


# Intel MKL configuration

USE_MKL=yes

MKL_INC_PATH = -I$(MKLROOT)/include

ifeq ($(UNAME), Linux)
    MKL_LNK_PATH = -L$(MKLROOT)/../../lib/intel64 -L$(MKLROOT)/lib/intel64
	MKL_LNK = -lmkl_intel_lp64 -lmkl_intel_thread -lmkl_core -liomp5 -lpthread
endif
ifeq ($(UNAME), Darwin)
    MKL_LNK_PATH = -L$(MKLROOT)/../../lib -L$(MKLROOT)/lib
	MKL_LNK = -lmkl_intel_lp64 -lmkl_intel_thread -lmkl_core -liomp5 -lpthread
endif
 
ifeq ($(USE_MKL), yes)
   BLAS_PATHS = $(MKL_INC_PATH) $(MKL_LNK_PATH)
   BLAS_LNKS = $(MKL_LNK)
endif


# Testing setup

MAIN_TEST_PRE=-isystem $(GTEST_HOME)/include test/bcs_test_main.cpp
MAIN_TEST_POST=$(GTEST_HOME)/lib/libgtest.a -lpthread


# directory configuration

INC=bcslib

#------ Output directory ----------

BIN=bin

#------ Header groups ----------

CORE_H = \
	$(INC)/config/user_config.h \
	$(INC)/config/platform_config.h \
	$(INC)/config/config.h \
	$(INC)/core/basic_types.h \
	$(INC)/core/syntax.h \
	$(INC)/core/scalar_math.h \
	$(INC)/core/basic_defs.h \
	$(INC)/core/type_traits.h \
	$(INC)/core/iterator.h \
	$(INC)/core/mem_op.h \
	$(INC)/engine/mem_op_impl.h \
	$(INC)/core/block.h \
	$(INC)/core.h


#---------- Target groups -------------------

.PHONY: all
all: test
# all: test bench

.PHONY: test
test: test_core

.PHONY: clean

clean:
	-rm $(BIN)/*


#------ Core tests ----------

.PHONY: test_basics
test_core: $(BIN)/test_memory



#_________________________________________________________________________
#
#  BELOW ARE DETAILS!
#


#----------------------------------------------------------
#
#   Core test (details)
#
#----------------------------------------------------------


TEST_MEMORY_SOURCES = \
	test/core/test_mem_op.cpp \
	test/core/test_blocks.cpp
	
$(BIN)/test_memory: $(CORE_H) $(TEST_MEMORY_SOURCES)
	$(CXX) $(CXXFLAGS) $(MAIN_TEST_PRE) $(TEST_MEMORY_SOURCES) $(MAIN_TEST_POST) -o $@



