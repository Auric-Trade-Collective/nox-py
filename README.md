# Not up-to-date, do not use. Still in development.

Proper README coming once fully functional.

File must be named `api.py`
```py
import noxpy

def NoxMain():
    print("Init from Python")
    nox = noxpy.initNox()

    def get_handler(resp, req):
        msg = f"GET {req.endpoint} from {req.remoteAddr}"
        print(msg)
        resp.writeText(msg)
        return
    
    nox.createGet("/test", get_handler)

```

### To-do
- [ ] Instead of returning NULL, throw an exception
- [ ] Better repeated init blockage like shown in [docs](https://docs.python.org/3/extending/extending.html)
- [x] initNox
- [x] CreateGet
- [x] CreatePost
- [x] CreatePut
- [x] CreateDelete
- [ ] WriteBuff
- [x] WriteText
- [ ] WriteFile
- [ ] WriteJson
- [ ] probably more I forgor

### Explaining Each File
- `core.c`: The host entrypoint, `CreateNoxApi`.
- `module.c`: The actual python module entrypoint, as well as `initNox()`.
- `create_functions.c`: Python functions for createGet, createPost, etc.
- `write_functions.c`: Python functions for writeText, writeJson, etc.
- `request_types.c`: Things for request py object.
- `response_types.c`: Things for response py object.
