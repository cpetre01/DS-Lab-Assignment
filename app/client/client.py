import argparse
import socket
from threading import Thread
from enum import Enum
from src import netUtils, utils


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
    CURRENT_USER = None

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
        request = utils.Request()
        # fill up the request
        request.header._op_code = utils.REGISTER
        request.header._username = str(user)
        # now, we connect to the socket
        sock = netUtils.connect_socket(server_address=(Client._server, Client._port))
        # and send te registration request
        netUtils.send_header(sock, request)
        # receive server reply (error code)
        reply = utils.Reply()
        reply.server_error_code = netUtils.receive_error_code(sock)
        # close the socket
        sock.close()
        # print the corresponding error message
        if reply.server_error_code == Client.RC.OK:
            print("REGISTER OK")
        elif reply.server_error_code == Client.RC.ERROR:
            print("USERNAME IN USE")
        elif reply.server_error_code == Client.RC.USER_ERROR:
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
        request = utils.Request()
        # fill up the request
        request.header._op_code = utils.UNREGISTER
        request.header._username = str(user)
        # now, we connect to the socket
        sock = netUtils.connect_socket(server_address=(Client._server, Client._port))
        # and send te registration request
        netUtils.send_header(sock, request)
        # receive server reply (error code)
        reply = utils.Reply()
        reply.server_error_code = netUtils.receive_error_code(sock)
        # close the socket
        sock.close()
        # print the corresponding error message
        if reply.server_error_code == Client.RC.OK:
            print("UNREGISTER OK")
        elif reply.server_error_code == Client.RC.ERROR:
            print("USER DOES NOT EXIST")
        elif reply.server_error_code == Client.RC.USER_ERROR:
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
        request = utils.Request()
        # now, create a socket and bind to port 0
        listen_sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        listen_sock.bind((Client._server, 0))
        # we get the port using getsockname
        port = listen_sock.getsockname()[1]
        # fill up the request
        request.header._op_code = utils.CONNECT
        request.header._username = str(user)
        request.item.listening_port = str(port)
        # now, we connect to the socket
        sock = netUtils.connect_socket(server_address=(Client._server, Client._port))
        # and send the connection request
        netUtils.send_connection_request(sock, request)
        # receive server reply (error code)
        reply = utils.Reply()
        reply.server_error_code = netUtils.receive_error_code(sock)
        # close the socket
        sock.close()
        # print the corresponding error message
        if reply.server_error_code == Client.RC.OK:
            print("CONNECT OK")
            # if the connection was performed successfully, update the current user and listen
            Client.CURRENT_USER = str(user)
            t = Thread(target=netUtils.listen_and_accept, args=[listen_sock])
            t.start()
        elif reply.server_error_code == Client.RC.USER_ERROR:
            print("USER ALREADY CONNECTED")
        elif reply.server_error_code == Client.RC.ERROR:
            print("CONNECT FAIL, USER DOES NOT EXIST")
        elif reply.server_error_code == Client.RC.OTHER_ERROR:
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
        request = utils.Request()
        # fill up the request
        request.header._op_code = utils.DISCONNECT
        request.header._username = str(user)
        # now, we connect to the socket
        sock = netUtils.connect_socket(server_address=(Client._server, Client._port))
        # and send te registration request
        netUtils.send_header(sock, request)
        # receive server reply (error code)
        reply = utils.Reply()
        reply.server_error_code = netUtils.receive_error_code(sock)
        # close the socket
        sock.close()
        # print the corresponding error message
        if reply.server_error_code == Client.RC.OK:
            Client.CURRENT_USER = None
            print("DISCONNECT OK")
        elif reply.server_error_code == Client.RC.ERROR:
            print("DISCONNECT FAIL: USER DOES NOT EXIST")
        elif reply.server_error_code == Client.RC.USER_ERROR:
            print("DISCONNECT FAIL: USER NOT CONNECTED")
        elif reply.server_error_code == Client.RC.OTHER_ERROR:
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
        request = utils.Request()
        # fill up the request
        request.header._op_code = utils.SEND
        request.header._username = Client.CURRENT_USER
        request.item._recipient_username = str(recipient)
        if len(message) > 255:
            print("ERROR, MESSAGE TOO LONG")
        request.item._message = str(message)
        # now, we connect to the socket
        sock = netUtils.connect_socket(server_address=(Client._server, Client._port))
        # and send te message request
        netUtils.send_message_request(sock, request)
        # receive server reply (error code)
        reply = utils.Reply()
        reply.server_error_code = netUtils.receive_error_code(sock)
        # close the socket
        sock.close()
        # print the corresponding error message
        if reply.server_error_code == Client.RC.OK:
            # in case of success, return the corresponding message id
            message_id = netUtils.receive_server_response(sock)
            print("SEND OK - MESSAGE", message_id)
        elif reply.server_error_code == Client.RC.ERROR:
            print("SEND FAIL, USER DOES NOT EXIST")
        elif reply.server_error_code == Client.RC.USER_ERROR:
            print("SEND FAIL, USER NOT CONNECTED")

    # *
    # * @param user    - Receiver user name
    # * @param file    - file  to be sent
    # * @param message - Message to be sent
    # * 
    # * @return OK if the server had successfully delivered the message
    # * @return USER_ERROR if the user is not connected (the message is queued for delivery)
    # * @return ERROR the user does not exist or another error occurred
    @staticmethod
    def send_attach(user, file, message):
        #  Write your code here
        print("SERVICE NOT SUPPORTED")
        return Client.RC.ERROR

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

                    elif line[0] == "SENDATTACH":
                        if len(line) >= 4:
                            #  Remove first two words
                            message = ' '.join(line[3:])
                            Client.send_attach(line[1], line[2], message)
                        else:
                            print("Syntax error. Usage: SENDATTACH <userName> <filename> <message>")

                    elif line[0] == "QUIT":
                        if len(line) == 1:
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
