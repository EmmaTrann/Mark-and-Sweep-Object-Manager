# Object Manager 

!!! Disclaimer: Code are written by me but the problem question belongs to the University of Manitoba. This repo is only used for the purpose of a code portfolio to support my job hunt process.  

Since we cannot implement Gabage Collector in C, we will try to acheive the same feature using object manager. We’re going to store generic objects in a managed buffer. That is, our container is going to be able to hold any kind of object we want and won’t be limited to just strings. Using a buffer means that we must directly manage the memory and references to objects. This means that our object manager must implement reference counting and a garbage collector so that we properly handle creation and deletion of objects. The object manager’s interface is given by ObjectManager.h and the implementation you write will be in a file named ObjectManager.c. Your task is to implement the functions required for the object manager. This includes all the functions listed in ObjectManager.h, along with any (private) static functions that will be useful. You will also need to define appropriate data types and data structures for keeping track of the objects that have been allocated. You are NOT allowed to change ObjectManager.h. To summarize, the functionalities you have to implement are:

initPool() – Initialize the object manager upon starting.
destroyPool() – Clean up the object manager upon quitting.
insertObject(size) – Request a block of memory of given size from the object manager.
retrieveObject(id) – Retrieve the address of an object, identified by the reference id.
addReference(id) – Increment the reference count for the object with reference id.
dropReference(id) – Decrement the reference count for the object with reference id.
compact() – Initiate garbage collection (see below). Note this function is NOT part of the interface available to the client code. That is, it’s not declared in ObjectManager.h, but you need to implement it and call it in your own implementation.
dumpPool() – Print (to stdout) info about each object that is currently allocated including its id, start address, and size.
Garbage collection

You will implement a Mark and Sweep Defragmenting/Mark-compact garbage collector, as described in class. This function is implemented by compact() in the object manager. So that we can evaluate your implementation, every time the garbage collector runs, print out the following statistics (to stdout):

The number of objects that exist
The current number of bytes in use
The number of bytes collected
Data structures

You must manage the tracking of allocated objects using an index, which will be implemented using a linked list. Each node in your linked list contains the information needed to manage its associated object. Note that the index is implemented inside ObjectManager.c, you don’t need any additional files like list.{h,c}.

