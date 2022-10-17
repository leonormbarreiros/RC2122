#include "Server.hpp"

#include <errno.h>
Server::Server(int argc, char** argv){

    m_verbose = false;

    parse_arguments(argc, argv);

    initialize_connection();

    receive_request();

    terminate();

}
//:::::::::::::::: INITIALIZATION/TERMINATION ::::::::::::::::://
/**
 * Parses the arguments used when invoking the Directory Server 
 * (DS).
 * The DS makes available two server applications, both with well
 * known port DSport, one in UDP, to answer configuration messa-
 * ges, and the other in TCP, to answer messaging requests, both
 * originating in the User application.
 * 
 * Usage: ./DS [-p DSport] [-v]
 * . DSport is the well-known port where DS accepts requests. If 
 * it's ommited then it assumes the value 58000+GN where GN is 
 * the group number (12).
 * . if the -v option is set when invoking the program, it opera-
 * tes in verbose mode, meaning that the DS server outputs to the 
 * screen a short description of the received requests (UID, GID)
 * and the IP and port originating those requests
 * 
 * @param argc number of arguments
 * @param argv vector of arguments
 */
void Server::parse_arguments(int argc, char** argv){
    extern char* optarg;
    int max_argc = 1;

    char c;
    while((c = getopt(argc, argv, "p:v")) != -1) {
        switch(c) {
            case 'p':
                m_dsport = optarg;
                max_argc += 2;
                break;
            case 'v':
                m_verbose = true;
                max_argc += 1;
                break;
            default:
                fprintf(stderr, "Usage: %s [-p DSport] [-v]\n", argv[0]);
                exit(EXIT_FAILURE);
        }
    }

    if(m_dsport.empty())
        m_dsport = DSPORT_DEFAULT;

    if(max_argc < argc) {
        fprintf(stderr, "Usage: %s [-p DSport] [-v]\n", argv[0]);
        exit(EXIT_FAILURE);
    }
}

/**
 * Reads each request and parses it in order to execute the cor-
 * responding command.
 */
void Server::handle_request(char * request){
    char command[MAX_INPUT_SIZE] = {'\0'};
    char arg1[MAX_INPUT_SIZE] = {'\0'};
    char arg2[MAX_INPUT_SIZE] = {'\0'};
    char arg3[MAX_INPUT_SIZE] = {'\0'};

    int numTokens = sscanf(request, "%s %s %s %s", command, arg1, arg2, arg3);

    string arg1_str = string(arg1);
    string arg2_str = string(arg2);
    string arg3_str = string(arg3);

    bool err = false;

    if (!strcmp(command, USER_REG_REQUEST)){
        if (!parse_nparam(numTokens, 3)){
err = true;            err = true;
        }
        reg(arg1_str, arg2_str);
    }
    else if (!strcmp(command, USER_UNREGISTER_REQUEST)){
        if (!parse_nparam(numTokens, 3)){
            err = true;
        }
        unregister(arg1_str, arg2_str);
    }
    else if (!strcmp(command, USER_LOGIN_REQUEST)){
        if (!parse_nparam(numTokens, 3)){
            err = true;
        }
        login(arg1_str, arg2_str);
    }
    else if (!strcmp(command, USER_LOGOUT_REQUEST)){
        if (!parse_nparam(numTokens, 3)){
            err = true;
        }
        logout(arg1_str, arg2_str);
    }
    else if (!strcmp(command, USER_GROUPS_REQUEST)){
        if (!parse_nparam(numTokens, 1)){
            err = true;
        }
        groups();
    }
    else if (!strcmp(command, USER_SUBSCRIBE_REQUEST)){
        if (!parse_nparam(numTokens, 4)){
            err = true;
        }
        subscribe(arg1, arg2, arg3);
    }
    else if (!strcmp(command, USER_UNSUBSCRIBE_REQUEST)){
        if (!parse_nparam(numTokens, 3)){
            err = true;
        }
        unsubscribe(arg1, arg2);
    }
    else if (!strcmp(command, USER_MY_GROUPS_REQUEST)){
        if (!parse_nparam(numTokens, 2)){
            err = true;
        }
        my_groups(arg1);
    }

    /* Remember that for the TCP commands we have only received
    the first 4 bytes of the message, which indicate which com-
    mand to execute */
    else if (!strcmp(command, USER_ULIST_REQUEST)){
        ulist();
    }
    else if (!strcmp(command, USER_POST_REQUEST)){
        post();
    }
    else if (!strcmp(command, USER_RETRIEVE_REQUEST)){
        retrieve();
    }

    else{
        sendstatusUDP(socketUDP, "", ERR);
    }

    if (err == true){
        sendstatusUDP(socketUDP, "", ERR);
    }
}

/**
 * Terminates the execution of the DS Server. 
 * Note we are aware this function will not be executed, as the 
 * handle_request function runs in an infinite loop. We created 
 * this function only for good practice reasons.
 */
void Server::terminate(){
    disconnect(socketUDP);
    disconnect(socketTCP);
    exit(EXIT_SUCCESS);
}

//::::::::::::::::::::::: COMMUNICATION ::::::::::::::::::::::://
/**
 * Initializes both UDP and TCP sockets, where the Server will be
 * able to receive requests from clients.
 */
void Server::initialize_connection(){
    connectTCP(m_dsport);
    connectUDP(m_dsport);
}

/**
 * Creates a UDP socket to communicate with clients.
 * 
 * @param port the well-known port (TCP and UDP) where the DS 
 * server accepts requests
 */
void Server::connectUDP(string port){
    int fd;
    struct addrinfo * res;
    int errcode, n;
	struct addrinfo hints;

    fd = socket(AF_INET,SOCK_DGRAM,0);
	if(fd == FAIL){
        fprintf(stderr, "Unable to create socket.\n");
        exit(EXIT_FAILURE);
    }

	memset(&hints, 0, sizeof hints);
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_DGRAM;
	hints.ai_flags = AI_PASSIVE;

	errcode = getaddrinfo(NULL, port.c_str(), &hints, &res);
	if(errcode != SUCCESS){
        fprintf(stderr, "Unable to link to user.\n");
        exit(EXIT_FAILURE);
    } 
    
    n = bind(fd,res->ai_addr,res->ai_addrlen);
	if(n == FAIL){
        fprintf(stderr, "Unable to bind to user.\n");
        exit(EXIT_FAILURE);
    }

    socketUDP = (SOCKET *) malloc(sizeof(SOCKET));
    strcpy(socketUDP->owner, SERVER);
    socketUDP->fd = fd;
    socketUDP->res = res;
}

/**
 * Creates a TCP socket to communicate with clients.
 * 
 * @param port the well-known port (TCP and UDP) where the DS 
 * server accepts requests
 */
