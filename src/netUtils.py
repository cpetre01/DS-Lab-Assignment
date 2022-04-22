"""Module containing the definition of the functions that will be in charge of sending data
to server socket, as well as connecting to the socket"""

# ******************** IMPORTS *********************
import utils
import client
import socket


# ******************** FUNCTIONS *********************
def connect_socket(server_address):
    """Function in charge of connecting to the server socket"""
    sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    print('connecting to {} port {}'.format(*server_address))
    sock.connect(server_address)
    return sock


def send_header(sock, request):
    """Function in charge of sending the header to the server socket"""
    # send the header
    sock.sendall(request.header._op_code.encode('utf-8'))
    sock.sendall(b'\0')
    sock.sendall(request.header._username.encode('utf-8'))
    sock.sendall(b'\0')


def send_connection_request(sock, request):
    """Function in charge of sending the header and the destination port to the server socket, so that the connection
    can be performed """
    # first, send the header
    send_header(sock, request)
    # now, send also the port
    sock.sendall(request.item._destination_port.encode('utf-8'))
    sock.sendall(b'\0')


def send_message_request(sock, request):
    """Function in charge of sending the header, the recipient user and the message to the server socket """
    # first, send the header
    send_header(sock, request)
    # we send the recipient user
    sock.sendall(request.item._recipient_username.encode('utf-8'))
    sock.sendall(b'\0')
    # and also the message
    sock.sendall(request.item._message.encode('utf-8'))
    sock.sendall(b'\0')


def receive_err_code(sock):
    """Function in charge of receiving the server error code"""
    err_code = ''
    while True:
        msg = sock.recv(1)
        if msg == b'\0':
            break
    err_code += msg.decode()
    return err_code





