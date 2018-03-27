adaboost: main.cpp
	g++ -g $< -o $@

clean:
	-@rm adaboost
