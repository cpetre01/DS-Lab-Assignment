"""Module containing the abstraction of both requests and replies messages of the client.
 It also includes the definition of the operation codes"""

# ******************** OPERATION CODES *********************
# services called by client, served by server
REGISTER = 'REGISTER'
UNREGISTER = 'UNREGISTER'
CONNECT = 'CONNECT'
DISCONNECT = 'DISCONNECT'
SEND = 'SEND'
SENDATTACH = 'SENDATTACH'
QUIT = 'QUIT'

TEST = 'TEST'  # test client connection with this op, you should receive TEST_ERR_CODE = 100 from server

# services called by server, served by client
SEND_MESSAGE = 'SEND_MESSAGE'
SEND_MESS_ACK = 'SEND_MESS_ACK'


# ******************** TYPES *********************
class Header:
    """
        Class used to represent a request header.
        ...
        Attributes
        ----------
        op_code: str
            operation code used to represent the desired operation to be performed
        username: str
            name of the client
     """
    _op_code = str
    _username = str


class Item:
    """
        Class used to represent a request item.
        ...
        Attributes
        ----------
        listening_port: str
            port which a thread will be listening to for receiving server replies
        message: str
            message to be sent to a client
    """
    listening_port = str
    _message = str
    _message_id = str


class Request(object):
    """
        Class used to represent a request.
        ...
        Attributes
        ----------
        header: class
            header containing the username and operation code
        item: class
            item containing the listening port of the client and the message to be sent
    """

    _header = Header()
    _item = Item()

    @property
    def header(self):
        return self._header

    @property
    def item(self):
        return self._item


class Reply:
    """
        Class used to represent a server reply.
        ...
        Attributes
        ----------
        server_error_code: str
            error code sent by the server
         header: class
            header containing the username and operation code
        item: class
            item containing the listening port of the client and the message to be sent
    """

    _header = Header()
    _server_error_code = str
    _item = Item()

    @property
    def header(self):
        return self._header

    @property
    def item(self):
        return self._item

    @property
    def server_error_code(self):
        return self._server_error_code

    @server_error_code.setter
    def server_error_code(self, value):
        self._server_error_code = value
