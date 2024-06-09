import socket
from eyemetercomm import *

UDS_UNI_NAMES_FILEPATH = "/etc/roles.names"
UDS_UNI_SOCK_FOLDER = "/tmp"
UDS_UNI_SOCK_EXT = ".sock"

def udsuni_makesockname(name):
    return UDS_UNI_SOCK_FOLDER + '/' + name + UDS_UNI_SOCK_EXT

class UdsUniCommAI:

    def __init__(self):
        self.role = EYEMETER_ROLE_AI
        self.sock = socket(AF_UNIX, SOCK_DGRAM)
        self.name = ''
        self.other_socks = {}


    def read_roles_file(self):
        with open(UDS_UNI_NAMES_FILEPATH, 'r') as f:
            lines = f.readlines()
            for l in lines:
                datal = l.split(' ')
                if len(datal) < 2:
                    continue
                role = int(datal[0])
                if role == EYEMETER_ROLE_AI:
                    self.name = udsuni_makesockname(data[1].strip())
                else:
                    self.other_socks += {role : udsuni_makesockname(data[1].strip())}



        
