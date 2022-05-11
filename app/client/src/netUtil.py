"""Module containing the definition of the functions that will be in charge of sending data
to server socket, as well as connecting to the socket"""

# ******************** IMPORTS ***********************
import socket
import threading
from src import util

# ******************** FUNCTIONS *********************


def connect_socket(server_address: tuple):
    """Function in charge of connecting to the server socket"""
    try:
        sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        print('connecting to {} port {}'.format(*server_address))
        sock.connect(server_address)
        return sock
    except Exception as ex:
        print(f"Cannot connect to server. Exception: {ex}")
    


def send_header(sock, request):
    """Function in charge of sending the header to the server socket"""
    # send the op_code
    sock.sendall(request.header._op_code.encode('ascii'))
    sock.sendall(b'\0')

    # send the username
    sock.sendall(request.header._username.encode('ascii'))
    sock.sendall(b'\0')


def send_connection_request(sock, request):
    """Function in charge of sending the header and the destination port to the server socket, so that the connection
    can be performed """
    # first, send the header
    send_header(sock, request)
    # now, send also the port
    sock.sendall(request.item.listening_port.encode('ascii'))
    sock.sendall(b'\0')


def send_message_request(sock, request):
    """Function in charge of sending the header, the recipient user and the message to the server socket """
    # first, send the header
    send_header(sock, request)
    # we send the recipient user
    sock.sendall(request.item._recipient_username.encode('ascii'))
    sock.sendall(b'\0')
    # and also the message
    sock.sendall(request.item._message.encode('ascii'))
    sock.sendall(b'\0')


def receive_error_code(sock):
    """Function in charge of receiving a byte representing the error code from the server"""
    error_code = int.from_bytes(sock.recv(1), "big")
    return error_code


def receive_server_response(sock):
    """Function in charge of receiving a message from the server"""
    response = ''
    # we create a while loop in charge of receiving data from the server
    while True:
        msg = sock.recv(1)
        # when the \0 character is reached, the string has ended and it stops receiving characters
        if msg == b'\0':
            break
    # the received data is decoded and returned
    response += msg.decode()
    return response


def listen_and_accept(sock, event):
    """Function in charge of listening at a free port and accepting the connection, receving server replies """
    sock.listen(1)
    # first, create the reply
    reply = util.Reply()
    while True:
        # then, accept the connection
        connection, client_address = sock.accept()

        # shut down thread if event is set
        if event.isSet():
            connection.close()
            sock.close()
            return

        try:
            # receive the operation code of the server response
            reply.header._op_code = receive_server_response(sock)
            # in the case of a message:
            if reply.header._op_code == util.SEND_MESSAGE:
                reply.header._username = receive_server_response(sock)
                reply.item._message_id = receive_server_response(sock)
                reply.item._message = receive_server_response(sock)
                print(f"c> MESSAGE {reply.item._message_id} FROM {reply.header._username}:\n {reply.item._message}\nEND")
            # in case of a message acknowledgement:
            elif reply.header._op_code == util.SEND_MESS_ACK:
                reply.item._message_id = receive_server_response(sock)
                print(f"c> SEND MESSAGE {reply.item._message_id} OK")
            else:
                print("ERROR, INVALID OPERATION")
                break
        finally:
            connection.close()
            sock.close()










