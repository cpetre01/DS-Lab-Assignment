import time, re
from spyne import Application, ServiceBase, Unicode, rpc
from spyne.protocol.soap import Soap11
from spyne.server.wsgi import WsgiApplication


class Project(ServiceBase):

    @rpc(Unicode, _returns=Unicode)
    def remove(ctx, st):
        time.sleep(1)
        return re.sub("\s\s+", " ", st)


application = Application(
    services=[Project],
    tns='http://tests.python-zeep.org/',
    in_protocol=Soap11(validator='lxml'),
    out_protocol=Soap11())
application = WsgiApplication(application)


if __name__ == '__main__':
    import logging
    from wsgiref.simple_server import make_server
    logging.basicConfig(level=logging.DEBUG)
    logging.getLogger('spyne.protocol.xml').setLevel(logging.DEBUG)
    logging.info("listening to http://127.0.0.1:8000")
    logging.info("wsdl is at: http://localhost:8000/?wsdl")
    server = make_server('127.0.0.1', 8000, application)
    server.serve_forever()