void Server::connectTCP(string port){
    int fd;
    int n;
    struct sockaddr_in servaddr;

    fd = socket(AF_INET,SOCK_STREAM,0);
	if(fd == FAIL){
        fprintf(stderr, "Unable to create socket.\n");
        exit(EXIT_FAILURE);
    }

    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(stoi(port));

    n = bind(fd, (struct sockaddr*)&servaddr, sizeof(servaddr));
	if(n == FAIL){
        fprintf(stderr, "Unable to bind.\n");
        exit(EXIT_FAILURE);
    }

    if(listen(fd, 10) == FAIL){
        fprintf(stderr, "Unable to listen.\n");
        exit(EXIT_FAILURE);
    }

    socketTCP = (SOCKET *) malloc(sizeof(SOCKET));
    strcpy(socketTCP->owner, SERVER);
    socketTCP->fd = fd;
    socketTCP->addr = servaddr;
}

/**
 * Always-running function which allows the server to wait cons-
 * tantly for requests from users and respond to them.
 * Each received request is processed once it's received.
 * Only one request is processed at a time. (change this by using
 * fork?)
 */
void Server::receive_request(){
    /* source: https://www.geeksforgeeks.org/tcp-and-udp-server-using-select/ */
    int tcpfd, connfd, udpfd, maxfdp1;
    char bufferUDP[MAX_REQUEST_UDP], bufferTCP[MAX_HEAD_TCP];
    fd_set rset;
    ssize_t n;
    socklen_t len;
    struct sockaddr_in cliaddr;
    void sig_chld(int);
    pid_t childpid;

    tcpfd = socketTCP->fd;
    udpfd = socketUDP->fd;

    FD_ZERO(&rset);

    maxfdp1 = max(tcpfd, udpfd) + 1;
    while(true){
        FD_SET(tcpfd, &rset);
        FD_SET(udpfd, &rset);

        select(maxfdp1, &rset, NULL, NULL, NULL);
        
        /* TCP request */
        if (FD_ISSET(tcpfd, &rset)){
            len = sizeof(cliaddr);
            connfd = accept(tcpfd, (struct sockaddr*)&cliaddr, &len);
            if (connfd == FAIL){
                handle_error(SERVER, SYS_CALL);
            }
            if ((childpid = fork()) == 0){
                close(socketTCP->fd);
                /* For TCP messages, we will only read the first 4 
                bytes, which indicate the command to be executed.
                The rest of each message will be read separately.
                */
                bzero(bufferTCP, MAX_HEAD_TCP);
                
                sTCP = (SOCKET *) malloc(sizeof(SOCKET));
                sTCP->fd = connfd;
                strcpy(sTCP->owner, SERVER);
                n = receiveTCP(sTCP, bufferTCP, MAX_HEAD_TCP);

                if (n == FAIL){
                    handle_error(SERVER, SYS_CALL);
                }
                
                handle_request(bufferTCP);

                exit(0);
            }
            close(connfd);
        }
        
        /* UDP request */
        if (FD_ISSET(udpfd, &rset)){
            bzero(bufferUDP, MAX_REQUEST_UDP);
            
            n = receiveUDP(socketUDP, bufferUDP);
            if (n == FAIL){
                handle_error(SERVER, SYS_CALL);
            }
            
            handle_request(bufferUDP);
        }
    }
}

//:::::::::::::::::: CONDITIONS VALIDATION :::::::::::::::::::://
/**
 * Validates the user existence and if it is logged in
 * 
 * @param UID specifies which user we're validating
 * @return int VALID, INVALID, NOT_LOGIN
 */
int Server::validate_user(const char * uid){
    char pathname[MAX_PATHNAME];

    sprintf(pathname, "%s/%s", USERS, uid);
    DIR * dir = opendir(pathname);
    if(opendir(pathname) == NULL){
        return INVALID;
    }
    closedir(dir);

    sprintf(pathname, "%s/%s/%s_login.txt", USERS, uid, uid);
    FILE * f = fopen(pathname, "r");
    if(f == NULL){
        return NOT_LOGIN;
    }
    fclose(f);
    return VALID;
}

/**
 * Validates the group existence and if the user is subscribed to
 * the group
 * 
 * @param GID specifies which group we're validating
 * @param UID specifies which user we're validating, if NULL it 
 *            means that we just validate the group existence
 * @return int VALID, INVALID, NOT_SUBSCRIBED 
 */
int Server::validate_group(const char * gid, const char * uid){
    char pathname[MAX_PATHNAME];

    sprintf(pathname, "GROUPS/%s/%s_name.txt", gid, gid);
    FILE * f1 = fopen(pathname, "r");
    if(f1 == NULL){
        return INVALID;
    }
    fclose(f1);

    if(uid != NULL){
        sprintf(pathname, "GROUPS/%s/%s.txt", gid, uid);
        FILE * f2 = fopen(pathname, "r");
        if(f2 == NULL){
            return NOT_SUBSCRIBED;
        }
        fclose(f2);
    }

    return VALID;
}

/**
 * Validates if the pass given as parameter matches the password 
 * of the user given by UID.
 * 
 * @param UID the UID parameter
 * @param pass the pass parameter
 * @return int VALID or INVALID
 */
int Server::validate_pass(const char * uid, const char * pass){
    char passfilepath[MAX_PATHNAME] = {'\0'};
    sprintf(passfilepath, "USERS/%s/%s_pass.txt", uid, uid);
    
    char true_pass[MAX_PASS + 1] = {'\0'};
    int n = read_file(true_pass, passfilepath, MAX_PASS);

    if ((n == NO_FILE) || (n != MAX_PASS)){
        return INVALID;
    }

    if (strcmp(true_pass, pass) != SUCCESS){
        return INVALID;
    }

    return VALID;
}

//:::::::::::::::: FILE/DIRECTORY MANAGEMENT :::::::::::::::::://
/**
 * Creates a directory given by dirname
 * 
 * @param dirname the pathname of the directory
 * @return int SUCCESS or FAIL
 */
int Server::create_dir(char * dirname){
    if (mkdir(dirname, 0700) == FAIL){
        fprintf(stderr, "Unable to create %s directory.\n", dirname);
        return FAIL;
    }
    return SUCCESS;
}

/**
 * Deletes a directory whose path is given
 * 
 * @param dirname the pathname of the directory
 * @return int SUCCESS or FAIL
 */
int Server::delete_dir(char * dirname){    
    if (rmdir(dirname) == FAIL){
        fprintf(stderr, "Unable to delete %s directory.\n", dirname);
        return FAIL;
    }
    return SUCCESS;
}

/**
 * Deletes a file by given the path of the file
 * 
 * @param pathname the path to the file
 * @return int SUCCESS, FAIL or NOT_FOUND
 */
int Server::delete_file(char * pathname){

    if(!fopen(pathname, "w")){
        return NOT_FOUND;
    }
    if (remove(pathname) != SUCCESS){
        return FAIL;
    }
    return SUCCESS;
}

