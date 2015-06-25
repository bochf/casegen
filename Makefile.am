AUTOMAKE_OPTIONS=foreign

THIRD_PARTY_DIR=$(top_srcdir)/../dep
SECURITY_DIR=$(top_srcdir)/../../../../security

LD_GTEST=-L$(THIRD_PARTY_DIR)/libs/gtest/.libs -lgtest -lpthread

AM_CXXFLAGS=-I$(THIRD_PARTY_DIR)/include/gtest-1.7.0 \
	-I$(THIRD_PARTY_DIR)/include/safec \
	-fprofile-arcs -ftest-coverage \
	-DCM_UNIX -DLINUX
	
AM_LDFLAGS=-fprofile-arcs

bin_PROGRAMS=casegen test_graph test_bitmap
casegen_SOURCES=casegen.cpp graph.cpp traveller.cpp state_machine.cpp
casegen_LDADD=$(SECURITY_DIR)/libs/linux/Release/safec.a \
								-lpthread -lssl -lrt

test_graph_SOURCES=test_graph.cpp graph.cpp traveller.cpp
test_graph_LDADD=-L$(THIRD_PARTY_DIR)/lib/gtest/.libs -lgtest -lpthread

test_bitmap_SOURCES=test_bitmap.cpp
test_bitmap_LDADD=-L$(THIRD_PARTY_DIR)/lib/gtest/.libs -lgtest -lpthread

TEST_REPORT_DIR=$(top_srcdir)/reports/gtest
COV_REPORT_DIR=$(top_srcdir)/reports/cov
cov-report:
	@test -z $(COV_REPORT_DIR) || mkdir -p $(COV_REPORT_DIR)
	@lcov -c --no-external -d . -o $(COV_REPORT_DIR)/coverage.info --rc lcov_branch_coverage=1 -q
	@genhtml $(COV_REPORT_DIR)/coverage.info --output-directory $(COV_REPORT_DIR) --highlight --legend --num-spaces 2 --branch-coverage -p $(top_srcdir)

cov-clean:
	@lcov --directory . --zerocounters
	@rm -f *.gcno *.gcna
	@rm -rf $(COV_REPORT_DIR)/*

gtest-clean:
	@rm -f $(TEST_REPORT_DIR)/*

clean-local: cov-clean gtest-clean

TESTS_ENVIRONMENT=\
	export GTEST_OUTPUT="xml:$(TEST_REPORT_DIR)/"; \
	export LD_LIBRARY_PATH=$$LD_LIBRARY_PATH:$(THIRD_PARTY_DIR)/libs/gtest/.libs; \
	test -z $(TEST_REPORT_DIR) || mkdir -p $(TEST_REPORT_DIR);

TESTS=$(bin_PROGRAMS)
