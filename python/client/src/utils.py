"""Module containing the abstraction of both requests and replies messages of the client.
 It also includes the definition of the operation codes"""

# ******************** OPERATION CODES *********************
REGISTER = 'REGISTER'
UNREGISTER = 'UNREGISTER'
CONNECT = 'CONNECT'
DISCONNECT = 'DISCONNECT'
SEND = 'SEND'
SENDATTACH = 'SENDATTACH'
QUIT = 'QUIT'


# ******************** TYPES *********************
class Header:
    _op_code = str
    _username = str


class Item:
    _recipient_username = str
    _destination_port = str
    _message = str

    @property
    def message(self):
        return self._message


class Request(object):
    _header = Header()
    _item = Item()

    @property
    def header(self):
        return self._header

    @property
    def item(self):
        return self._item


class Reply:
    _header = Header()
    _server_error_code = str
    _message_id = int
    _message = Item().message
