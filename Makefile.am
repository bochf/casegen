THIRD_PARTY_DIR=$(top_srcdir)/3rd
LD_GTEST=-L$(THIRD_PARTY_DIR)/libs/gtest/.libs -lgtest -lpthread
#LD_SIPSDK=-L$(THIRD_PARTY_DIR)/libs/sipsdk -lsipsdk -lresip -lrutil -lares -lssl -lrt

AM_CXXFLAGS=-I$(THIRD_PARTY_DIR)/include \
						-I$(THIRD_PARTY_DIR)/include/safec \
						-DCM_UNIX -DLINUX

bin_PROGRAMS=casegen pathfinder test_graph test_bitmap
casegen_SOURCES=casegen.cpp graph.cpp traveller.cpp
casegen_LDADD=-L$(THIRD_PARTY_DIR)/libs/utiltp -lutiltp \
							$(THIRD_PARTY_DIR)/libs/safec/safec.a \
							-lpthread -lssl -lrt

pathfinder_SOURCES=pathfinder.cpp

test_graph_SOURCES=test_graph.cpp graph.cpp traveller.cpp
test_graph_LDADD=-L$(THIRD_PARTY_DIR)/libs/utiltp -lutiltp \
								 -L$(THIRD_PARTY_DIR)/libs/gtest/.libs -lgtest -lpthread 

test_bitmap_SOURCES=test_bitmap.cpp
test_bitmap_LDADD=-L$(THIRD_PARTY_DIR)/libs/gtest/.libs -lgtest -lpthread

TESTS_ENVIRONMENT = \
	export GTEST_OUTPUT="xml:output/"; \
	export LD_LIBRARY_PATH=$$LD_LIBRARY_PATH:$(THIRD_PARTY_DIR)/libs/gtest/.libs; \
	test -z output || mkdir -p output;

TESTS = $(bin_PROGRAMS)
