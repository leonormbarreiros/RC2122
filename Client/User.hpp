#ifndef __H_USER
#define __H_USER

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

#include "../utils.hpp"

using namespace std;
using namespace parsers;
using namespace checkers;
using namespace protocols;
using namespace auxiliaries;

class User {
    string m_dsip, m_dsport;

    SOCKET * socketUDP, * socketTCP;

    string m_uid;
    string m_pass;
    string m_gid;

public:
    User(int argc, char** argv);

private:
    //:::::::::::::: INITIALIZATION/TERMINATION ::::::::::::::://
    void parse_arguments(int argc, char** argv);
    void handle_keyboard_input();
    void terminate();

    //::::::::::::::::::::: COMMUNICATION ::::::::::::::::::::://
    void connectUDP(string ip, string port);
    void connectTCP(string ip, string port);
    bool process_status(string status, string extra, string command);

    //::::::::::::::::::::::: CHECKERS :::::::::::::::::::::::://
    bool check_logged_in(string command);
    bool check_selected(string command);
    bool check_not_logged_in(string command);
    
    //::::::::::::::::::::::: PRINTERS :::::::::::::::::::::::://
    void print_groups(vector<string> ans);

    //::::::::::::::::::::::: COMMANDS :::::::::::::::::::::::://
    void reg(string uid, string pass);
    void unregister(string uid, string pass);
    void login(string uid, string pass);
    void logout();
    void showuid();
    void usr_exit();
    void groups();
    void subscribe(string gid, string gname);
    void unsubscribe(string gid);
    void my_groups();
    void usr_select(string gid);
    void showgid();
    void ulist();
    void post(string text, string fname, int nparams);
    void retrieve(string mid);

};

#endif