//::::::::::::::::::::::: AUXILIARIES ::::::::::::::::::::::::://
void Server::init_groups_dir(GROUPLIST * groups){
    (groups->no_groups) = 0;
    for (int i = 0; i < MAX_NGROUPS; i++){
        bzero(groups->group_no[i], MAX_GID + 1);
        bzero(groups->group_name[i], MAX_GID + 1);
        bzero(groups->group_mid[i], MAX_GID + 1);
    }
}

/**
 * Scans the directories in order to get a list of the groups
 * available or a list of the groups that the user is subscribed.
 * 
 * @param list a pointer of struct GROUPLIST where we put the answer
 * @param uid specifies which user we're validating, if NULL it 
 * means that we want all the groups available
 * @return number of groups available or FAIL
 */
int Server::list_groups_dir(GROUPLIST * groups, const char * uid){
    DIR * d;
    struct dirent * dir;
    d = opendir("GROUPS");
    if (d) {
        /* Read each entry */
        while ((dir = readdir(d)) != NULL) {
            /* If the entry is a directory, we will get the cor-
            responding GID, GName and MID */
            if (dir->d_type == DT_DIR) {
                if (!parse_gid(dir->d_name)) continue;                
                
                /* Get GID */
                char gid[MAX_GID + 1] = {'\0'};
                strncpy(gid, dir->d_name, MAX_GID);

                if ((uid != NULL) && (validate_group(gid, uid) == NOT_SUBSCRIBED)) continue;
                (groups->no_groups)++;

                strcpy(groups->group_no[atoi(gid) - 1], gid);

                /* Get GName */
                char gnamefilepath[MAX_PATHNAME] = {'\0'};
                sprintf(gnamefilepath, "GROUPS/%s/%s_name.txt", gid, gid);

                FILE * gnamefile = fopen(gnamefilepath, "r");
                if (!gnamefile) return FAIL;

                char gname[MAX_GNAME + 1] = {'\0'};
                int nread = fread(gname, 1, MAX_GNAME, gnamefile);
                gname[nread] = '\0';

                fclose(gnamefile);

                strcpy(groups->group_name[atoi(gid) - 1], gname);

                /* Get MID */
                int mid_n = count_mid(gid);

                char mid[MAX_MID + 1] = {'\0'};
                sprintf(mid, "%04d", mid_n);

                strcpy(groups->group_mid[atoi(gid) - 1], mid);
            }
        }
        closedir(d);
    }

    return groups->no_groups;
}

/**
 * Counts the number of messages a certain group, given by GID, 
 * contains.
 * 
 * @param gid the GID parameter
 * @return int the number of messages of that group
 */
int Server::count_mid(char * gid){
    char msgdirpath[MAX_PATHNAME] = {'\0'};
    sprintf(msgdirpath, "GROUPS/%s/MSG", gid);
    int mid = 0;

    DIR * msgdir = opendir(msgdirpath);
    struct dirent * msgdirent;
    if (msgdir){
        while ((msgdirent = readdir(msgdir)) != NULL){
            if (msgdirent->d_type == DT_DIR){
                if (parse_mid(string(msgdirent->d_name))){
                    mid++;
                }
            }
        }
        closedir(msgdir);
    }
    return mid;
}

/**
 * Determins the first GID available (not already created). 
 * 
 * @return int the available GID or FAIL (no more groups can be 
 * created)
 */
int Server::count_gid(){
    int i = 1;
    while (i <= 99){
        char gid[MAX_PATHNAME] = {'\0'};
        sprintf(gid, "%02d", i);
        if (validate_group(gid, NULL) == INVALID){
            return i;
        }
        i++;
    }
    return FAIL;
}

/* Reads the data of a file up to MAX_TEXT, given the path of the 
 * file
 *
 * @param data data in the file
 * @param path the path of the file
 * @param bytes the max number of bytes expected to be read
 * @return FAIL, NO_FILE, or the number of size read
 */
int Server::read_file(char * data, char * path, int bytes){
    FILE * fp = fopen(path, "r");
    if(!fp){
        return NO_FILE;
    }

    int n = fread(data, 1, bytes, fp);

    if(n > 0){
        fclose(fp);
        return n;
    }
    fclose(fp);
    return FAIL;
}

/**
 * If the DS Server is operating in verbose mode, it outputs a 
 * short description of the received requests (UID, GID) and the
 * IP and port originating those requests.
 * 
 * @param s pointer to the socket structure in which the request 
 * was written
 * @param request the name of the request (command we're execu-
 * ting)
 * @param uid the uid associated with that request (if applicable)
 * @param gid the gid associated with that request (if applicable)
 */
void Server::print_verbose(SOCKET * s, string request, string uid, string gid){
    string clientip = get_clientIPv4(s);
    string clientport = get_clientport(s);

    fprintf(stdout, "Request from %s in port %s: ", clientip.c_str(), clientport.c_str());
    fprintf(stdout, "%s ", request.c_str());
    if (!uid.empty()){
        fprintf(stdout, "%s ", uid.c_str());
    }
    if (!gid.empty()){
        fprintf(stdout, "%s", gid.c_str());
    }
    fprintf(stdout, "\n");
}

/**
 * Returns the IP address (in IPv4) of the client sending re-
 * quests through the socket given as argument.
 * 
 * @param s pointer to the socket structure in which the request 
 * was written
 * @return string the IP address (in IPv4) of the client
 */
string Server::get_clientIPv4(SOCKET * s){
    struct in_addr *addr;
    char buffer[INET_ADDRSTRLEN];
    addr =  &( (s->addr).sin_addr);
    
    inet_ntop(AF_INET,addr,buffer,sizeof buffer);

    return string(buffer);
}

/**
 * Returns the port originating the requests coming through the
 * socket given as argument.
 * 
 * @param s pointer to the socket structure in which the request 
 * was written
 * @return string the port where the client is sending requests 
 * from
 */
string Server::get_clientport(SOCKET * s){
    return to_string(ntohs((s->addr).sin_port));
}

//::::::::::::::::::::::::: COMMANDS :::::::::::::::::::::::::://

/**
 * Executes the request corresponding to a reg command. The DS
 * server registers a new user, given by its UID and pass.
 * 
 * @param uid the UID parameter
 * @param pass the pass parameter
 */
