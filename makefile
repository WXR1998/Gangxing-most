obj_run = run.o threads.o utils.o
obj_run_decomp = run_decomp.o threads.o utils.o
CXX = g++

all: run run_decomp

run: $(obj_run)
	$(CXX) $(obj_run) -o run -w -std=c++11 -O2

run_decomp: $(obj_run_decomp)
	$(CXX) $(obj_run_decomp) -o run_decomp -w -std=c++11 -O2

%.o: %.cpp
	$(CXX) -c $< -o $@ -w -std=c++11 -O2

.PHONY: clean
clean:
	rm -rf $(obj_run) $(obj_run_decomp) run run_decomp
