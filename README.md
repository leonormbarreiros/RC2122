# Projeto RC

The goal of this project is to develop a prototype of a centralized messaging service

The development of the project requires implementing a Directory Server (DS) and a
User Application (User). The DS server and the various User application instances are
intended to operate on different machines connected to the Internet.

The application should provide a messaging platform in which users can
subscribe to the groups of their interest, and post messages to those groups, as well as to
read the messages that have been published in those groups.

For the implementation, the application layer protocols operate according to the client server 
paradigm, using the transport layer services made available by the socket
interface.

The DS will be running on a machine with known IP address and ports.

## How to compile program

Opening the project folder a Makefile can be found and used to compile the Directory Server (DS) 
and User with the command *make*. 

### Run DS

To run the server use the command *./DS* with two possible flags:

- *-v* to activate verbose
- *-p __port__* to set a custom port for the server. Default port: **58012**

### Run User

To run the user use the command *./User* with two possible flags:

- *-n* to set a custom target server IP. Default target IP: **local host**

- *-p* to set a custom port. Default port: **58012**

## File organization

**proj_12** *auxiliary functions for the project*

**proj_12/Server** *server functionality of the project*

**proj_12/Client** *user functionality of the project*


### proj_12 

#### constant.hpp

File with the defenitions of constants used on the project.

#### utils.cpp

File containing all the auxiliary functions from validation to communication protocols used by 
the User and Directory Server (DS).

#### utils.hpp

Header file that contains the definition of the Socket and the also the namespace with the declaration for communication protocols, 
checkers and parsers functions.

### proj_12/Client

#### user.cpp

Main file of the user functionality.

Deals with initialization, termination and parsing the arguments used when invoking 
the user application. Handles, constructs and sends all requests to the Directory Server (DS).
Displays all the information received from the server.

#### user.hpp

Header file that contains the definition of user and the declaration of the used 
functions in the user.cpp file.

### proj_12/Server

#### server.cpp

Main file of the server functionality.

Deals with initialization, termination and parsing the arguments used when invoking 
the Directory Server (DS). Handles the requests received by the user and sends the information back
so it can be displayed.


#### server.hpp

Header file that contains the definition of server and grouplist and the declaration of the used 
functions in the server.cpp file.


#### Presistence Information storing system

**proj_12**

&emsp;|-> **USERS**

&emsp;&emsp;|-> ***UID***

&emsp;&emsp;&emsp;|-> **uid_pass.txt** *File that stores this users pass*

&emsp;&emsp;&emsp;|-> **uid_login.txt** *File created if the user is logged in into the system*

&emsp;|-> **GROUPS**

&emsp;&emsp;|->***GID***

&emsp;&emsp;&emsp;|-> **gid_name.txt** *File that stores groups name*

&emsp;&emsp;&emsp;|-> **uid.txt** *File created for the users that are subscribed containing their uid*

&emsp;&emsp;&emsp;|-> **MSG**

&emsp;&emsp;&emsp;&emsp;|-> ***MID***

&emsp;&emsp;&emsp;&emsp;&emsp;|-> **FILE** *File sent anexed to the message by the user*

&emsp;&emsp;&emsp;&emsp;&emsp;|-> **F N A M E.txt** *File with the files name*

&emsp;&emsp;&emsp;&emsp;&emsp;|-> **A U T H O R.txt** *File with the author's uid*

&emsp;&emsp;&emsp;&emsp;&emsp;|-> **T E X T.txt** *File with the message's text*


## Authors

*Diogo Dionísio 95560*

*Juliana Yang 95617*

*Leonor Barreiros 95618*
