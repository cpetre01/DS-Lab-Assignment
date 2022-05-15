import unittest
import os
from client import Client
from src import util


class MyTestCase(unittest.TestCase):
    def test_init(self):
        client_a.server = os.getenv("SERVER_IP")
        client_a.port = int(os.getenv("SERVER_PORT"))
        # print(f"{client_a.server=}")
        # print(f"{client_a.port=}")

        client_b._server = os.getenv("SERVER_IP")
        client_b._port = int(os.getenv("SERVER_PORT"))
        # print(f"{client_b.server=}")
        # print(f"{client_b.port=}")

        self.assertEqual(True, True)  # add assertion here

    # register new user a
    def test_register_a(self):
        print(f"{client_a.server=}")
        print(f"{client_a.port=}")
        self.assertEqual(client_a.register("a"), util.EC.SUCCESS.value)

    # register new user b
    def test_register_b(self):
        print(f"{client_b.server=}")
        print(f"{client_b.port=}")
        self.assertEqual(client_b.register("b"), util.EC.SUCCESS.value)

    # register the same user as before, a
    def test_register_registered_user(self):
        self.assertEqual(client_a.register("a"), util.EC.REGISTER_USR_ALREADY_REG.value)

    # unregister non existing user, c
    def test_unregister_c(self):
        self.assertEqual(client_a.unregister("c"), util.EC.UNREGISTER_USR_NOT_EXISTS.value)

    # connect a registered user, a
    def test_connect_a(self):
        client_a.server = os.getenv("SERVER_IP")
        client_a.port = int(os.getenv("SERVER_PORT"))
        print(f"{client_a.server=}")
        print(f"{client_a.port=}")
        client_a.register("a")
        self.assertEqual(client_a.connect('a'), util.EC.SUCCESS.value)
        client_a.unregister("a")

    # unregister user b
    def test_unregister_a(self):
        # print(f"{client_a.server=}")
        # print(f"{client_a.port=}")
        self.assertEqual(client_a.unregister("a"), util.EC.SUCCESS.value)

    # unregister user b
    def test_unregister_b(self):
        # print(f"{client_b.server=}")
        # print(f"{client_b.port=}")
        self.assertEqual(client_b.unregister("b"), util.EC.SUCCESS.value)


if __name__ == '__main__':
    client_a = Client()
    client_b = Client()

    unittest.main()
