Name: Stephen Porter
Clss: CS 453
Assn: p3
Updt: 10/30/2014

============
Build / Run
============
To Build:
	make
	
To Run:
	./pc <poolsize> <#items/producer> <#producers> <#consumers> <sleep interval(microsecs)>

To Clean:
	make clean
	
======
Files
======
include
	|_
	  contains all header files after make
lib
	|_
	  contains all library files after make
libsrc
	|_
	  common.h
	  List.c
	  List.h
	  Makefile
	  Node.c
	  Node.h
Item.c
Item.h
Makefile
pc.c
README.txt
test.sh


===========
Discussion
===========
This project was pretty easy. The only issue I ran into was with the conditions. I was using a single condition for both producers and consumers, which lead the broadcast in the finishUp() method to hang. I fixed that by creating a separate conditional for producers and consumers.

I also took out unnecessary code in my list. I know that in general I wouldn't do that, I just did it because it wasn't necessary for this project (e.g. printList or getSize). It just made the code look cleaner and we only see the important stuff for this project.

Also, thanks Amit for the extra day in light of Halloween. It was nice to not have to worry about this until I had a day with nothing going on rather than trying t do it throughout the week when my schedule is jam packed full of stuff to do.