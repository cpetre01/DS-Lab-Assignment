import argparse
import socket
import threading
from threading import Thread
from enum import Enum
from src import netUtil, util
global t


class Client:
    # ******************** TYPES *********************
    # *
    # * @brief Return codes for the protocol methods
    class RC(Enum):
        OK = 0
        ERROR = 1
        USER_ERROR = 2
        OTHER_ERROR = 3

    # ****************** ATTRIBUTES ******************
    _server = None
    _port = -1
    # username of currently connected user
    CURRENT_USER = None
    # event used to shut down the receiving thread when disconnect service is called
    dcn_event = threading.Event()

    # ******************** METHODS *******************
    # *
    # * @param user - User name to register in the system
    # * 
    # * @return OK if successful
    # * @return USER_ERROR if the user is already registered
    # * @return ERROR if another error occurred
    @staticmethod
    def register(user):
        # first, we create the request
        request = util.Request()
        # fill up the request
        request.header._op_code = util.REGISTER
        request.header._username = str(user)
        # now, we connect to the socket
        sock = netUtil.connect_socket(server_address=(Client._server, Client._port))
        if sock:
            # and send te registration request
            netUtil.send_header(sock, request)
            # receive server reply (error code)
            reply = util.Reply()
            reply.server_error_code = netUtil.receive_error_code(sock)
            # close the socket
            sock.close()
            # print the corresponding error message
            if reply.server_error_code == Client.RC.OK.value:
                print("REGISTER OK")
            elif reply.server_error_code == Client.RC.ERROR.value:
                print("USERNAME IN USE")
            elif reply.server_error_code == Client.RC.USER_ERROR.value:
                print("REGISTRATION FAIL")

    # *
    # 	 * @param user - User name to unregister from the system
    # 	 *
    # 	 * @return OK if successful
    # 	 * @return USER_ERROR if the user does not exist
    # 	 * @return ERROR if another error occurred
    @staticmethod
    def unregister(user):
        # first, we create the request
        request = util.Request()
        # fill up the request
        request.header._op_code = util.UNREGISTER
        request.header._username = str(user)
        # now, we connect to the socket
        sock = netUtil.connect_socket(server_address=(Client._server, Client._port))
        if sock:
            # and send te registration request
            netUtil.send_header(sock, request)
            # receive server reply (error code)
            reply = util.Reply()
            reply.server_error_code = netUtil.receive_error_code(sock)
            # close the socket
            sock.close()
            # print the corresponding error message
            if reply.server_error_code == Client.RC.OK.value:
                print("UNREGISTER OK")
            elif reply.server_error_code == Client.RC.ERROR.value:
                print("USER DOES NOT EXIST")
            elif reply.server_error_code == Client.RC.USER_ERROR.value:
                print("UNREGISTER FAIL")

    # *
    # * @param user - User name to connect to the system
    # *
    # * @return OK if successful
    # * @return USER_ERROR if the user does not exist or if it is already connected
    # * @return ERROR if another error occurred
    @staticmethod
    def connect(user):
        global t
        # first, we create the request
        request = util.Request()
        # now, create a socket and bind to port 0
        listen_sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        listen_sock.bind((Client._server, 0))
        # we get the port using getsockname
        port = listen_sock.getsockname()[1]
        # fill up the request
        request.header._op_code = util.CONNECT
        request.header._username = str(user)
        request.item.listening_port = str(port)
        # now, we connect to the socket
        sock = netUtil.connect_socket(server_address=(Client._server, Client._port))
        if sock:
            # and send the connection request
            netUtil.send_connection_request(sock, request)
            # receive server reply (error code)
            reply = util.Reply()
            reply.server_error_code = netUtil.receive_error_code(sock)
            # close the socket
            sock.close()
            # print the corresponding error message
            if reply.server_error_code == Client.RC.OK.value:
                print("CONNECT OK")
                # if the connection was performed successfully, update the current user and listen
                Client.CURRENT_USER = str(user)
                t = Thread(target=netUtil.listen_and_accept, args=(listen_sock, Client.dcn_event))
                t.daemon = True
                t.start()
            elif reply.server_error_code == Client.RC.USER_ERROR.value:
                print("USER ALREADY CONNECTED")
            elif reply.server_error_code == Client.RC.ERROR.value:
                print("CONNECT FAIL, USER DOES NOT EXIST")
            elif reply.server_error_code == Client.RC.OTHER_ERROR.value:
                print("CONNECT FAIL")

    # *
    # * @param user - User name to disconnect from the system
    # *
    # * @return OK if successful
    # * @return USER_ERROR if the user does not exist
    # * @return ERROR if another error occurred
    @staticmethod
    def disconnect(user):
        global t
        # first, we create the request
        request = util.Request()
        # fill up the request
        request.header._op_code = util.DISCONNECT
        request.header._username = str(user)
        # now, we connect to the socket
        sock = netUtil.connect_socket(server_address=(Client._server, Client._port))
        if sock:
            # and send te registration request
            netUtil.send_header(sock, request)
            # receive server reply (error code)
            reply = util.Reply()
            reply.server_error_code = netUtil.receive_error_code(sock)
            # close the socket
            sock.close()
            # print the corresponding error message
            if reply.server_error_code == Client.RC.OK.value:
                Client.CURRENT_USER = None
                Client.dcn_event.set()
                # t.join()
                print("DISCONNECT OK")
            elif reply.server_error_code == Client.RC.ERROR.value:
                print("DISCONNECT FAIL: USER DOES NOT EXIST")
            elif reply.server_error_code == Client.RC.USER_ERROR.value:
                print("DISCONNECT FAIL: USER NOT CONNECTED")
            elif reply.server_error_code == Client.RC.OTHER_ERROR.value:
                print("DISCONNECT FAIL")

    # *
    # * @param user    - Receiver user name
    # * @param message - Message to be sent
    # *
    # * @return OK if the server had successfully delivered the message
    # * @return USER_ERROR if the user is not connected (the message is queued for delivery)
    # * @return ERROR the user does not exist or another error occurred

    @staticmethod
    def send(recipient, message):
        # first, we create the request
        request = util.Request()
        # fill up the request
        request.header._op_code = util.SEND
        request.header._username = Client.CURRENT_USER
        request.item._recipient_username = str(recipient)
        if len(message) > 255:
            print("ERROR, MESSAGE TOO LONG")
        request.item._message = str(message)
        # now, we connect to the socket
        sock = netUtil.connect_socket(server_address=(Client._server, Client._port))
        if sock:
            # and send te message request
            netUtil.send_message_request(sock, request)
            # receive server reply (error code)
            reply = util.Reply()
            reply.server_error_code = netUtil.receive_error_code(sock)
            # close the socket
            sock.close()
            # print the corresponding error message
            if reply.server_error_code == Client.RC.OK.value:
                # in case of success, return the corresponding message id
                message_id = netUtil.receive_server_response(sock)
                print(f"SEND OK - MESSAGE {message_id}")
            elif reply.server_error_code == Client.RC.ERROR.value:
                print("SEND FAIL, USER DOES NOT EXIST")
            elif reply.server_error_code == Client.RC.USER_ERROR.value:
                print("SEND FAIL, USER NOT CONNECTED")

    @staticmethod
    def shell():
        """Simple Command Line Interface for the client. It calls the protocol functions."""
        while True:
            try:
                command = input("c> ")
                line = command.split(" ")
                if len(line) > 0:

                    line[0] = line[0].upper()

                    if line[0] == "REGISTER":
                        if len(line) == 2:
                            Client.register(line[1])
                        else:
                            print("Syntax error. Usage: REGISTER <userName>")

                    elif line[0] == "UNREGISTER":
                        if len(line) == 2:
                            Client.unregister(line[1])
                        else:
                            print("Syntax error. Usage: UNREGISTER <userName>")

                    elif line[0] == "CONNECT":
                        if len(line) == 2:
                            Client.connect(line[1])
                        else:
                            print("Syntax error. Usage: CONNECT <userName>")

                    elif line[0] == "DISCONNECT":
                        if len(line) == 2:
                            Client.disconnect(line[1])
                        else:
                            print("Syntax error. Usage: DISCONNECT <userName>")

                    elif line[0] == "SEND":
                        if len(line) >= 3:
                            #  Remove first two words
                            message = ' '.join(line[2:])
                            Client.send(line[1], message)
                        else:
                            print("Syntax error. Usage: SEND <userName> <message>")

                    elif line[0] == "QUIT":
                        if len(line) == 1:
                            if Client.CURRENT_USER:
                                Client.disconnect(Client.CURRENT_USER)
                            break
                        else:
                            print("Syntax error. Use: QUIT")
                    else:
                        print(f"Error: command {line[0]} not valid.")
            except Exception as ex:
                print(f"Exception: {ex}")

    # *
    # * @brief Prints program usage
    @staticmethod
    def usage():
        print("Usage: python3 client.py -s <server> -p <port>")

    # *
    # * @brief Parses program execution arguments
    @staticmethod
    def parse_arguments():
        parser = argparse.ArgumentParser()
        parser.add_argument('-s', type=str, required=True, help='Server IP')
        parser.add_argument('-p', type=int, required=True, help='Server Port')
        args = parser.parse_args()

        if not args.s:
            parser.error("Usage: python3 client.py -s <server> -p <port>")
            return False

        if (args.p < 1024) or (args.p > 65535):
            parser.error("Error: Port must be in the range 1024 <= port <= 65535")
            return False

        Client._server = args.s
        Client._port = args.p

        return True

    # ******************** MAIN *********************
    @staticmethod
    def main():
        if not Client.parse_arguments():
            Client.usage()
            return

        Client.shell()
        print("+++ FINISHED +++")


if __name__ == "__main__":
    Client.main()
