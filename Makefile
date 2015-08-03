CXX = g++
CXXFLAGS = -O2 -Wall -pedantic -Wextra -Weffc++ -lboost_regex
#CXXFLAGS = -O2 -Wall -pedantic -Wextra -Weffc++

#CXX = icpc
#CXXFLAGS = -O3 -lboost_regex


.PHONY: testall
testall: test01 test02

test01: parser.exe HCuNbad.basis
	./parser.exe HCuNbad.basis > $@.log 2> $@.err

test02: parser.exe HCuNbad.basis
	./parser.exe HCuNbad2.basis > $@.log 2> $@.err

parser.exe: main.cpp SingleBasisSet.hpp
	$(CXX) $(CXXFLAGS) main.cpp -o $@

.PHONY: clean
clean:
	rm -rf test*.log parser.exe test*.err



