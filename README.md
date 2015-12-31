
# CS 111 winter 2015: Assignment 3 - Key-Value Store and Server

Brittany Arthur (bharthur)
Jason Heron (jheron)
Peter Froud (pfroud)

We are using one (1) late pass.

**TO BUILD:**

Type 'make'.
Run ./server and ./client.

## Part I – Key-Value Store

The primary goal of this first part of the assignment is to let you experience the power and flexibility of the simple byte-stream
file interface provided by the Unix operating system. To realize that goal, you will implement what is commonly called a
*key-value store* – in other words, you will build a hash table that is instantiated as a file on disk.

You should implement a library (a set of functions) as given below:

* ``int initialize(char *file, int length, int size)``
* ``int insert(char *key, void *value, int length)``
* ``int fetch (char *key, void *value, int *length)``
* ``int probe (char *key)``
* ``int delete(char *key)``

When you call ``initialize`` it will either create—if this is the first use—or open a file that will contain your hash table.
The return value from ``initialize`` is a file descriptor, or −1 in the case of an error. The argument ``size`` will be the number
of entries in your hash table.

As you will recall from CS 101, a hash function is a function [...]
where the domain of the function is the key-space, in our case a set of character strings, and the range of the function is the
natural numbers from 0 to n−1. This means that the hash table will have n entries, and arithmetic on the table indices will
be done mod n (in C as % N).

Starting with ``insert``, which will take a pointer to the key, a pointer to the value (which *should* contain a copy of the key)
and insert it into the hash table. This is accomplished using an ``lseek`` and a ``write``—but be careful, since you might have a
*hash collision*! The return value of ``insert`` is the slot location where the value is stored, or −1 if the hash table is full.

How do you avoid a hash collision? First of all, the occupied slots should be marked with a *magic number* ``0xDEADD00D`` as
the first four bytes of the entry, if these are present then you you should do *linear probing* (moving on to the first unoccupied
slot in sequence). Checking for collision, then, requires that you *read* before you *write*.

The ``fetch`` function takes a key and returns the value. You will note that both the ``insert`` and ``fetch`` functions require
a ``length`` to be specified. That is because we are passing the object (likely a ``struct``) as a ``void *``. Since you are doing linear
probing, if the value is not at its hash location you need to continue looking (linearly) until you find it or you come to an
empty slot, in which case it is not found. The return value of ``fetch`` is the slot location of where the value is stored, or −1 if
it is not found.

What then is a slot? A slot is the area of the file that holds the value, and of course the magic number. For simplicity—and
as I have said, simplicity is chief among the virtues—make the value specify a maximum fixed size for the objects; as a result,
the slots will be a fixed size as well. The location of the slot in the file can then be calculated as the product of the hash value
and the size of the slot.

The probe function searches to find the slot location of the value that matches the key, or −1 if it is not found. As with
``insert`` it hashes to the location, looks there, and if the magic number is not present then it is not found, if some other value
is there (the key does not match) then continue with a linear probe until it is found or you encounter an empty slot.

Finally, you need to implement ``delete``, which is harder than simply setting the magic number to *zero*. Why is that?
Consider how linear probing works: you have a value where it ought to be, and others in locations immediately following.
If you delete one of those values then you need to move the ones forward of it back to fill the hole. When do you stop?
Either when you encounter an empty slot, or when you encounter a value that is hashed to its *proper location* (not displaced
by probing). The return value from ``delete`` is 0 if successful, and −1 if the key was not found.

You must create a ``test harness`` that demonstrates the full functionality of your library. It should insert, find, and delete at
least several thousand key-value pairs.

## Part II – Server Process

The purpose of this assignment is to get familiar with the Unix socket interface. For this assignment you will write a simple
server in C that will provide *concurrent* access to your key-value store by *multiple* client processes.

### 2.1 Your Server

1. Create a socket, bind the socket to port 10732: ``socket()``, ``bind()``.
2. Wait for client connection to complete: ``listen()``, ``accept()``.
3. You are implementing concurrency, ``fork()`` or create a new thread using ``pthread.create()``.
4. Once a request has been received, parse the request. Your server will need to handle ``INIT``, ``GET``, ``PUT``, and ``DEL``.
5. Check if the file in the request is present.
If the key is not present, send a ``404`` message (just like a web server).  
If the key is present. Send a ``200 OK`` message (again, like a web server) along with the contents of the object. Similarly, success of an insert should be indicated by a ``201 OK`` message.
Unknown operations should result in a ``513 LUL WUT?`` message.
6. Return to listening state.

### 2.2 Assignment Specifications

* Your server needs to support ``INIT``, ``GET``, ``PUT``, and ``DEL`` commands.
* Your server should create a socket, bind it to port 10732, and listen for a request

### 2.3 Output

You will need to create a client library that will connect to your server and perform operations that exercise your key value store. You can use the same test harness that you used for the first part of the assignment if you replace the calls to the functions with wrapper functions that send the appropriate commands to the server.

Your must use a *human readable* format, like XML. For example,

``INIT <name>myHashTable</name><length>4096</length><size>1000000</size>``

or

``PUT <key>Coconuts</key><value>I have a lovely bunch of coconuts!</value>``
which might result in

``201 OK``

or

``GET <key>Bat Country</key>``

which might result in

``200 OK 
<value>We Can’t Stop Here, This is Bat Country</value>``
