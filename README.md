# What is
This software download the HTML code of a site in a file named index.html

# How it works
For compilation use the command of UNIX `make`. It will found the Makefile and it will compiled the file `download.c`.
Then launch the software followed by URL of web site: 
```sh
./download [webSite]
```

You will have the new file index.html which it will contain the HTML code.
The software use the sockets and requests `GET` of `HTTP protocol`.

### Version
1.0.0