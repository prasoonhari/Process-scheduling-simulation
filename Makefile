prog: Process.cpp Des.cpp Event.cpp scheduler.cpp
	g++ -std=c++11 -g *.cpp -I. -o sched
clean:
	rm -f sched *~ *.o