void Server::reg(string uid, string pass){
    if (m_verbose) print_verbose(socketUDP, USER_REG, uid, "");
    
    /** 
     * 1. Parameters verification 
     * Expected format: REG UID pass
    */
    if (!parse_uid(uid) || !parse_pass(pass)){
        sendstatusUDP(socketUDP, USER_REG_ANSWER, ERR);
        return;
    }

    /* 2. Check for duplicate user */
    if (validate_user(uid.c_str()) != INVALID){
        sendstatusUDP(socketUDP, USER_REG_ANSWER, DUP);
        return;
    }

    /**
     * 3. Execute request 
     * Steps:
     *  a) create the directory associated with the new user
     *  b) inside that directory, create the file which contains
     * the user's pass
     */

    /* a) Create the directory USERS/UID */
    char directorypath[MAX_PATHNAME] = {'\0'};
    sprintf(directorypath, "USERS/%s", uid.c_str());
    if (create_dir(directorypath) == FAIL){
        sendstatusUDP(socketUDP, USER_REG_ANSWER, NOK);
        return;
    }

    /* b)1) Create the file USERS/UID/UID_pass.txt */
    char passfilepath[MAX_PATHNAME] = {'\0'};
    sprintf(passfilepath, "USERS/%s/%s_pass.txt", uid.c_str(), uid.c_str());
    FILE * passfile = fopen(passfilepath, "w");
    if (!passfile){
        sendstatusUDP(socketUDP, USER_REG_ANSWER, NOK);
        return;
    }

    /* b)2) Write the pass in the USERS/UID/UID_pass.txt file */
    int nwritten = fwrite(pass.c_str(), 1, pass.length(), passfile);
    if (nwritten < (int) pass.length()){
        fclose(passfile);
        delete_file(passfilepath);
        sendstatusUDP(socketUDP, USER_REG_ANSWER, NOK);
        return;
    }

    fclose(passfile);
    sendstatusUDP(socketUDP, USER_REG_ANSWER, OK);
    return;
}

/**
 * Executes the request corresponding to an unregister command.
 * The DS server unregisters a user, given by its UID and pass.
 * 
 * @param uid the UID parameter
 * @param pass the pass parameter
 */
void Server::unregister(string uid, string pass){
    if (m_verbose) print_verbose(socketUDP, USER_UNREGISTER, uid, "");
    
    /** 
     * 1. Parameters verification 
     * Expected format: UNR UID pass
    */
    if (!parse_uid(uid) || !parse_pass(pass)){
        sendstatusUDP(socketUDP, USER_UNREGISTER_ANSWER, ERR);
        return;
    }

    /**
     * 2. Conditions for valid unregister
     *  a) user exists
     *  b) password is correct
     */
    if (validate_user(uid.c_str()) == INVALID){
        sendstatusUDP(socketUDP, USER_UNREGISTER_ANSWER, NOK);
        return;
    }
    if (validate_pass(uid.c_str(), pass.c_str()) == INVALID){
        sendstatusUDP(socketUDP, USER_UNREGISTER_ANSWER, NOK);
        return;
    }

    /** 
     * 3. Execute request 
     * Steps:
     *  a) delete the user's pass file
     *  b) delete the user's login file (if present)
     *  c) delete the user's directory 
     *  d) unsubscribe the user from all groups which it was sub-
     * sribed 
    */
    /* a) Delete the file USERS/UID/UID_pass.txt */
    char passfilepath[MAX_PATHNAME] = {'\0'};
    sprintf(passfilepath, "USERS/%s/%s_pass.txt", uid.c_str(), uid.c_str());
    if (delete_file(passfilepath) != SUCCESS){
        sendstatusUDP(socketUDP, USER_UNREGISTER_ANSWER, NOK);
        return;
    }

    /* b) Delete the file USERS/UID/UID_login.txt (if present) */
    char loginfilepath[MAX_PATHNAME] = {'\0'};
    sprintf(loginfilepath, "USERS/%s/%s_login.txt", uid.c_str(), uid.c_str());
    if (delete_file(loginfilepath) != SUCCESS){
        sendstatusUDP(socketUDP, USER_UNREGISTER_ANSWER, NOK);
        return;
    }

    /* c) Delete the directory USERS/UID */
    char directoryfilepath[MAX_PATHNAME] = {'\0'};
    sprintf(directoryfilepath, "USERS/%s", uid.c_str());
    if (delete_dir(directoryfilepath) != SUCCESS){
        sendstatusUDP(socketUDP, USER_UNREGISTER_ANSWER, NOK);
        return;
    }

    /* d) Delete every GROUPS/GID/UID.txt file */
    for (int i = 0; i < 99; i++){
        char grouppathname[MAX_PATHNAME] = {'\0'};
        sprintf(grouppathname, "GROUPS/%02d/%s.txt", i + 1, uid.c_str());
        if (delete_file(grouppathname) == FAIL){
            sendstatusUDP(socketUDP, USER_UNREGISTER_ANSWER, NOK);
            return;
        }
    }

    sendstatusUDP(socketUDP, USER_UNREGISTER_ANSWER, OK);
}

/**
 * Executes the request corresponding to an login command. The DS
 * server logs in a user, given by its UID and pass.
 * 
 * @param uid the UID parameter
 * @param pass the pass parameter
 */
void Server::login(string uid, string pass){
    if (m_verbose) print_verbose(socketUDP, USER_LOGIN, uid, "");

    /** 
     * 1. Parameters verification 
     * Expected format: LOG UID pass
    */
    if (!parse_uid(uid) || !parse_pass(pass)){
        sendstatusUDP(socketUDP, USER_LOGIN_ANSWER, ERR);
        return;
    }

    /**
     * 2. Conditions for valid login
     *  a) user exists
     *  b) password is correct
     */
    if (validate_user(uid.c_str()) == INVALID){
        sendstatusUDP(socketUDP, USER_LOGIN_ANSWER, NOK);
        return;
    }

    if (validate_pass(uid.c_str(), pass.c_str()) == INVALID){
        sendstatusUDP(socketUDP, USER_LOGIN_ANSWER, NOK);
        return;
    }

    /* 3. Execute request, by creating the USERS/UID/UID_login.txt file */
    char loginfilepath[MAX_PATHNAME] = {'\0'};
    sprintf(loginfilepath, "USERS/%s/%s_login.txt", uid.c_str(), uid.c_str());
    FILE * loginfile = fopen(loginfilepath, "w");
    if (!loginfile){
        sendstatusUDP(socketUDP, USER_LOGIN_ANSWER, NOK);
        return;
    }

    sendstatusUDP(socketUDP, USER_LOGIN_ANSWER, OK);
}

/**
 * Executes the request corresponding to an logout command. The 
 * DS server logs out a user, given by its UID and pass.
 * 
 * @param uid the UID parameter
 * @param pass the pass parameter
 */
void Server::logout(string uid, string pass){
    if (m_verbose) print_verbose(socketUDP, USER_LOGOUT, uid, "");

    /** 
     * 1. Parameters verification 
     * Expected format: OUT UID pass
    */
    if (!parse_uid(uid) || !parse_pass(pass)){
        sendstatusUDP(socketUDP, USER_LOGOUT_ANSWER, ERR);
        return;
    }

    /**
     * 2. Conditions for valid logout
     *  a) user exists
     *  b) password is correct
     *  c) user is logged in
     */
    /* a) Make sure the directory USERS/UID exists */
    if (validate_user(uid.c_str()) == INVALID){
        sendstatusUDP(socketUDP, USER_LOGOUT_ANSWER, NOK);
        return;
    }

    if (validate_pass(uid.c_str(), pass.c_str()) == INVALID){
        sendstatusUDP(socketUDP, USER_LOGOUT_ANSWER, NOK);
        return;
    }

    /*  2c) Make sure the file USERS/UID/UID_login.txt exists 
        simultaneously with
        3. Execute request, by deleting the USERS/UID/UID_login.txt file */
    char loginfilepath[MAX_PATHNAME] = {'\0'};
    sprintf(loginfilepath, "USERS/%s/%s_login.txt", uid.c_str(), uid.c_str());
    int status = delete_file(loginfilepath);
    if (status != SUCCESS){
        sendstatusUDP(socketUDP, USER_LOGOUT_ANSWER, NOK);
    }

    sendstatusUDP(socketUDP, USER_LOGOUT_ANSWER, OK);
}

