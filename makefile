obj_run = run.o threads.o utils.o int128.o
obj_run_decomp = run_decomp.o threads.o utils.o int128.o
obj_test = test.o utils.o int128.o
options = -w -std=c++11 -O4 -lpthread
CXX = g++

all: run run_decomp test

run: $(obj_run)
	$(CXX) $(obj_run) -o run $(options)

run_decomp: $(obj_run_decomp)
	$(CXX) $(obj_run_decomp) -o run_decomp $(options)

test: $(obj_test)
	$(CXX) $(obj_test) -o test $(options)

%.o: %.cpp
	$(CXX) -c $< -o $@ $(options)

.PHONY: clean
clean:
	rm -rf $(obj_run) $(obj_run_decomp) $(obj_test) run run_decomp test
