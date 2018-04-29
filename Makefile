adaboost: main.cpp
	g++ -DLR -Ofast -m64 -funroll-loops $< -o $@
	g++ -DUD -Ofast -m64 -funroll-loops $< -o $@1
	g++ -DLR3 -Ofast -m64 -funroll-loops $< -o $@2
	g++ -DUD3 -Ofast -m64 -funroll-loops $< -o $@3
	g++ -DCHECK -Ofast -m64 -funroll-loops $< -o $@4

clean:
	-@rm adaboost
	-@rm adaboost1
	-@rm adaboost2
	-@rm adaboost3
	-@rm adaboost4
