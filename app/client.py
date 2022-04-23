import socket
from enum import Enum
import argparse
from src import netUtils
from src import utils


class client:
    # ******************** TYPES *********************
    # *
    # * @brief Return codes for the protocol methods
    class RC(Enum):
        OK = 0
        ERROR = 1
        USER_ERROR = 2

    # ****************** ATTRIBUTES ******************
    _server = None
    _port = -1

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
        sock = netUtils.connect_socket(server_address=(client._server, client._port))
        # and send te registration request
        netUtils.send_header(sock, request)
        # receive server reply (error code)
        err_code = netUtils.receive_server_response(sock)
        # close the socket
        sock.close()
        # print the corresponding error message
        if err_code == client.RC.OK:
            print("REGISTER OK")
        elif err_code == client.RC.USER_ERROR:
            print("USERNAME IN USE")
        elif err_code == client.RC.ERROR:
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
        sock = netUtils.connect_socket(server_address=(client._server, client._port))
        # and send te registration request
        netUtils.send_header(sock, request)
        # receive server reply (error code)
        err_code = netUtils.receive_server_response(sock)
        # close the socket
        sock.close()
        # print the corresponding error message
        if err_code == client.RC.OK:
            print("UNREGISTER OK")
        elif err_code == client.RC.USER_ERROR:
            print("USER DOES NOT EXIST")
        elif err_code == client.RC.ERROR:
            print("UNREGISTER FAIL")

    # *
    # * @param user - User name to connect to the system
    # *
    # * @return OK if successful
    # * @return USER_ERROR if the user does not exist or if it is already connected
    # * @return ERROR if another error occurred
    @staticmethod
    def connect(user):
        # first, we create the request
        request = utils.Request()
        # fill up the request
        request.header._op_code = utils.CONNECT
        request.header._username = str(user)
        request.item._destination_port = str(client._port)
        # now, we connect to the socket
        sock = netUtils.connect_socket(server_address=(client._server, client._port))
        # and send te connection request
        netUtils.send_connection_request(sock, request)
        # receive server reply (error code)
        err_code = netUtils.receive_server_response(sock)
        # close the socket
        sock.close()
        # print the corresponding error message
        if err_code == client.RC.OK:
            print("CONNECT OK")
        elif err_code == client.RC.ERROR:
            print("USER ALREADY CONNECTED")
        elif err_code == client.RC.USER_ERROR:
            print("CONNECT FAIL, USER DOES NOT EXIST")
        else:
            print("CONNECT FAIL")

    # *
    # * @param user - User name to disconnect from the system
    # *
    # * @return OK if successful
    # * @return USER_ERROR if the user does not exist
    # * @return ERROR if another error occurred
    @staticmethod
    def disconnect(user):
        # first, we create the request
        request = utils.Request()
        # fill up the request
        request.header._op_code = utils.DISCONNECT
        request.header._username = str(user)
        # now, we connect to the socket
        sock = netUtils.connect_socket(server_address=(client._server, client._port))
        # and send te registration request
        netUtils.send_header(sock, request)
        # receive server reply (error code)
        err_code = netUtils.receive_server_response(sock)
        # close the socket
        sock.close()
        # print the corresponding error message
        if err_code == client.RC.OK:
            print("DISCONNECT OK")
        elif err_code == client.RC.USER_ERROR:
            print("DISCONNECT FAIL: USER DOES NOT EXIST")
        elif err_code == client.RC.ERROR:
            print("DISCONNECT FAIL: USER NOT CONNECTED")
        else:
            print("DISCONNECT FAIL")

    # *
    # * @param user    - Receiver user name
    # * @param message - Message to be sent
    # *
    # * @return OK if the server had successfully delivered the message
    # * @return USER_ERROR if the user is not connected (the message is queued for delivery)
    # * @return ERROR the user does not exist or another error occurred

    @staticmethod
    def send(user, message, recipient):
        # first, we create the request
        request = utils.Request()
        # fill up the request
        request.header._op_code = utils.SEND
        request.header._username = str(user)
        request.item._recipient_username = str(recipient)
        if len(message) > 255:
            print("ERROR, MESSAGE TOO LONG")

        request.item._message = str(message)
        # now, we connect to the socket
        sock = netUtils.connect_socket(server_address=(client._server, client._port))
        # and send te message request
        netUtils.send_message_request(sock, request)
        # receive server reply (error code)
        err_code = netUtils.receive_server_response(sock)
        # close the socket
        sock.close()
        # print the corresponding error message
        if err_code == client.RC.OK:
            # in case of success, return the corresponding message id
            message_id = netUtils.receive_server_response(sock)
            print("SEND OK")
            return message_id
        elif err_code == client.RC.ERROR:
            print("SEND FAIL, USER DOES NOT EXIST")
        elif err_code == client.RC.USER_ERROR:
            print("SEND FAIL, USER NOT CONNECTED")
        else:
            print("SEND FAIL")


    # *
    # * @param user    - Receiver user name
    # * @param file    - file  to be sent
    # * @param message - Message to be sent
    # * 
    # * @return OK if the server had successfully delivered the message
    # * @return USER_ERROR if the user is not connected (the message is queued for delivery)
    # * @return ERROR the user does not exist or another error occurred
    @staticmethod
    def sendAttach(user, file, message):
        #  Write your code here
        return client.RC.ERROR

    # *
    # **
    # * @brief Command interpreter for the client. It calls the protocol functions.
    @staticmethod
    def shell():

        while (True):
            try:
                command = input("c> ")
                line = command.split(" ")
                if (len(line) > 0):

                    line[0] = line[0].upper()

                    if (line[0] == "REGISTER"):
                        if (len(line) == 2):
                            client.register(line[1])
                        else:
                            print("Syntax error. Usage: REGISTER <userName>")

                    elif (line[0] == "UNREGISTER"):
                        if (len(line) == 2):
                            client.unregister(line[1])
                        else:
                            print("Syntax error. Usage: UNREGISTER <userName>")

                    elif (line[0] == "CONNECT"):
                        if (len(line) == 2):
                            client.connect(line[1])
                        else:
                            print("Syntax error. Usage: CONNECT <userName>")

                    elif (line[0] == "DISCONNECT"):
                        if (len(line) == 2):
                            client.disconnect(line[1])
                        else:
                            print("Syntax error. Usage: DISCONNECT <userName>")

                    elif (line[0] == "SEND"):
                        if (len(line) >= 3):
                            #  Remove first two words
                            message = ' '.join(line[2:])
                            client.send(line[1], message)
                        else:
                            print("Syntax error. Usage: SEND <userName> <message>")

                    elif (line[0] == "SENDATTACH"):
                        if (len(line) >= 4):
                            #  Remove first two words
                            message = ' '.join(line[3:])
                            client.sendAttach(line[1], line[2], message)
                        else:
                            print("Syntax error. Usage: SENDATTACH <userName> <filename> <message>")

                    elif (line[0] == "QUIT"):
                        if (len(line) == 1):
                            break
                        else:
                            print("Syntax error. Use: QUIT")
                    else:
                        print("Error: command " + line[0] + " not valid.")
            except Exception as e:
                print("Exception: " + str(e))

    # *
    # * @brief Prints program usage
    @staticmethod
    def usage():
        print("Usage: python3 client.py -s <server> -p <port>")

    # *
    # * @brief Parses program execution arguments
    @staticmethod
    def parseArguments(argv):
        parser = argparse.ArgumentParser()
        parser.add_argument('-s', type=str, required=True, help='Server IP')
        parser.add_argument('-p', type=int, required=True, help='Server Port')
        args = parser.parse_args()

        if (args.s is None):
            parser.error("Usage: python3 client.py -s <server> -p <port>")
            return False

        if ((args.p < 1024) or (args.p > 65535)):
            parser.error("Error: Port must be in the range 1024 <= port <= 65535");
            return False

        client._server = args.s
        client._port = args.p

        return True

    # ******************** MAIN *********************
    @staticmethod
    def main(argv):
        if (not client.parseArguments(argv)):
            client.usage()
            return

        #  Write code here
        client.shell()
        print("+++ FINISHED +++")


if __name__ == "__main__":
    client.main([])