/**
 * Executes the request corresponding to the groups command.
 * The DS server sends the information of the available groups.
 */
void Server::groups(){
    if (m_verbose) print_verbose(socketUDP, USER_GROUPS, "", "");
        
    /**
     * 1. Execute request; send answer
     * Format: RGl N[ GID GName MID]*
     */

    GROUPLIST * groups = (GROUPLIST * )malloc(sizeof(GROUPLIST));
    
    init_groups_dir(groups);
    int N = list_groups_dir(groups, NULL);

    string answer = string(USER_GROUPS_ANSWER) + " " + to_string(N); 

    int ndone = 0;
    while (ndone < N){
        for (int i = 0; i < MAX_NGROUPS; i++){
            if (atoi(groups->group_no[i]) != 0){
                ndone++;
                answer += " " + string(groups->group_no[i]) + " " + string(groups->group_name[i]) + " " + string(groups->group_mid[i]);
            }
        }
    }
    free(groups);
    answer += "\n";
    sendUDP(socketUDP, answer);
}

/**
 * Executes the request corresponding to the subscribe command.
 * The DS server subscribes a user, given by its UID, to a cer-
 * tain group, given by its GID and GName.
 * 
 * @param uid the UID parameter
 * @param gid the GID parameter
 * @param gname the GName parameter
 */
void Server::subscribe(string uid, string gid, string gname){
    if (m_verbose) print_verbose(socketUDP, USER_SUBSCRIBE, uid, gid);
    string answer;

    /** 
     * 1. Parameters verification 
     * Expected format: GSR UID GID GName
    */
    if(!parse_uid(uid) || !validate_user(uid.c_str())){
        sendstatusUDP(socketUDP, USER_SUBSCRIBE_ANSWER, E_USR);
        return;
    }
    else if(!parse_gid(gid)){
        sendstatusUDP(socketUDP, USER_SUBSCRIBE_ANSWER, E_GRP);
        return;
    }
    else if(!parse_gname(gname)){
        sendstatusUDP(socketUDP, USER_SUBSCRIBE_ANSWER, E_GNAME);
        return;
    }

    /* In the case the number of groups have reached the maximum number allowed */
    int group_no = count_gid();
    if (group_no == FAIL){
        sendstatusUDP(socketUDP, USER_SUBSCRIBE_ANSWER, E_FULL);
        return;
    }
    
    /**
     * 2. Execute request, by subscribing the user to the given 
     * group
     * Possible scenarios:
     *  a) GID is 00, therefore we create a new group, with GID 
     * group_no
     *  b) GID is a valid GID and we check for the existence of 
     * the group and subscribe the user to it
     */
    /* Case a) */
    if (gid == "00"){
        /* 1) Create the directory GROUPS/GID */
        char dirname[MAX_DIRNAME];
        int new_gid = group_no;
        sprintf(dirname, "GROUPS/%02d", new_gid);

        if(create_dir(dirname) == FAIL){
            sendstatusUDP(socketUDP, USER_SUBSCRIBE_ANSWER, NOK);
            return;
        }

        /* 2) Create the file GID_name.txt */
        FILE* gname_file;
        char pathname[MAX_PATHNAME];

        sprintf(pathname, "GROUPS/%02d/%02d_name.txt", new_gid, new_gid);

        if (!(gname_file = fopen(pathname, "w"))){
            sendstatusUDP(socketUDP, USER_SUBSCRIBE_ANSWER, NOK);
            return;
        }

        if (fwrite(gname.c_str(), 1, gname.length(), gname_file) < gname.length()){
            remove(pathname);
            fclose(gname_file);
            sendstatusUDP(socketUDP, USER_SUBSCRIBE_ANSWER, NOK);
            return;
        }

        fclose(gname_file);

        /* 3) Create the file UID.txt */
        FILE* uid_file;

        sprintf(pathname, "GROUPS/%02d/%s.txt", new_gid, uid.c_str());

        if (!(uid_file = fopen(pathname, "w"))){
            sendstatusUDP(socketUDP, USER_SUBSCRIBE_ANSWER, NOK);
            return;
        }

        if (fwrite(uid.c_str(), 1, uid.length(), uid_file) < uid.length()){
            remove(pathname);
            fclose(uid_file);
            sendstatusUDP(socketUDP, USER_SUBSCRIBE_ANSWER, NOK);
            return;
        }

        fclose(uid_file);

        /* 4) Create the directory GROUPS/GID/MSG */
        sprintf(dirname, "GROUPS/%02d/MSG", new_gid);

        if(create_dir(dirname) == FAIL){
            sendstatusUDP(socketUDP, USER_SUBSCRIBE_ANSWER, NOK);
            return;
        }
        /**
         * 3. Send answer 
         * Format: RGS NEW GID
         */
        char aux[3];
        sprintf(aux, "%02d", new_gid);
        answer = string(USER_SUBSCRIBE_ANSWER) + " NEW " + string(aux) + "\n";

        if(sendUDP(socketUDP, answer) == FAIL){
            return;
        }
    }
    /* Case b) */
    else{
        /* 1) Check if the group exists */
        if (validate_group(gid.c_str(), NULL) == INVALID){
            sendstatusUDP(socketUDP, USER_SUBSCRIBE_ANSWER, E_GRP);
            return;
        }

        /* 2) Check if the GName given as argument matches the 
        actual name of the group */
        char gnamepath[MAX_PATHNAME] = {'\0'};
        sprintf(gnamepath, "GROUPS/%s/%s_name.txt", gid.c_str(), gid.c_str());
        char true_gname[MAX_GNAME + 1] = {'\0'};
        int n = read_file(true_gname, gnamepath, MAX_GNAME);
        true_gname[n] = '\0';

        if (strcmp(true_gname, gname.c_str()) != SUCCESS){
            sendstatusUDP(socketUDP, USER_SUBSCRIBE_ANSWER, E_GNAME);
            return;
        }

        /* 3) Create the file UID.txt */
        FILE* uid_file;
        char pathname[MAX_PATHNAME];

        sprintf(pathname, "GROUPS/%s/%s.txt", gid.c_str(), uid.c_str());

        if(!(uid_file = fopen(pathname, "w"))){
            sendstatusUDP(socketUDP, USER_SUBSCRIBE_ANSWER, NOK);
            return;
        }

        /**
         * 3. Send request 
         * Expected format: RGS OK
         */

        fclose(uid_file);
        sendstatusUDP(socketUDP, USER_SUBSCRIBE_ANSWER, OK);
        return;
    }
}

