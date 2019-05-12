TESTS = generic_test standard_test

CXXFLAGS = -g -Wno-deprecated -std=c++17

CXXFLAGS := $(CXXFLAGS) -I ../
CXXFLAGS := $(CXXFLAGS) -I ../include

ifdef BOOST_ROOT
	CXXFLAGS := $(CXXFLAGS) -I $(BOOST_ROOT)/include
	LDFLAGS := $(LDFLAGS) -L $(BOOST_ROOT)/lib
endif

# Boost
LDFLAGS := $(LDFLAGS) -l boost_graph
LDFLAGS := $(LDFLAGS) -l boost_program_options
LDFLAGS := $(LDFLAGS) -l boost_system
LDFLAGS := $(LDFLAGS) -l boost_test_exec_monitor
LDFLAGS := $(LDFLAGS) -l boost_unit_test_framework

.PHONY: clean depend run

all: $(TESTS)

otf: otf.o
	g++ $(CXXFLAGS) $^ $(LDFLAGS) -o $@

generic_test: ../gd.o ../label.o ../shared.o generic_test.o
	g++ $(CXXFLAGS) $^ $(LDFLAGS) -o $@

standard_test: standard_test.o
	g++ $(CXXFLAGS) $^ $(LDFLAGS) -o $@

# run the tests
run:
	@for i in $(TESTS); do echo "Running" $$i; ./$$i; done

count:
	wc -l *.hpp *.cc

clean:
	rm -rf *~
	rm -rf *.o
	rm -rf $(TESTS)

depend:
	g++ -MM $(CXXFLAGS) *.cc > dependencies

include dependencies
