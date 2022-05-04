"""Module containing the definition of the functions that will be in charge of sending data
to server socket, as well as connecting to the socket"""

# ******************** IMPORTS ***********************
import socket
import utils

# ******************** FUNCTIONS *********************


def connect_socket(server_address: tuple):
    """Function in charge of connecting to the server socket"""
    sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    print('connecting to {} port {}'.format(*server_address))
    sock.connect(server_address)
    return sock


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
    error_code = sock.recv(1).decode()
    return error_code


def receive_server_response(sock):
    """Function in charge of receiving a message from the server"""
    response = ''
    # we create a while loop in charge of receiving data from the server
    while True:
        msg = sock.recv(1)
        if msg == b'\0':
            break
    # the received data is decoded and returned
    response += msg.decode()
    return response


def listen_and_accept(sock):
    """Function in charge of listening at a free port and accepting the connection, receving server replies """
    sock.listen(1)
    # first, create the reply
    reply = utils.Reply()
    while True:
        # then, accept the connection
        connection, client_address = sock.accept()
        try:
            # receive the operation code of the server response
            reply.header._op_code = receive_server_response(sock)
            # in the case of a message:
            if reply.header._op_code == utils.SEND_MESSAGE:
                reply.header._username = receive_server_response(sock)
                reply.item._message_id = receive_server_response(sock)
                reply.item._message = receive_server_response(sock)
                print("c> MESSAGE ", reply.item._message_id, "FROM", reply.header._username, ":\n",  reply.item._message,"\n", "END")
            # in case of a message acknowledgement:
            elif reply.header._op_code == utils.SEND_MESS_ACK:
                reply.item._message_id = receive_server_response(sock)
                print("c> SEND MESSAGE ", reply.item._message_id, " OK")
            else:
                print("ERROR, INVALID OPERATION")
                break
        finally:
            connection.close()
            sock.close()










