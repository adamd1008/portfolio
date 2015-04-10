# surf

This is a set of Python classes designed to make surfing easy! Surfing as in surfing on Counter-Strike and other Valve GoldSrc/Source games.

It uses the SQLite database stored in surf.db. This will contain your choice of surf servers and maps along with their tier and your personal ratings of them.

A little knowledge of Python will go a long way but hopefully you wont need any. Just remember that if you get stuck and want to get back to the ">>>" prompt, press Ctrl+C.

## Getting started (Windows)

* If you don't have it already, download Python 2.7 from https://www.python.org/downloads/
* Ensure the path to python.exe is in your PATH environment variable
* Make a shortcut on your desktop of run.bat

## Getting started (other)

(This should work for Mac and Linux!)

* Create a shortcut to run.sh

## Surf servers

### Editable fields on a server (defaults in parentheses):

`nick' : str
`addr' : str
`port' : int (27015)

### Adding a server to your watch list:

    >>> srv = SourceServer(SourceServer.getNextID(), "<server nickname>", "<server hostname/IP>", <port>)
    >>> srv.insert()

### Updating server details:

    >>> srv = SurfDb.getServer(<server id>)
    >>> srv.nick = "New nickname"
    >>> srv.addr = "<new hostname/IP>"
    >>> srv.port = <new port>
    >>> srv.update()

### Deleting a server:

    >>> srv = SurfDb.getServer(<server id>)
    >>> srv.delete()

## Maps

### Editable fields on a map (defaults in parentheses):

`tier' : int (-1)
`rating' : int (-1)
`stages' : int (-1)
`bonus' : int (-1)
`complete' : bool (False)
`favourite' : bool (False)

### Add a map:

    >>> m = SurfDb.getMap("surf_pyrism_njv")
    Map not found; returning defaults
    >>> m.rating = 10
    >>> m.tier = 5
    >>> m.insert()

### Update a map:

    >>> m = SurfDb.getMap("surf_ez")
    >>> m.complete = True
    >>> m.rating = 1
    >>> m.update()

### Delete a map:

    >>> m = SurfDb.getMap("surf_jizznipples")
    >>> m.delete()

## Server list

### One-time server list:

    >>> SurfDb.prettyPrint()

(or)

    >>> SurfDb.pp()

### Continuous monitoring:

    >>> SurfDb.monitor()

(or)

    >>> SurfDb.mon()

SurfDb.monitor() has an optional argument which is the delay between refreshes. The default is 180 seconds.

### Joining a server:

    >>> SurfDb.join(<server ID>)
    Joining server "XXX" (14/32)

### Joining a full server:

    >>> SurfDb.join(<server ID>)
    Server "XXX" is full (32/32)
    Listening for free space...........
    Space now available (31/32)

When a server is full, the library will rapidly ping the server until it detects that there is at least one free slot available, at which point it will immediately attempt to join. Note that if the game is not running when this happens, the game will open before joining. If the server you aim to join is high-demand, launch the game before attempting to join or the time the game spends launching may lose you your free slot!

A general optimisation would be to add "-novid" to your launch options.
