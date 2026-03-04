/*  Mytget - A download accelerator for GNU/Linux
 *  Homepage: https://github.com/lytsing/Mytget
 *  Copyright (C) 2005- xiaosuo
 *  Copyright (C) 2013 Lytsing Huang
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
#include <cstdlib>
#include <getopt.h>
#include <signal.h>

#include "mytget.h"

using namespace std;

#ifndef VERSION
#define VERSION "1.13"
#endif

void print_help() {
    cout << "Mytget " VERSION ": A download accelerator for GNU/Linux" << endl;
    cout << "Usage: mytget [options]... [URL]..." << endl;
    cout << "Options:" << endl;
    cout << "  -A,  --user-agent=UA  Set the user agent [UA]" << endl;
    cout << "  -b,  --debug          Show the debug message" << endl;
    cout << "  -c,  --count=num      Set the retry count to [num], no limit when \"0\", the default is \"99\"" << endl;
    cout << "  -d,  --directory=dir  Set the local directory to [dir], the default is \".\"" << endl;
    cout << "  -o,  --output=FILE    Write output to FILE (like curl -o)" << endl;
    cout << "  -H,  --host=host      Modify `Host: [host]\' header in HTTP request." << endl;
    cout << "  -h,  --help           A brief summary of all the options" << endl;
    cout << "  -i,  --interval=num   Set the ftp retry interval to [num] seconds, the default is \"5\"" << endl;
    cout << "  -n,  --number=num     Use [num] connections instead of the default (4)" << endl;
    cout << "  -r,  --referer=URL    Include `Referer: [URL]\' header in HTTP request." << endl;
    cout << "  -t,  --timeout=num    Set the connection timeout to [num] seconds, the default is \"30\"" << endl;
    cout << "  -v,  --version        Show the version of the mytget and exit" << endl;
    cout << "  -x,  --proxy=URL      Set the proxy [URL]" << endl;
};

const struct option long_options[] = {
    {"user-agent", 1,  NULL,  'A'},
    {"debug"     , 0 , NULL , 'b'} ,
    {"count"     , 1 , NULL , 'c'} ,
    {"directory" , 1 , NULL , 'd'} ,
    {"output"    , 1 , NULL , 'o'} ,
    {"help"      , 0 , NULL , 'h'} ,
    {"interval"  , 1 , NULL , 'i'} ,
    {"number"    , 1 , NULL , 'n'} ,
    {"referer"   , 1 , NULL , 'r'} ,
    {"timeout"   , 1 , NULL , 't'} ,
    {"version"   , 0 , NULL , 'v'} ,
    {"proxy"     , 1 , NULL , 'x'} ,
    {"host"      , 1 , NULL , 'H'} ,
    {NULL        , 0 , NULL , 0}
};

char short_options[] = "bc:d:hi:n:o:r:t:vx:H:";

int main(int argc, char **argv) {
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
#  if OPENSSL_VERSION_NUMBER < 0x10100000L
    SSL_load_error_strings();
    SSLeay_add_ssl_algorithms();
#  else
    /*
     * OpenSSL 1.1.0+ does its own library init; use the
     * recommended OPENSSL_init_ssl() entry point instead
     * of the deprecated SSL_load_error_strings() and
     * SSLeay_add_ssl_algorithms().
     */
    OPENSSL_init_ssl(OPENSSL_INIT_LOAD_SSL_STRINGS |
                     OPENSSL_INIT_LOAD_CRYPTO_STRINGS,
                     NULL);
#  endif
#endif

    while (1) {
        int option_index = 0;

        ret = getopt_long(argc, argv, short_options,
                long_options, &option_index);
        if (ret == -1) break;

        switch (ret) {
            case 'A':
                task.set_ua(optarg);
                break;
            case 'b':
                global_debug = true;
                break;
            case 'c': {
                char *endptr = NULL;
                long v = strtol(optarg, &endptr, 10);
                if (optarg[0] == '\0' || *endptr != '\0' || v < 0) {
                    cerr << "Invalid value for -c/--count: " << optarg
                         << " (must be a non-negative integer)" << endl;
                    print_help();
                    return -1;
                }
                task.tryCount = v;
                break;
            }
            case 'd':
                task.set_local_dir(optarg);
                break;
            case 'o':
                task.set_local_file(optarg);
                break;
            case 'H':
                task.set_host(optarg);
                break;
            case 'h':
                print_help();
                return 0;
                break;
            case 'i': {
                char *endptr = NULL;
                long v = strtol(optarg, &endptr, 10);
                if (optarg[0] == '\0' || *endptr != '\0' || v < 0) {
                    cerr << "Invalid value for -i/--interval: " << optarg
                         << " (must be a non-negative integer)" << endl;
                    print_help();
                    return -1;
                }
                task.retryInterval = v;
                break;
            }
            case 'n': {
                char *endptr = NULL;
                long v = strtol(optarg, &endptr, 10);
                if (optarg[0] == '\0' || *endptr != '\0' || v < 0) {
                    cerr << "Invalid value for -n/--number: " << optarg
                         << " (must be a non-negative integer)" << endl;
                    print_help();
                    return -1;
                }
                task.threadNum = v;
                break;
            }
            case 'r':
                task.set_referer(optarg);
                break;
            case 't': {
                char *endptr = NULL;
                long v = strtol(optarg, &endptr, 10);
                if (optarg[0] == '\0' || *endptr != '\0' || v < 0) {
                    cerr << "Invalid value for -t/--timeout: " << optarg
                         << " (must be a non-negative integer)" << endl;
                    print_help();
                    return -1;
                }
                task.timeout = v;
                break;
            }
            case 'v':
                cout << "Mytget " VERSION << endl;
                cout << "Features:" << endl;
                cout << "  Protocols: HTTP";
#ifdef HAVE_SSL
                cout << ", HTTPS";
#endif
                cout << ", FTP" << endl;

                cout << "  SSL: ";
#ifdef HAVE_SSL
                cout << "enabled";
#else
                cout << "disabled";
#endif
                cout << endl;

                cout << "  Build type: ";
#ifdef NDEBUG
                cout << "release";
#else
                cout << "debug";
#endif
                cout << endl;

                cout << "  C++ standard: ";
#if __cplusplus >= 202002L
                cout << "C++20 or later";
#elif __cplusplus >= 201703L
                cout << "C++17";
#elif __cplusplus >= 201402L
                cout << "C++14";
#elif __cplusplus >= 201103L
                cout << "C++11";
#else
                cout << "pre-C++11";
#endif
                cout << endl;

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
        const char *env = getenv("http_proxy");
        if (env == NULL) env = getenv("HTTP_PROXY");
        if (env == NULL) env = getenv("proxy");
        if (env != NULL) ptr = StrDup(env);
    }
    if (ptr) {
        if (url.set_url(ptr) < 0) {
            delete[] ptr;
            cerr << "!!!Please check your http_proxy set" << endl;
            print_help();
            return -1;
        }
        delete[] ptr;
        if (url.get_protocol() != HTTP) {
            cerr << "!!!The proxy type is not supported" << endl;
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
}

