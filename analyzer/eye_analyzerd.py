#!/usr/bin/python3
 
import sys, time
from daemon import Daemon
# import udsunicomm
# import bin_analyzer

class EyeAnalyzerDaemon(Daemon):

    #def setup(self):
        #self.obj = bin_analyzer.EyeAnalyzer()
        #self.data = udsunicomm.UdsUniCommAI()#(self.obj)
        #self.data.setup()

    def run(self, arg = None):
        import udsunicomm
        self.data = udsunicomm.UdsUniCommAI()#(self.obj)
        self.data.setup()
        self.data.recv_process()

if __name__ == "__main__":
    daemon = EyeAnalyzerDaemon('/tmp/eye_analyzerd.pid', stdin='/dev/null', stdout='/tmp/ai_log.txt', stderr='/tmp/ai_err.txt')
    if len(sys.argv) == 2:
        if 'start' == sys.argv[1]:
            #daemon.setup()
            daemon.start()
        elif 'stop' == sys.argv[1]:
            daemon.stop()
        elif 'print' == sys.argv[1]:
            #ea_inst = bin_analyzer.EyeAnalyzer()
            import udsunicomm
            uc = udsunicomm.UdsUniCommAI()#(ea_inst)
            uc.setup()
            uc.recv_process()
        else:
            print("Unknown command")
            sys.exit(2)
        sys.exit(0)
    else:
        print("usage: %s start|stop" % sys.argv[0])
        sys.exit(2)
