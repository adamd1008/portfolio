#!/usr/bin/env python
#
# A Python script to recursively backup a directory and it's sub-directories
# by adding them to a tar.bz2 compressed archive. Also places information on
# all the files into a MySQL database.

import tarfile
import hashlib
import os
import sys
from sys import argv
from datetime import datetime, date, timedelta
import MySQLdb
import _mysql_exceptions

def hashFile(filePath):
	sha1 = hashlib.sha1()
	
	f = open(filePath, 'rb')
	
	try:
		sha1.update(f.read())
	finally:
		f.close()
	
	return sha1.hexdigest()

permit = ['pdf', 'xls', 'doc', '123', 'docx', 'xlsx']
# List of files that will be backed up, regardless of size

exclude = ['mp3', 'm4a', 'wma', 'jpg', 'jpeg', 'png', 'gif', 'mpg', 'mpeg',
			  'mp4', 'wav', 'wmv', 'exe', 'dll', 'cab', 'msi', 'chm', 'html',
			  'css']
# List of files that definitely will NOT get backed up

# Otherwise, files are subject to the size limit below

conn = MySQLdb.connect(user = 'user', passwd = 'password', host = '127.0.0.1',
							  db = 'backupdb')
conn.autocommit(True)

cur = conn.cursor()

query = "SELECT MAX(id) + 1 as max FROM backupinstance"
cur.execute(query)

backupInstance = cur.fetchone()[0]

query = "INSERT INTO backupinstance (`id`, `start`) VALUES (%d, '%s')" \
		  % (backupInstance, datetime.now().strftime('%Y-%m-%d %H:%M:%S'))

sys.stdout.write(query + '\n\n')

cur.execute(query)

tarName = "/path/to/backup_dir/backup-%s.tar.bz2" % \
			 datetime.now().strftime('%y%m%d-%H%M%S')

tar = tarfile.open(tarName, 'w:bz2')

fileSizeLimit = 32 * 1048576

# Error codes:
# 0 = yes, 1 = no (excluded extension), 2 = no (file too big), 3 = no (IOError)
# 4 = tar error
# IOError and tar error appear to be fatal and would end the script prematurely

for backupDir in ('/path/to/network_dir1', '/path/to/network_dir2'):
	for root, dirs, files in os.walk(backupDir):
		for name in files:
			fileStatus = 0
			permitted = False
			
			for x in exclude:
				if name[-len(x):] == x:
					fileStatus = 1
			
			try:
				fullPath = os.path.join(root, name)
				fileStat = os.stat(fullPath)
				
				for x in permit:
					if name[-len(x):] == x:
						permitted = True
				
				if permitted == False and fileStat.st_size > fileSizeLimit:
					fileStatus = 2
				
				if fileStatus == 0:
					fileHash = hashFile(fullPath)
			except IOError as e:
				fileStatus = 3
				thisErrno = e.errno
				thisStrerror = e.strerror
			
			if fileStatus == 0:
				try:
					tar.add(fullPath)
					
					query = "INSERT INTO backupfile (`backupInstanceID`, `path`, " \
							  "`size`, `dateAccessed`, `dateModified`, " \
							  "`sha1`) VALUES (%d, '%s', %d, FROM_UNIXTIME(%s), " \
							  "FROM_UNIXTIME(%s), '%s')" % (backupInstance,
							  conn.escape_string(fullPath), fileStat.st_size,
							  fileStat.st_atime, fileStat.st_mtime, fileHash)
				except Exception as e:
					query = "INSERT INTO backupfile (`backupInstanceID`, `path`, " \
							  "`size`, `dateAccessed`, `dateModified`, " \
							  "`sha1`, `errno`) VALUES (%d, '%s', %d, " \
							  "FROM_UNIXTIME(%s), FROM_UNIXTIME(%s), '%s', 4)" \
							  % (backupInstance, conn.escape_string(fullPath),
							  fileStat.st_size, fileStat.st_atime, fileStat.st_mtime,
							  fileHash)
			else:
				query = "INSERT INTO backupfile (`backupInstanceID`, `path`, " \
						  "`size`, `dateAccessed`, `dateModified`, `errno`) VALUES " \
						  "(%d, '%s', %d, FROM_UNIXTIME(%s), FROM_UNIXTIME(%s), %d)" \
						  % (backupInstance, conn.escape_string(fullPath),
						  fileStat.st_size, fileStat.st_atime, fileStat.st_mtime,
						  fileStatus)
			
			sys.stdout.write(str(fileStatus) + ' ' + fullPath + '\n')
			cur.execute(query)

query = "UPDATE backupinstance SET `end` = '%s' WHERE `id` = %d" \
		  % (datetime.now().strftime('%Y-%m-%d %H:%M:%S'), backupInstance)

sys.stdout.write('###################################################'
					  '#############################\n')

sys.stdout.write(query + '\n')

cur.execute(query)
		
cur.close()
conn.close()

tar.close()

sys.exit(0)
