# Important:
Local Web Server is using port **8887**

# How to use:
1. Launch the injector.exe with arguments <process name.exe> <dll name.dll>
Example: `injector.exe gmod.exe gluonapi.dll`

2. Web Api requests
Server hello `GET /` <br>
Execute **GLua** script `POST /execute [ Content-Type: text/plain ]`

Javascript fetch example:
```javascript
fetch("/execute", {
	body: "print('Hello World!')",
	method: "POST",
});
```
