LIBUTF = libutf16.a

CC = gcc
#CC = clang

CFLAGS = -g -O2 -I. -Wall -pedantic -Wextra -DNDEBUG
#CFLAGS = -g -O2 -I. -Weverything -DNDEBUG

CCFLAGS = $(CFLAGS) -c -o

AR = ar

EMPTY =

ARFLAGS = -crs $(EMPTY)

TEST = test

TEST_RUN = ./$(TEST)

TEST_FLAGS =
#TEST_FLAGS = -DCHECK_UTF8_LOCALE -DLIBC_GLIBC
#TEST_FLAGS = -DCHECK_UTF8_LOCALE -DLIBC_FBSD

TLINK = $(CFLAGS) $(TEST_FLAGS) -o $(TEST)

CLEAN = rm -f src/*.o $(LIBUTF) $(TEST)

include common.mk