/**
 * Executes the request corresponding to a unsubscribe command.
 * The DS server unsubscribes a user, given by its UID, from a 
 * certain group, given by its GID and GName.
 * 
 * @param uid the UID parameter
 * @param gid the gid parameter
 */
void Server::unsubscribe(string uid, string gid){
    if (m_verbose) print_verbose(socketUDP, USER_UNSUBSCRIBE, uid, gid);

    /* 
     * GUR UID GID
     * RGU status
     * status = {OK, E_USR, E_GRP, NOK}
     */

    /* 1. Parameters verification */
    if (!check_uid(uid, USER_UNSUBSCRIBE)) {
        sendstatusUDP(socketUDP, USER_UNSUBSCRIBE_ANSWER, E_USR);
        return;
    }
    if (!check_gid(gid, USER_UNSUBSCRIBE)) {
        sendstatusUDP(socketUDP, USER_UNSUBSCRIBE_ANSWER, E_GRP);
        return;
    }

    /* 2. Conditions for valid unsubscribe verification 
     * Steps:
     *  a) validate user 
     *      1) exist? -> E_USR
     *      2) logged in? -> NOK
     *  b) validate group
     *      1) exist? -> E_GRP
     */
    switch(validate_user(uid.c_str())){
        case INVALID:
            sendstatusUDP(socketUDP, USER_UNSUBSCRIBE_ANSWER, E_USR);
            return;
        case NOT_LOGIN:
            sendstatusUDP(socketUDP, USER_UNSUBSCRIBE_ANSWER, NOK);
            return;
    }

    switch(validate_group(gid.c_str(), uid.c_str())){
        case INVALID:
            sendstatusUDP(socketUDP, USER_UNSUBSCRIBE_ANSWER, E_GRP);
            return;
        case NOT_SUBSCRIBED:
            return;
    }
    /**
     * 3. Execute request 
     * Verify if the user is subscribed to the group.
     * If True then delete GROUPS/GID/UID.txt
     * else ignore
     */
    char pathname[MAX_PATHNAME];
    sprintf(pathname, "%s/%s/%s.txt", GROUPS, gid.c_str(), uid.c_str());
    switch(delete_file(pathname)){
        case FAIL:
            sendstatusUDP(socketUDP, USER_UNSUBSCRIBE_ANSWER, NOK);
            return;
        case NOT_FOUND:
            handle_error(SERVER, OTHER);
            return;
    }
    sendstatusUDP(socketUDP, USER_UNSUBSCRIBE_ANSWER, OK);
}

/**
 * Executes the request corresponding to a my_groups command.
 * The DS server sends the informations of subscribed groups
 * by the user, given by its UID
 * 
 * @param uid the UID parameter
 */
void Server::my_groups(string uid){
    if (m_verbose) print_verbose(socketUDP, USER_MY_GROUPS, uid, "");

    /* 1. Parameters verification */
    if (!parse_uid(uid)){
        sendstatusUDP(socketUDP, USER_MY_GROUPS_ANSWER, E_USR);
        return;
    }

    /**
     * 2. Conditions for valid my_groups verification: the user
     * needs to exist and be logged in.
     */
    if(validate_user(uid.c_str()) != VALID){
        sendstatusUDP(socketUDP, USER_MY_GROUPS_ANSWER, E_USR);
        return;
    }

    /**
     * 3. Execute request; send answer
     * Format: RGM N[ GID GName MID]*
     */

    GROUPLIST * groups = (GROUPLIST * )malloc(sizeof(GROUPLIST));
    init_groups_dir(groups);

    int N = list_groups_dir(groups, uid.c_str());

    string answer = string(USER_MY_GROUPS_ANSWER) + " " + to_string(N); 

    int ndone = 0;
    while (ndone < N){
        for (int i = 0; i < MAX_NGROUPS; i++){
            if (atoi(groups->group_no[i]) != 0){
                ndone++;
                answer += " " + string(groups->group_no[i]) + " " + string(groups->group_name[i]) + " " + string(groups->group_mid[i]);
            }
        }
    }
    
    free(groups);

    answer += "\n";

    sendUDP(socketUDP, answer);
}

/**
 * Executes the request corresponding to an ulist command.
 * The DS server sends the information of the users subscribed to
 * a group, given by its GID
 */
void Server::ulist(){
    /* 1. Read the rest of the request: GID */
    char gid[MAX_GID + 1] = {'\0'};
    int res = gid_receiveTCP(sTCP, gid, USER_ULIST);
    if (res == FAIL) return;
    if (!res){
        sendstatusTCP(sTCP, USER_ULIST_ANSWER, NOK);
        disconnect(sTCP);
        return;
    }
    gid[MAX_GID] = '\0';

    /* 2. Parameters verification */
    if (validate_group(gid, NULL) == INVALID){
        sendstatusTCP(sTCP, USER_ULIST_ANSWER, NOK);
        disconnect(sTCP);
        return;
    }

    if (m_verbose) print_verbose(socketUDP, USER_ULIST, "", string(gid));

    /**
     * 2. Construct and send answer. We only need the consider 
     * the case when status is OK, because if it isn't we simply 
     * need to call the sendstatusTCP function
     * Format: RUL status[ GName[ UID]*]
     */
    string buffer = string(USER_ULIST_ANSWER) + " " + string(OK) + " "; 

    /* 2a) Get GName and add to answer */
    char gnamefilepath[MAX_PATHNAME] = {'\0'};
    sprintf(gnamefilepath, "GROUPS/%s/%s_name.txt", gid, gid);

    FILE * gnamefile = fopen(gnamefilepath, "r");
    if (!gnamefile){
        sendstatusTCP(sTCP, USER_ULIST_ANSWER, NOK);
        disconnect(sTCP);
        return;
    }
    char gname[MAX_GNAME + 1] = {'\0'};
    fread(gname, 1, MAX_GNAME, gnamefile);
    if (!parse_gname(string(gname))){
        sendstatusTCP(sTCP, USER_ULIST_ANSWER, NOK);
        disconnect(sTCP);
        return;
    }

    buffer += string(gname);

    if (sendTCP(sTCP, buffer, buffer.size()) == FAIL){
        disconnect(sTCP);
        return;
    }

    /* 2b) Get each subscriber of the group and add to answer */    
    /* Get the name of the GName file */
    char gnamepath[MAX_PATHNAME] = {'\0'};
    sprintf(gnamepath, "%s_name.txt", gid);
    
    /* Get the name of the group directory */
    char pathname[MAX_PATHNAME] = {'\0'};
    sprintf(pathname, "GROUPS/%s", gid);
    DIR * d;
    struct dirent * dir;
    d = opendir(pathname);
    if (d) {
        /* Read each entry */
        while ((dir = readdir(d)) != NULL) {
            /* If that directory is a file, we will see if cor-
            responds to a subscribed user */
            if (dir->d_type == DT_REG) {
                char fname[MAX_PATHNAME + 1] = {'\0'};
                strcpy(fname, dir->d_name);
                                
                /* If it corresponds to a subscribed user, send 
                its respective UID */
                if (strcmp(gnamepath, fname) != SUCCESS){
                    char uid[MAX_UID + 1] = {'\0'};
                    strncpy(uid, fname, MAX_UID);
                    uid[MAX_UID] = '\0';

                    if (!parse_uid(string(uid))) continue;

                    string ans_uid = " " + string(uid);

                    if (sendTCP(sTCP, ans_uid, ans_uid.size()) == FAIL){
                        disconnect(sTCP);
                        return;
                    }
                }
            }
        }
        closedir(d);
    }
    /* Need to add the '\n' at the end. Note we don't check if 
    this call failed. This is because whatever the result, the 
    function will return. The specific user who needed the action 
    may be harmed, but the server will carry on */
    sendTCP(sTCP, "\n\0", 2);
    disconnect(sTCP);
}

