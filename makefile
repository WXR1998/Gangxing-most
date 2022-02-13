obj_run = run.o threads.o utils.o
obj_run_decomp = run_decomp.o threads.o utils.o
options = -w -std=c++11 -O4 -lpthread
CXX = g++

all: run run_decomp

run: $(obj_run)
	$(CXX) $(obj_run) -o run $(options)

run_decomp: $(obj_run_decomp)
	$(CXX) $(obj_run_decomp) -o run_decomp $(options)

%.o: %.cpp
	$(CXX) -c $< -o $@ $(options)

.PHONY: clean
clean:
	rm -rf $(obj_run) $(obj_run_decomp) run run_decomp
