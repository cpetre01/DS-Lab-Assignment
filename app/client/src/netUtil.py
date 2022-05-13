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
        # print(f"connecting to {server_address[0]} port {server_address[1]}")
        sock.connect(server_address)
        return sock
    except Exception as ex:
        print(f"Cannot connect to server. Exception: {ex}")
    

def send_header(sock, request):
    """Function in charge of sending the header to the server socket"""
    # send the op_code
    sock.sendall(request.header.op_code.encode('ascii'))
    sock.sendall(b'\0')

    # send the username
    sock.sendall(request.header.username.encode('ascii'))
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
    """Function in charge of sending the header, the recipient user and the message to the server socket"""
    # first, send the header
    send_header(sock, request)
    # we send the recipient user
    sock.sendall(request.item.recipient_username.encode('ascii'))
    sock.sendall(b'\0')
    # and also the message
    sock.sendall(request.item.message.encode('ascii'))
    sock.sendall(b'\0')


def receive_server_error_code(sock):
    """Function in charge of receiving a byte representing the error code from the server"""
    error_code = int.from_bytes(sock.recv(1), "big")
    return error_code


def receive_string(sock):
    """Function in charge of receiving a string from a given socket"""
    string = ''
    # we create a while loop in charge of receiving data from the socket
    while True:
        msg = sock.recv(1)
        # when the \0 character is reached, the string has ended, so it stops receiving characters
        if msg == b'\0':
            break
        # the received data is decoded and returned
        string += msg.decode()
    return string


def listen_and_accept(sock):
    """Function in charge of listening at a free port and accepting the connection, receiving server replies"""
    sock.listen(1)
    # first, create the reply
    reply = util.Reply()
    while True:
        # then, accept the connection
        connection, client_address = sock.accept()
        print(f"{sock.getsockname()=}")
        try:
            # receive the operation code of the server response
            reply.header.op_code = receive_string(sock)
            # in the case of a message:
            if reply.header.op_code == util.SEND_MESSAGE:
                reply.header.username = receive_string(sock)
                reply.item.message_id = receive_string(sock)
                reply.item.message = receive_string(sock)
                print(f"c> MESSAGE {reply.item.message_id} FROM {reply.header.username}:\n {reply.item.message}\nEND")
            # in case of a message acknowledgement:
            elif reply.header.op_code == util.SEND_MESS_ACK:
                reply.item.message_id = receive_string(sock)
                print(f"c> SEND MESSAGE {reply.item.message_id} OK")
            elif reply.header.op_code == util.END_LISTEN_THREAD:
                # end thread
                connection.close()
                sock.close()
                return None
            else:
                print("ERROR, INVALID OPERATION")
                break
        finally:
            connection.close()
            sock.close()
