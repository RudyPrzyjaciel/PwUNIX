# PwUNIX

Assignment about multiprocess interactions

Programowanie w systemie UNIX - Part of university assignment using C

Producers and Customers are using same shared memory. Producers create set number of random goods. Customers consume set number of random products

"Zadanie1.c" does not restrict acces to shared memory, therefore race occurs and some actions are wrong

"Zadanie2.c" implements mutex for shared memory eliminating race of subprocesses

"Revision3.cpp" was added not as a part of assignment, but as a further self study and uses C++11 <mutex> and <thread> standard libraries
