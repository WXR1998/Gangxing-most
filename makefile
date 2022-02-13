obj = run.o threads.o utils.o
target = run
CXX = g++

$(target): $(obj)
	$(CXX) $(obj) -o $(target) -w -std=c++11 -O2

%.o: %.cpp
	$(CXX) -c $< -o $@ -w -std=c++11 -O2

.PHONY: clean
clean:
	rm -rf $(obj) $(target) 
