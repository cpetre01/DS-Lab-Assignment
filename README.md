# DS-Lab-Assignment
Repo used for Lab Assignment in Distributed Systems

    client side written in Python3, server side written in C11

Project Structure:

    app: source code for server executable and client script

        client: client source code (Python)
    
            src: library source code and auxiliary source files
    
                netUtils.py: source code for netUtils library; Python network API
            
                utils.py: constants and classes used on the client side; useful stuff

            client.py: client script source code

        server.c: server source code

    build: directory used to build the project; create it if it doesn't exist

    include/DS-Lab-Assignment: header files

        dbms: header files for DBMS library

            dbms.h: function prototypes used for DB management; server-side API

            dbmsUtils.h: function prototypes for functions called internally in the dbms module
 
        netUtils.h: header for netUtils library; contains function prototypes used to send and receive stuff; server-side network API

        utils.h: types, constants and function prototypes used on the server side; useful stuff

    src: library source code and auxiliary source files

        dbms: source code for DBMS library
    
            dbms.c: source code for DB management; server-side API
    
            dbmsUtils.c: source code for the function prototypes defined in dbmsUtils.h
     
        netUtils.c: source code for netUtils library; C network API
    
        utils.c: source code for the function prototypes defined in utils.h
