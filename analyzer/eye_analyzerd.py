#!/usr/bin/python3
 
import sys, time
from daemon import Daemon
import bin_analyzer
import udsunicomm

class EyeAnalyzerDaemon(Daemon):
    def run(self, arg = None):
        ea_inst = bin_analyzer.EyeAnalyzer()
        self.data = udsunicomm.UdsUniCommAI(ea_inst)
        self.data.setup()
        self.data.recv_process()

if __name__ == "__main__":
    daemon = EyeAnalyzerDaemon('/tmp/eye_analyzerd.pid')
    if len(sys.argv) == 2:
        if 'start' == sys.argv[1]:
            daemon.start()
        elif 'stop' == sys.argv[1]:
            daemon.stop()
        else:
            print("Unknown command")
            sys.exit(2)
        sys.exit(0)
    else:
        print("usage: %s start|stop" % sys.argv[0])
        sys.exit(2)