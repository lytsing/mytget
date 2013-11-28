Mytget 
======================

Mytget is a muti-thread downloader accelerator for GNU/Linux.

It forks from http://myget.sourceforge.net/

Current Features
------------

* Supports HTTP, HTTPS, FTP protocols
* Supports HTTP-GET proxy
* Resumes the aborted downloads
* Supports directory download
* Can accelerates downloading through using muti-connections
* Supports large file( > 4G )


Install in Linux/Mac OS
------------

	$ automake --add-missing
	$ ./configure && make && make install

