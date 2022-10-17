#ifndef __H_SERVER
#define __H_SERVER

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
#include <dirent.h>
#include <stdio.h>
#include <algorithm>

#include "../utils.hpp"
#include "../constant.hpp"

using namespace std;
using namespace parsers;
using namespace checkers;
using namespace protocols;
using namespace auxiliaries;

/* Contains information about the created groups */
typedef struct grouplist {
    int no_groups; /* The number of created groups */
    char group_no[MAX_NGROUPS][MAX_GID + 1]; /* The GID of each group */
    char group_name[MAX_NGROUPS][MAX_GNAME + 1]; /* The GName of each group */
    char group_mid[MAX_NGROUPS][MAX_MID + 1]; /* The MID of the last message of each group */
} GROUPLIST;

class Server{
    bool m_verbose;
    string m_dsport;
    SOCKET * socketUDP, * socketTCP, * sTCP;

public:
    Server(int argc, char** argv);

private:
    //:::::::::::::: INITIALIZATION/TERMINATION ::::::::::::::://
    void parse_arguments(int argc, char** argv);
    void handle_request(char * request);
    void terminate();

    //::::::::::::::::::::: COMMUNICATION ::::::::::::::::::::://
    void initialize_connection();
    void connectUDP(string port);
    void connectTCP(string port);
    void receive_request();

    //:::::::::::::::: CONDITIONS VALIDATION :::::::::::::::::://
    int validate_user(const char * uid);
    int validate_group(const char * gid, const char * uid);
    int validate_pass(const char * uid, const char * pass);

    //:::::::::::::: FILE/DIRECTORY MANAGEMENT :::::::::::::::://
    int create_dir(char * dirname);
    int delete_dir(char * dirname);
    int delete_file(char * pathname);

    //::::::::::::::::::::: AUXILIARIES ::::::::::::::::::::::://
    void init_groups_dir(GROUPLIST * groups);
    int list_groups_dir(GROUPLIST * list, const char * uid);
    int count_mid(char * gid);
    int count_gid();
    int read_file(char * data, char * pathname, int bytes);
    void print_verbose(SOCKET * s, string request, string uid, string gid);
    string get_clientIPv4(SOCKET * s);
    string get_clientport(SOCKET * s);

    //::::::::::::::::::::::: COMMANDS :::::::::::::::::::::::://
    void reg(string uid, string pass);
    void unregister(string uid, string pass);
    void login(string uid, string pass);
    void logout(string uid, string pass);
    void groups();
    void subscribe(string uid, string gid, string gname);
    void unsubscribe(string uid, string gid);
    void my_groups(string uid);
    void ulist();
    void post();
    void retrieve();
};


#endif