/**
 * Executes the request corresponding to an post command.
 * The DS server creates the files and saves the information related
 * to the message being posted by the User.
 */
void Server::post(){
    /** 
     * 1. Receiving parameters: UID GID Tsize text
     */
    /* Get UID */
    char uid[MAX_UID + 1] = {'\0'};
    int res1 = uid_receiveTCP(sTCP, uid, USER_POST);
    if (res1 == FAIL) return;
    if (!res1){
        sendstatusTCP(sTCP, USER_POST_ANSWER, NOK);
        disconnect(sTCP);
        return;
    }
    uid[MAX_UID] = '\0';

    /* Get GID */
    char gid[MAX_GID + 1] = {'\0'};
    int res2 = gid_receiveTCP(sTCP, gid, USER_POST);
    if (res2 == FAIL) return;
    if (!res2){
        sendstatusTCP(sTCP, USER_POST_ANSWER, NOK);
        disconnect(sTCP);
        return;
    }
    gid[MAX_GID] = '\0';

    if (m_verbose) print_verbose(sTCP, USER_POST, string(uid), string(gid));

    /* Get Tsize */
    char tsize[MAX_TSIZE + 1] = {'\0'};
    int res3 = tsize_receiveTCP(sTCP, tsize, USER_POST);
    if (res3 == FAIL) return;
    if (!res3){
        sendstatusTCP(sTCP, USER_POST_ANSWER, NOK);
        disconnect(sTCP);
        return;
    }

    /* Get text */
    char text[MAX_TEXT + 1] = {'\0'};
    char last_caracter = '\0';
    int res4 = text_receiveTCP(sTCP, text, stoi(string(tsize)), USER_POST, &last_caracter);
    if (res4 == FAIL) return;
    if (!res4){
        sendstatusTCP(sTCP, USER_POST_ANSWER, NOK);
        disconnect(sTCP);
        return;
    }

    /**
     * 2. Execute request Part 1
     * Steps:
     * a) Determine the new MID
     * b) Create "GROUPS/GID/MSG/MID" directory
     * c) Create "A U T H O R.txt" file in GROUPS/GID/MSG/MID
     * d) Create "T E X T.txt" file in GROUPS/GID/MSG/MID
     * e) Create "F N A M E.txt" file in GROUPS/GID/MSG/MID
     * 
     */

    /* 3.a) Determine the new MID */

    int mid_n = count_mid(gid) + 1;
    char mid[MAX_MID + 1] = {'\0'};
    sprintf(mid, "%04d", mid_n);
    mid[MAX_MID] = '\0';

    /* 3.b) Create "GROUPS/GID/MSG/MID" directory */
    char dirname[MAX_DIRNAME];
    sprintf(dirname, "GROUPS/%2s/MSG/%4s", gid, mid);
    if (create_dir(dirname) == FAIL){
        sendstatusTCP(sTCP, USER_POST_ANSWER, NOK);
        disconnect(sTCP);
        return;
    }

    /* 3.c) Create "A U T H O R.txt" file in GROUPS/GID/MSG/MID */
    FILE* author_file;
    char pathname[MAX_PATHNAME];

    sprintf(pathname, "GROUPS/%2s/MSG/%4s/A U T H O R.txt", gid, mid);
    if (!(author_file = fopen(pathname, "w"))){
        sendstatusTCP(sTCP, USER_POST_ANSWER, NOK);
        disconnect(sTCP);
        return;
    }

    if (fwrite(uid, 1, strlen(uid), author_file) < string(uid).length()){
        remove(pathname);
        fclose(author_file);
        sendstatusTCP(sTCP, USER_POST_ANSWER, NOK);
        disconnect(sTCP);
        return;
    }
    fclose(author_file);

    /* 3.d) Create "T E X T.txt" file in GROUPS/GID/MSG/MID */
    FILE* text_file;

    sprintf(pathname, "GROUPS/%2s/MSG/%4s/T E X T.txt", gid, mid);
    if (!(text_file = fopen(pathname, "w"))){
        sendstatusTCP(sTCP, USER_POST_ANSWER, NOK);
        disconnect(sTCP);
        return;
    }

    if (fwrite(text, 1, strlen(text), text_file) < string(text).length()){
        remove(pathname);
        fclose(text_file);
        sendstatusTCP(sTCP, USER_POST_ANSWER, NOK);
        disconnect(sTCP);
        return;
    }

    fclose(text_file);

    /**
     * 4. Execute request Part 2
     * Steps:
     * a) Determine if a file was sent.
     */

    /* A file was not sent */
    if(last_caracter == '\n'){
        sendstatusTCP(sTCP, USER_POST_ANSWER, mid);
    }
    /* A file was sent */
    else if(last_caracter == ' '){
        /** 
         * 2. Receiving parameters: Fname Fsize data
        */

        /* Receiving fname */
        char fname[MAX_FNAME + 1] = {'\0'};
        int res5 = fname_receiveTCP(sTCP, fname, USER_POST);
        if (res5 == FAIL) return;
        if (!res5){
            sendstatusTCP(sTCP, USER_POST_ANSWER, NOK);
            disconnect(sTCP);
            return;
        }

        /* Receiving fsize */
        char fsize[MAX_FSIZE + 1] = {'\0'};
        int res6 = fsize_receiveTCP(sTCP, fsize, USER_POST);
        if (res6 == FAIL) return;
        if (!res6){
            sendstatusTCP(sTCP, USER_POST_ANSWER, NOK);
            disconnect(sTCP);
            return;
        }

        /* Receiving data and creating a file with it in the pathname below */
        sprintf(pathname, "GROUPS/%2s/MSG/%4s/%s", gid, mid, fname);
        if(receivefileTCP(sTCP, pathname, atoi(fsize))==FAIL){
            sendstatusTCP(sTCP, USER_POST_ANSWER, NOK);
            disconnect(sTCP);
            return;
        }
        
        /* Create "F N A M E.txt" file in GROUPS/GID/MSG/MID */
        FILE* fname_file;

        sprintf(pathname, "GROUPS/%s/MSG/%s/F N A M E.txt", gid, mid);
        if (!(fname_file = fopen(pathname, "w"))){
            sendstatusTCP(sTCP, USER_POST_ANSWER, NOK);
            disconnect(sTCP);
            return;
        }

        if (fwrite(fname, 1, strlen(fname), fname_file) < string(fname).length()){
            remove(pathname);
            fclose(fname_file);
            sendstatusTCP(sTCP, USER_POST_ANSWER, NOK);
            disconnect(sTCP);
            return;
        }
        fclose(fname_file);

        sendstatusTCP(sTCP, USER_POST_ANSWER, mid);
    }
    else{
        sendstatusTCP(sTCP, USER_POST_ANSWER, NOK);
    }
    
    disconnect(sTCP);
}

