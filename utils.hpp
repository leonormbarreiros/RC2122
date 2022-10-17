#ifndef __H_UTILS
#define __H_UTILS

#include <vector>
#include <iostream>
#include <sstream>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <string.h>
#include <cstring>
#include <string>
#include <fstream>
#include <cstdio>
#include <sys/stat.h>

using namespace std;

namespace parsers{
    bool parse_nparam(int nparams, int expected);
    
    bool parse_uid(const string input);
    bool parse_pass(const string input);

    bool parse_gid(const string input);
    bool parse_gname(const string input);

    bool parse_mid(const string input);

    bool parse_fname(const string input);
    bool parse_fsize(const string input);

    bool parse_text(const string input);
    bool parse_tsize(const string input);

    bool is_number(char input);
    bool is_letter(char input);
}

namespace checkers{
    bool check_nparam(int nparams, int expected, string command);

    bool check_uid(string uid, string command);
    bool check_pass(string pass, string command);

    bool check_gid(string gid, string command);
    bool check_gname(string gname, string command);

    bool check_mid(string mid, string command);

    bool check_fname(string fname, string command);
    bool check_fsize(string fsize, string command);

    bool check_text(string text, string command);
    bool check_tsize(string tsize, string command);
}

typedef struct SOCK{
    char owner[7]; /* Which application uses the socket: USER or SERVER */
    int fd;
    struct addrinfo * res;
    struct sockaddr_in addr;
} SOCKET;

namespace protocols{
    void disconnect(SOCKET * s);

    //:::::::::::::::::::: GENERIC SENDERS :::::::::::::::::::://
    int sendUDP(SOCKET * s, string message);
    int sendTCP(SOCKET * s, string, int nbytes);

    //::::::::::::::::::: GENERIC RECEIVERS ::::::::::::::::::://
    int receiveUDP(SOCKET * s, char * message);
    int receiveTCP(SOCKET * s, char * message, int nbytes);

    //:::::::::::::::::::: UDP AUXILIARIES :::::::::::::::::::://
    int sendstatusUDP(SOCKET * s, string command, string status);
    int sendstatusTCP(SOCKET * s, string command, string status);

    //:::::::::::::::::::: TCP AUXILIARIES :::::::::::::::::::://
    int sendfileTCP(SOCKET * s, FILE * file);
    int receivefileTCP(SOCKET * s, char * fname, int fsize);

    int word_receiveTCP(SOCKET * s, char* answer, int limit);

    int uid_receiveTCP(SOCKET * s, char * uid, string command);

    int gid_receiveTCP(SOCKET * s, char * gid, string command);
    int gname_receiveTCP(SOCKET * s, char * gname, string command);

    int mid_receiveTCP(SOCKET * s, char * mid, string command);

    int fname_receiveTCP(SOCKET * s, char * fname, string command);
    int fsize_receiveTCP(SOCKET * s, char * fsize, string command);
    
    int text_receiveTCP(SOCKET * s, char * text, int tsize, string command, char * last_caracter);
    int tsize_receiveTCP(SOCKET * s, char * tsize, string command);  
}

namespace auxiliaries{
    vector<string> process_string(const string input);
    void handle_error(string program, int type);
}

#endif