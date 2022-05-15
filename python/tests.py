import unittest
import os
from client import Client
from src import util


class MyTestCase(unittest.TestCase):
    def test_init(self):
        client_a.server = os.getenv("SERVER_IP")
        client_a.port = int(os.getenv("SERVER_PORT"))
        print(f"{client_a.server=}")
        print(f"{client_a.port=}")

        client_b.server = os.getenv("SERVER_IP")
        client_b.port = int(os.getenv("SERVER_PORT"))
        print(f"{client_b.server=}")
        print(f"{client_b.port=}")

        self.assertEqual(True, True)  # add assertion here

    def test_register(self):
        self.assertEqual(client_a.register("a"), util.EC.SUCCESS.value)

    def test_unregister(self):
        self.assertEqual(client_a.unregister("a"), util.EC.SUCCESS.value)


if __name__ == '__main__':
    client_a = Client()
    client_b = Client()

    unittest.main()