/**
 * @brief 
 * 
 */
void Server::retrieve(){
    /** 
     * 1. Receive and verify parameters: UID GID MID
     */
    /* Get UID */
    char uid[MAX_UID + 1] = {'\0'};
    int res1 = uid_receiveTCP(sTCP, uid, USER_RETRIEVE);
    if (res1 == FAIL) return;
    if (!res1){
        sendstatusTCP(sTCP, USER_RETRIEVE_ANSWER, NOK);
        disconnect(sTCP);
        return;
    }
    uid[MAX_UID] = '\0'; 

    /* Get GID */
    char gid[MAX_GID + 1] = {'\0'};
    int res2 = gid_receiveTCP(sTCP, gid, USER_RETRIEVE);
    if (res2 == FAIL) return;
    if (!res2){
        sendstatusTCP(sTCP, USER_RETRIEVE_ANSWER, NOK);
        disconnect(sTCP);
        return;
    }
    gid[MAX_GID] = '\0'; 

    if (m_verbose) print_verbose(sTCP, USER_RETRIEVE, string(uid), string(gid));

    /* Get MID */
    char mid[MAX_MID + 1] = {'\0'};
    int res3 = mid_receiveTCP(sTCP, mid, USER_RETRIEVE);
    if (res3 == FAIL) return;
    if (!res3){
        sendstatusTCP(sTCP, USER_RETRIEVE_ANSWER, NOK);
        disconnect(sTCP);
        return;
    }
    mid[MAX_MID] = '\0'; 

    /* 2. Conditions for valid retrieve verification 
     * Steps:
     * a) validate user (exists and is logged in)
     * b) valid gid (exists and the user subscribed to the group)
     */
    if(validate_user(uid) != VALID){
        sendstatusTCP(sTCP, USER_RETRIEVE_ANSWER, NOK);
        disconnect(sTCP);
        return;
    }
    
    if(validate_group(gid, uid) != VALID){
        sendstatusTCP(sTCP, USER_RETRIEVE_ANSWER, NOK);
        disconnect(sTCP);
        return;
    }

    /**
     * 3. Execute request
     * Steps:
     *  a) Get and send N (number of messages retrieved)
     *  b) For each message:
     *      1) Get MID
     *      2) Get UID
     *      3) Get Tsize and text
     *      4) Send MID UID Tsize text
     *      5) Get and send Fname Fsize
     *      6) Send data
     */ 
    /* a) Calculate and send N */
    int group_mid = count_mid(gid);
    int asked_mid = stoi(mid);
    int N = group_mid - asked_mid + 1;

    if (N > 20){
        N = asked_mid + 19;
    }
    if (N <= 0){
        sendstatusTCP(sTCP, USER_RETRIEVE_ANSWER, EOF_);
        disconnect(sTCP);
        return;
    }

    string buffer = string(USER_RETRIEVE_ANSWER) + " " + string(OK) + " " + to_string(N);

    if (sendTCP(sTCP, buffer, buffer.size()) == FAIL){
        disconnect(sTCP);
        return;
    }

    /* b) Send each message */
    for (int i = 0; i < N; i++){
        char path[MAX_PATHNAME] = {'\0'};

        /* 1) MID */
        char messageid[MAX_MID + 1] = {'\0'};
        sprintf(messageid, "%04d", i + asked_mid);
        messageid[MAX_MID] = '\0';

        /* 2) UID */
        sprintf(path, "GROUPS/%s/MSG/%s/A U T H O R.txt", gid, messageid);
        char userid[MAX_UID + 1] = {'\0'};
        if(read_file(userid, path, MAX_UID) != MAX_UID){
            disconnect(sTCP);
            return;
        }
        userid[MAX_UID] = '\0';
        
        /* 3) Tsize and text */
        char text[MAX_TEXT + 1] = {'\0'};
        int tsize = 0;

        sprintf(path, "GROUPS/%s/MSG/%s/T E X T.txt", gid, messageid);
        if((tsize = read_file(text, path, MAX_TEXT)) == FAIL){
            disconnect(sTCP);
            return;
        }
        text[tsize] = '\0';

        /* 4) send MID UID Tsize text */
        buffer = " " + string(messageid) + " " + string(userid) + " " + to_string(tsize) + " " + string(text);

        /* Check if there's a file */
        sprintf(path, "GROUPS/%s/MSG/%s/F N A M E.txt", gid, messageid);
        char fname[MAX_FNAME + 1] = {'\0'};

        int n = read_file(fname, path, MAX_FNAME);
        switch(n){
            case FAIL:
                disconnect(sTCP);
                return;
            case NO_FILE:
                if(sendTCP(sTCP, buffer, buffer.size()) == FAIL){
                    disconnect(sTCP);
                    return;
                }
                continue;
        }

        /* 5) Get and send " / Fname Fsize " */
        sprintf(path, "GROUPS/%s/MSG/%04d/%s", gid, i + asked_mid, fname);
        FILE * file = fopen(path, "r");
        fseek(file, 0, SEEK_END);
        int fsize = ftell(file);
        fseek(file, 0, SEEK_SET);

        buffer += " / " + string(fname) + " " + to_string(fsize) + " ";
        if(sendTCP(sTCP, buffer, buffer.length()) == FAIL){
            disconnect(sTCP);
            return;
        }

        /* 6) Send data */
        if(sendfileTCP(sTCP, file) == FAIL){
            fclose(file);
            disconnect(sTCP);
            return;
        }
        fclose(file);
    }

    if(sendTCP(sTCP, "\n", 1) == FAIL){
        disconnect(sTCP);
        return;
    }
    disconnect(sTCP);
}

//::::::::::::::::::::::::::: MAIN :::::::::::::::::::::::::::://

int main(int argc, char** argv) {
    //Program will run in Server's constructor
    Server(argc, argv);
    return 0;
}