# DS-Lab-Assignment
Repo used for Lab Assignment in Distributed Systems

    client side written in Python3, server side written in C11

Project Structure:

    app: source code for server executable and client script

        client: client source code (Python)
    
            src: library source code and auxiliary source files
    
                netUtil.py: source code for netUtil library; Python network API
            
                util.py: constants and classes used on the client side; useful stuff

            client.py: client script source code

        server.c: server source code

    build: directory used to build the project; create it if it doesn't exist

    include/DS-Lab-Assignment: header files

        dbms: header files for DBMS library

            dbms.h: function prototypes used for DB management; server-side API

            dbmsUtil.h: function prototypes for functions called internally in the dbms module
 
        netUtil.h: header for netUtil library; contains function prototypes used to send and receive stuff; server-side network API

        util.h: types, constants and function prototypes used on the server side; useful stuff

    src: library source code and auxiliary source files

        dbms: source code for DBMS library
    
            dbms.c: source code for DB management; server-side API
    
            dbmsUtil.c: source code for the function prototypes defined in dbmsUtil.h
     
        netUtil.c: source code for netUtil library; C network API
    
        util.c: source code for the function prototypes defined in util.h
