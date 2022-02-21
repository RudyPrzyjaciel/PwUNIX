# PwUNIX
Programowanie w systemie UNIX - Part of university assignment using C

Assignment was about multiprocess interactions. 

Producers and Customers are using same shared memory. Producers create set number of random goods. Customers consume set number of random products. 

"Zadanie1.c" does not restrict acces to shared memory, therefore race occurs and some actins are not recorded.

"Zadanie2.c" implements mutex for shared memory eliminating race of subprocesses.
