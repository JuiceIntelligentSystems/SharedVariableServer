# SharedVariableServer
A simple web server that sends data back and forth constantly between a C++ program and web server.

Can run on any Linux system with the jsoncpp library installed.
When compiling make sure to include "-ljsoncpp" in tasks.json or in gcc.

How to run:
- Host the server locally on your system e.g. with NGINX or with VSCode live server plugin
- Run the compiled C++ program while the server is running.

When you move the slider, the value should print in the terminal.
There's also a random number generator where the C++ program sends a random number everytime a GET request is called.
All data is sent in JSON format.