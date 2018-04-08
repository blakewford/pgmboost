adaboost: main.cpp
	g++ -Ofast -m64 -funroll-loops $< -o $@

clean:
	-@rm adaboost
