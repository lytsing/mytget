/*  Mytget - A download accelerator for GNU/Linux
 *  Homepage: http://myget.sf.net
 *  Copyright (C) 2005- xiaosuo
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */


#include <iostream>
#include <getopt.h>
#include <signal.h>

#include "mytget.h"

using namespace std;

void print_help()
{
	cout << "Mytget "VERSION": A download accelerator for GNU/Linux" << endl;
	cout << "Usage: mytget [options]... [URL]..." << endl;
	cout << "Options:" << endl;
	cout << "  -b,  --debug          Show the debug message" << endl;
	cout << "  -c,  --count=num      Set the retry count to [num], no limit when \"0\", the default is \"99\"" << endl;
	cout << "  -d,  --directory=dir  Set the local direcotry to [dir], the default is \".\"" << endl;
	cout << "  -f,  --file=file      Rename the file to [file]" << endl;
	cout << "  -h,  --help           A brief summary of all the options" << endl;
	cout << "  -i,  --interval=num   Set the ftp retry interval to [num] seconds, the default is \"5\"" << endl;
	cout << "  -n,  --number=num     Use [num] connections instead of the default (4)" << endl;
	cout << "  -r,  --referer=URL    Include `Referer: [URL]\' header in HTTP request." << endl;
	cout << "  -t,  --timeout=num    Set the connection timeout to [num] seconds, the default is \"30\"" << endl;
	cout << "  -v,  --version        Show the version of the myget and exit" << endl;
	cout << "  -x,  --proxy=URL      Set the proxy [URL]" << endl;
};

const struct option long_options [] = {
	{"debug", 0, NULL, 'b'},
	{"count", 1, NULL, 'c'},
	{"direcotry", 1, NULL, 'd'},
	{"file", 1, NULL, 'f'},
	{"help", 0, NULL, 'h'},
	{"interval", 1, NULL, 'i'},
	{"number", 1, NULL, 'n'},
	{"referer", 1, NULL, 'r'},
	{"timeout", 1, NULL, 't'},
	{"version", 0, NULL, 'v'},
	{"proxy", 1, NULL, 'x'},
	{NULL, 0, NULL, 0}
};

char short_options [] = "bc:d:f:hi:n:r:t:vx:";

int main(int argc, char **argv)
{
	int ret;
	URL url;
	Downloader downloader;
	Task task;
	Proxy proxy;
	char *ptr = NULL;
	// init_config
	// parse arguments
	// parse url
	// ftp or http ?
	// new download thread

	signal(SIGPIPE, SIG_IGN);
#ifdef HAVE_SSL
	SSL_load_error_strings();
	SSLeay_add_ssl_algorithms();
#endif
	
	while (1) {
		int option_index = 0;

		ret = getopt_long(argc, argv, short_options,
				long_options, &option_index);
		if (ret == -1) break;

		switch (ret) {
			case 'b':
				global_debug = true;
				break;
			case 'c':
				task.tryCount = atoi(optarg);
				break;
			case 'd':
				task.set_local_dir(optarg);
				break;
			case 'f':
				task.set_local_file(optarg);
				break;
			case 'h':
				print_help();
				return 0;
				break;
			case 'i':
				task.retryInterval = atoi(optarg);
				break;
			case 'n':
				task.threadNum = atoi(optarg);
				break;
			case 'r':
				task.set_referer(optarg);
				break;
			case 't':
				task.timeout = atoi(optarg);
				break;
			case 'v':
				cout<<"Mytget "VERSION<<endl;
				return 0;
			case 'x':
				ptr = StrDup(optarg);
				break;
			case '?':
			default:
				print_help();
				return -1;
		}
	}
	
	if (ptr == NULL) {
		ptr = StrDup(getenv("proxy"));
	}
	if (ptr) {
		if (url.set_url(ptr) < 0) {
			delete[] ptr;
			cerr<<"!!!Please check your http_proxy set"<<endl;
			print_help();
			return -1;
		}
		delete[] ptr;
		if (url.get_protocol() != HTTP) {
			cerr<<"!!!The proxy type is not supported"<<endl;
			return -1;
		}
		proxy.set_type(HTTP_PROXY);
		proxy.set_host(url.get_host());
		proxy.set_port(url.get_port());
		proxy.set_user(url.get_user());
		proxy.set_password(url.get_password());
		task.proxy = proxy;
	}

	
	if (optind >= argc) {
		print_help();
		return -1;
	}

	while (optind < argc) {
		if (url.set_url(argv[optind++]) < 0) {
			print_help();
			return -1;
		}
		task.url = url;
		downloader.task = task;
		downloader.run();
	}

	return 0;
};

