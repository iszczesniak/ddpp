TARGETS = ddpp
TARGET_OBJS = $(addsuffix .o, $(TARGETS))

OBJS = bf.o cli_args.o client.o connection.o gd.o label.o routing.o	\
shared.o stats.o utils.o traffic.o

# CXXFLAGS := $(CXXFLAGS) -g
# CXXFLAGS := $(CXXFLAGS) -pg --no-pie
CXXFLAGS := $(CXXFLAGS) -O2
# CXXFLAGS := $(CXXFLAGS) -O2 -D NDEBUG

CXXFLAGS := $(CXXFLAGS) -std=c++17
CXXFLAGS := $(CXXFLAGS) -I include

# Use the C++ linker
LINK.o = $(LINK.cc)

ifdef BOOST_ROOT
	CXXFLAGS := $(CXXFLAGS) -I $(BOOST_ROOT)/include
	LDFLAGS := $(LDFLAGS) -L $(BOOST_ROOT)/lib
endif

LDFLAGS := $(LDFLAGS) -l boost_graph
LDFLAGS := $(LDFLAGS) -l boost_program_options
LDFLAGS := $(LDFLAGS) -l boost_system
LDFLAGS := $(LDFLAGS) -l boost_timer

all: $(TARGETS)

ddpp: $(OBJS)

.PHONY: clean count depend test

clean:
	rm -f *~
	rm -f $(OBJS)
	rm -f $(TARGET_OBJS)
	rm -f $(TARGETS)

count:
	wc -l *.hpp *.cc

depend:
	g++ $(CXXFLAGS) -MM *.cc > dependencies

include dependencies