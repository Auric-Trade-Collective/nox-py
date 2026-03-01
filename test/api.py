import noxpy

def NoxMain():
    print("Init from Python")
    nox = noxpy.initNox()

    def get_handler(resp, req):
        print("GET handler from python called")
        return
    
    nox.createGet("/test", get_handler)
