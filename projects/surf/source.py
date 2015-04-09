import socket
import struct
import string
import time
import sys
import sqlite3
import webbrowser

class SourceServer(object):
	"""Base class for Valve Source engine game servers"""
	
	def __init__(self, nick, addr, port):
		self.nick = nick
		self.addr = addr
		self.port = port
		
		# Internal variables to determine status
		self._pinged = False
		self._online = bool()
		
		# Variables from ping
		self._header = int()
		self._protocol = int()
		self._name = str()
		self._map = str()
		self._folder = str()
		self._game = str()
		self._gameID = int()
		self._players = int()
		self._max_players = int()
		self._bots = int()
		self._type = str()
		self._env = str()
		self._vis = bool()
		self._vac = bool()
	
	def _ping(self, timeout):
		request_packet = "\xff\xff\xff\xff\x54\x53\x6f\x75\x72\x63\x65\x20\x45" \
							  "\x6e\x67\x69\x6e\x65\x20\x51\x75\x65\x72\x79\x00"
		
		sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
		sock.settimeout(timeout)
		sock.connect((self.addr, self.port))
		sock.sendall(request_packet)
		
		try:
			reply = sock.recv(4096)
		except sock.error as e:
			reply = None
		finally:
			sock.close()
		
		return reply
	
	def ping(self, timeout = 0.35, maxAttempts = 2):
		attempts = 0
		
		while attempts < maxAttempts:
			reply = self._ping(timeout)
			
			if not reply == None:
				break
			
			attempts += 1
		
		if reply == None:
			self._online = False
		else:
			self._online = True
			
			self._header = struct.unpack('B', reply[4])[0]
			self._protocol = int(struct.unpack('B', reply[5])[0])
			
			i = 6
			
			while reply[i] != '\x00':
				i += 1
			
			self._name = reply[6:i]
			i += 1
			j = i
			
			while reply[i] != '\x00':
				i += 1
			
			self._map = reply[j:i].lower()
			i += 1
			j = i
			
			while reply[i] != '\x00':
				i += 1
			
			self._folder = reply[j:i]
			i += 1
			j = i
			
			while reply[i] != '\x00':
				i += 1
			
			self._game = reply[j:i]
			i += 1
			self._gameID = struct.unpack('H', reply[i:i + 2])[0]
			i += 2
			self._players = int(struct.unpack('B', reply[i])[0])
			i += 1
			self._max_players = int(struct.unpack('B', reply[i])[0])
			i += 1
			self._bots = int(struct.unpack('B', reply[i])[0])
			i += 1
			self._type = reply[i]
			i += 1
			self._env = reply[i]
			i += 1
			self._vis = bool(struct.unpack('B', reply[i])[0])
			i += 1
			self._vac = bool(struct.unpack('B', reply[i])[0])
		
		self._pinged = True
	
	def join(self, pingInterval = 0.2):
		self.ping()
		
		if self._online and self._players >= self._max_players:
			sys.stdout.write("Server \"%s\" is full (%d/%d)\nListening for free " \
								  + "space" % (self.nick, self._players,
								  self._max_players))
			
			while self._online and self._players >= self._max_players:
				self.ping()
				sys.stdout.write(".")
				time.sleep(pingInterval)
			
			sys.stdout.write("\n")
			
			if self._online:
				print "Space now available (%d/%d)" % (self._players,
						self._max_players)
				webbrowser.open("steam://connect/%s:%d" % (self.addr, self.port))
			else:
				print "Server \"%s\" has gone offline" % (self.nick,)
		else:
			print "Joining server (%d/%d)" % (self._players, self._max_players)
			webbrowser.open("steam://connect/%s:%d" % (self.addr, self.port))
			# Consider FancyURLopener() if this doesn't work on some platform?
	
	@staticmethod
	def pingAll(servers):
		for server in servers:
			server.ping()
