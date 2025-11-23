# Important:
Local Web Server is using port **8887** <br>

# How to use:
1. Launch the injector.exe with arguments <process name.exe> <dll name.dll> <br>
Example: `injector.exe gmod.exe gluonapi.dll` <br>

2. Web Api requests <br>
Server hello `GET /` <br>
Execute **GLua** script `POST /execute [ Content-Type: text/plain ]` <br>

Javascript fetch example:
```javascript
fetch("/execute", {
	body: "print('Hello World!')",
	method: "POST",
});
```
