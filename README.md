Proper README coming once fully functional.

File must be named `api.py`
```py
import noxpy

def NoxMain():
    print("Init from Python")
    nox = noxpy.initNox()

    def get_handler(resp, req):
        print("GET handler from python called")
        return
    
    nox.createGet("/test", get_handler)

```

### To-do
- [ ] Instead of returning NULL, throw an exception
- [ ] Better repeated init blockage like shown in [docs](https://docs.python.org/3/extending/extending.html)
- [x] initNox
- [x] CreateGet
- [ ] CreatePost
- [ ] CreatePut
- [ ] CreateDelete
- [ ] WriteBuff
- [ ] WriteText
- [ ] WriteFile
- [ ] WriteJson
- [ ] probably more I forgor