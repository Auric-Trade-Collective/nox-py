import noxpy

def NoxMain():
    print("Init from Python")
    nox = noxpy.initNox()

    def get_handler(resp, req):
        msg = f"GET {req.endpoint} from {req.remoteAddr}"
        print(msg)
        resp.writeText(msg)
        return
    
    def post_handler(resp, req):
        msg = f"POST {req.endpoint} from {req.remoteAddr}"
        print(msg)
        resp.writeText(msg)
        return
    
    def put_handler(resp, req):
        msg = f"PUT {req.endpoint} from {req.remoteAddr}"
        print(msg)
        resp.writeText(msg)
        return
    
    def delete_handler(resp, req):
        msg = f"DELETE {req.endpoint} from {req.remoteAddr}"
        print(msg)
        resp.writeText(msg)
        return
    
    nox.createGet("/test", get_handler)
    nox.createPost("/test", post_handler)
    nox.createPut("/test", put_handler)
    nox.createDelete("/test", delete_handler)
