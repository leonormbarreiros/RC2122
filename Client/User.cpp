#include "User.hpp"
#include "../constant.hpp"
#include "../utils.hpp"

User::User(int argc, char** argv){
    m_uid = "";
    m_pass = "";
    m_gid = "";

    parse_arguments(argc, argv);

    connectUDP(m_dsip, m_dsport);

    handle_keyboard_input();
}

//:::::::::::::::: INITIALIZATION/TERMINATION ::::::::::::::::://
/**
 * Parses the arguments used when invoking the user application.
 * Usage: ./user [-n DSIP] [-p DSport]
 * . DSIP is the IP address of the machine where DS runs. If it's
 * ommited then DS runs on the same machine.
 * . DSport is the well-known port where DS accepts requests. If 
 * it's ommited then it assumes the value 58000+GN where GN is 
 * the group number (12).
 * 
 * @param argc number of arguments
 * @param argv vector of arguments
 */
void User::parse_arguments(int argc, char** argv) {
    extern char* optarg;
    int max_argc = 1;

    char c;
    while((c = getopt(argc, argv, "n:p:")) != -1) {
        switch(c) {
            case 'n':
                m_dsip = optarg;
                max_argc += 2;
                break;
            case 'p':
                m_dsport = optarg;
                max_argc += 2;
                break;
            default:
                fprintf(stderr, "Usage: %s [-n DSIP] [-p DSport]\n", argv[0]);
                exit(EXIT_FAILURE);
        }
    }

    if(m_dsip.empty()){
        m_dsip = DSIP_DEFAULT;
    }
        
    if(m_dsport.empty()){
        m_dsport = DSPORT_DEFAULT;
    }
        
    if(max_argc < argc) {
        fprintf(stderr, "Usage: %s [-n DSIP] [-p DSport]\n", argv[0]);
        exit(EXIT_FAILURE);
    }
}

/**
 * Reads each line written on the stdin, and parses it in order 
 * to execute the corresponding command.
 */
void User::handle_keyboard_input() {
    char line[MAX_INPUT_SIZE];

    fprintf(stdout, "****************************************************************\n");
    fprintf(stdout, "**                                                            **\n");
    fprintf(stdout, "**                  Welcome to the User app!                  **\n");
    fprintf(stdout, "**             The DS Server is at your service :)            **\n");
    fprintf(stdout, "**                                                            **\n");
    fprintf(stdout, "****************************************************************\n");

    while(true){
        fprintf(stdout, "Please insert a command: ");
        if (!fgets(line, sizeof(line)/sizeof(char), stdin)){
            handle_error(USER, SYS_CALL);
        }

        char command[MAX_INPUT_SIZE] = {'\0'};
        char arg1[MAX_INPUT_SIZE] = {'\0'};
        char arg2[MAX_INPUT_SIZE] = {'\0'};

        int numTokens = sscanf(line, "%s %s %s", command, arg1, arg2);

        string arg1_str = string(arg1);
        string arg2_str = string(arg2);

        if(!strcmp(command, USER_REG)){
            if (!check_nparam(numTokens, 3, USER_REG)) continue;
            reg(arg1_str, arg2_str);
        }

        if((!strcmp(command, USER_UNREGISTER_ALT)) || (!strcmp(command, USER_UNREGISTER))){
            if (!check_nparam(numTokens, 3, USER_UNREGISTER)) continue;
            unregister(arg1_str, arg2_str);
        }

        if(!strcmp(command, USER_LOGIN)){
            if (!check_nparam(numTokens, 3, USER_LOGIN)) continue;
            login(arg1_str, arg2_str);
        }

        if(!strcmp(command, USER_LOGOUT)){
            if (!check_nparam(numTokens, 1, USER_LOGOUT)) continue;
            logout();
        }

        if(!strcmp(command, USER_SHOWUID) || !strcmp(command, USER_SHOWUID_ALT)){
            if (!check_nparam(numTokens, 1, USER_SHOWUID)) continue;
            showuid();
        }

        if(!strcmp(command, USER_EXIT)){
            if (!check_nparam(numTokens, 1, USER_EXIT)) continue;
            usr_exit();
        }
        
        if(!strcmp(command, USER_SUBSCRIBE) || !strcmp(command, USER_SUBSCRIBE_ALT)){
            if (!check_nparam(numTokens, 3, USER_SUBSCRIBE)) continue;
            subscribe(arg1_str, arg2_str);
        }

        if(!strcmp(command, USER_GROUPS) || !strcmp(command, USER_GROUPS_ALT)){
            if (!check_nparam(numTokens, 1, USER_GROUPS)) continue;
            groups();
        }

        if(!strcmp(command, USER_UNSUBSCRIBE) || !strcmp(command, USER_UNSUBSCRIBE_ALT)){
            if (!check_nparam(numTokens, 2, USER_UNSUBSCRIBE)) continue;
            unsubscribe(arg1_str);
        }

        if(!strcmp(command, USER_MY_GROUPS) || !strcmp(command, USER_MY_GROUPS_ALT)){
            if (!check_nparam(numTokens, 1, USER_MY_GROUPS)) continue;
            my_groups();
        }
                
        if(!strcmp(command, USER_SELECT) || !strcmp(command, USER_SELECT_ALT)){
            if (!check_nparam(numTokens, 2, USER_SELECT)) continue;
            usr_select(arg1_str);
        }

        if(!strcmp(command, USER_SHOWGID) || !strcmp(command, USER_SHOWGID_ALT)){
            if (!check_nparam(numTokens, 1, USER_SHOWGID)) continue;
            showgid();
        }

        if (!strcmp(command, USER_ULIST) || !strcmp(command, USER_ULIST_ALT)){
            if (!check_nparam(numTokens, 1, USER_ULIST)) continue;
            ulist();
        }

        if(!strcmp(command, USER_POST)){
            char text[MAX_INPUT_SIZE] = {'\0'};
            char fname[MAX_INPUT_SIZE] = {'\0'};
            numTokens = sscanf(line, "%s \"%[^\"]\" %s", command, text, fname);

            post(string(text), string(fname), numTokens);
        }
        
        if(!strcmp(command, USER_RETRIEVE) || !strcmp(command, USER_RETRIEVE_ALT)){
            if (!check_nparam(numTokens, 2, USER_RETRIEVE)) return;
            retrieve(arg1_str);
        }
    } 
}

/**
 * Terminates the user program (closes connections and exits with
 * success).
 */
void User::terminate() {
    disconnect(socketUDP);
    exit(EXIT_SUCCESS);
}

//::::::::::::::::::::::: COMMUNICATION ::::::::::::::::::::::://
/**
 * Creates a UDP socket to communicate with the server specified
 * by ip and port.
 * 
 * @param ip the ip address of the machine where the directory 
 * server (DS) runs
 * @param port the well-known port (TCP and UDP) where the DS 
 * server accepts requests
 */
void User::connectUDP(string ip, string port){
    int fd;
    struct addrinfo * res;
    int errcode;
	struct addrinfo hints;

	fd = socket(AF_INET,SOCK_DGRAM,0);
	if(fd == FAIL){
        fprintf(stderr, "Unable to create socket.\n");
        exit(EXIT_FAILURE);
    } 

	memset(&hints,0,sizeof hints);
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_DGRAM;

	errcode = getaddrinfo(ip.c_str(), port.c_str(), &hints, &res);
	if(errcode != SUCCESS){
        fprintf(stderr, "Unable to link to server.\n");
        exit(EXIT_FAILURE);
    } 

    socketUDP = (SOCKET *) malloc(sizeof(SOCKET));
    strcpy(socketUDP->owner, USER);
    socketUDP->fd = fd;
    socketUDP->res = res;
}

/**
 * Creates a TCP socket to communicate with the server specified
 * by ip and port.
 * 
 * @param ip the ip address of the machine where the directory 
 * server (DS) runs
 * @param port the well-known port (TCP and UDP) where the DS 
 * server accepts requests
 */
void User::connectTCP(string ip, string port){
    int fd;
    struct addrinfo * res;
    int errcode;
	struct addrinfo hints;

    fd = socket(AF_INET,SOCK_STREAM,0);
	if(fd == FAIL){
        fprintf(stderr, "Unable to create socket.\n");
        exit(EXIT_FAILURE);
    }

	memset(&hints,0,sizeof hints);
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;

	errcode = getaddrinfo(ip.c_str(), port.c_str(), &hints, &res);
	if(errcode != SUCCESS){
        fprintf(stderr, "Unable to link to server.\n");
        exit(EXIT_FAILURE);
    }

    if(connect(fd,res->ai_addr, res->ai_addrlen) == FAIL){
        fprintf(stderr, "Unable to connect to server");
        exit(EXIT_FAILURE);
    }

    socketTCP = (SOCKET *) malloc(sizeof(SOCKET));
    strcpy(socketTCP->owner, USER);
    socketTCP->fd = fd;
    socketTCP->res = res;
}

/**
 * Auxiliary function to perceive the communication status, given 
 * by the server's answer.
 * 
 * @param status the communication status
 * @param extra some extra auxiliary parameter
 * @param command the command which the server replied to
 * @return true if the status indicates success, which may lead 
 * to further actions using the server's answer
 * @return false if the status indicates something went wrong,
 * which means there will be no more actions on the server's ans-
 * wer
 */
bool User::process_status(string status, string extra, string command){
    if (status == OK){
        fprintf(stdout, "%s successful.\n", command.c_str());
        return true;
    }
    else if (status == NOK){
        fprintf(stdout, "%s unsuccessful.\n", command.c_str());
        return false;
    }
    else if (status == DUP){
        fprintf(stdout, "%s unsuccessful. Duplicate user.\n", command.c_str());
        return false;
    }
    else if (status == NEW){
        if ( parse_gid(extra)){
            fprintf(stdout, "%s successful. New group was created: %s.\n", command.c_str(), extra.c_str());
            return true;
        }
        else{
            handle_error(SERVER, PROTOCOL);
        }
    }
    else if (status == E_USR){
        fprintf(stdout, "%s unsuccessful. Invalid UID.\n", command.c_str());
        return false;
    }
    else if (status == E_GRP){
        fprintf(stdout, "%s unsuccessful. Invalid GID.\n", command.c_str());
        return false;
    }
    else if (status == E_GNAME){
        fprintf(stdout, "%s unsuccessful. Invalid GName.\n", command.c_str());
        return false;
    }
    else if (status == E_FULL){
        fprintf(stdout, "%s unsusccessful. Too many groups.\n", command.c_str());
        return false;
    }
    else if ( parse_mid(status)){
        fprintf(stdout, "%s successful. Message ID: %s\n", command.c_str(), status.c_str());
        return true;
    }
    else if (status == EOF_){
        fprintf(stdout, "There are no messages available.\n");
        return false;
    }
    else{
        handle_error(SERVER, PROTOCOL);
    }
    return false;
}

//::::::::::::::::::::::::: CHECKERS :::::::::::::::::::::::::://
/**
 * Checks if a user is logged in. This function applies for the 
 * case when a valid login is necessary.
 * 
 * @param command the command which requires this validation
 * @return true if there's a logged in user
 * @return false if there isn't
 */
bool User::check_logged_in(string command){
    if(m_uid.empty()){
        fprintf(stderr, "No user logged in to be able to %s.\n", command.c_str());
        return false;
    }
    return true;
}

/**
 * Checks if a group is selected. This function applies for the
 * case when a valid group needs to be selected.
 * 
 * @param command the command which requires this validation
 * @return true if there's a selected group
 * @return false if there isn't
 */
bool User::check_selected(string command){
    if (m_gid.empty()){
        fprintf(stderr, "No group selected to be able to %s.\n", command.c_str());
        return false;
    }
    return true;
}

/**
 * Checks if no user is logged in. This function applies for the 
 * case when no user can be logged in.
 * 
 * @param command the command which requires this validation
 * @return true if there isn't a logged in user
 * @return false if there is
 */
bool User::check_not_logged_in(string command){
    if(!m_uid.empty()){
        fprintf(stderr, "Cannot %s because there's a user logged in.\n", command.c_str());
        return false;
    }
    return true;
}

//::::::::::::::::::::::::: PRINTERS :::::::::::::::::::::::::://
/**
 * Prints groups from an already formatted vector of groups, 
 * while performing validation.
 * 
 * @param ans vector with groups. Each 3 entries represent a 
 * group.
 * Structure: RGL N[ GID GName MID]* or RGM N[ GID GName MID]*
 */
void User::print_groups(vector<string> ans){
    char output[MAX_STRING_UDP] = " GID |                     GName | MID\n";
    int n = stoi(ans[1]);
    char line[MAX_STRING] = {'\0'};

    for(int i = 2; i <= 3*n; i+=3){
        if(! parse_gid(ans[i])){
            handle_error(SERVER, PROTOCOL);
        }
        if(! parse_gname(ans[i+1])){
            handle_error(SERVER, PROTOCOL);
        }

        if(! parse_mid(ans[i+2])){
            handle_error(SERVER, PROTOCOL);
        }
        sprintf(line, "%4s |  %24s | %s\n", ans[i].c_str(), ans[i+1].c_str(), ans[i+2].c_str());
        strcat(output, line);

    }
    fprintf(stdout, "%s", output);
}

//::::::::::::::::::::::::: COMMANDS :::::::::::::::::::::::::://
/**
 * The commands related to the registration of a new user ID are:
 * reg UID pass
 * unregister UID pass or unr UID pass
 */

/**
 * reg UID pass
 * Following this command the User application sends a message to
 * the DS server, using the UDP protocol, asking to register a 
 * new user, sending its identification UID and a selected pass-
 * word pass.
 * The result of the DS registration request should be displayed.
 * 
 * @param uid the UID parameter
 * @param pass the pass parameter
 */
void User::reg(string uid, string pass){
    string buffer;
    char answer[MAX_STRING] = {'\0'};

    /* 1. Parameters verification */
    if (!check_uid(uid, USER_REG)) return;
    if (!check_pass(pass, USER_REG)) return;

    /** 
     * 2. Construct and send message; get answer 
     * Format: REG UID pass
    */
    buffer = string(USER_REG_REQUEST) + " " + uid + " " + pass + "\n" ;

    if (sendUDP(socketUDP, buffer) == FAIL){
        return;
    }
    if (receiveUDP(socketUDP, answer) == FAIL){
        return;
    }

    /** 
     * 3. Process and display result 
     * Format: RRG status
    */
    vector<string> ans = process_string(string(answer));
    string header = ans[0];

    if (header == ERR){
        handle_error(SERVER, PROTOCOL);
    }
    if ((header != USER_REG_ANSWER) || (ans.size() != 2)){
        handle_error(SERVER, PROTOCOL);
    }
    string status = ans[1];
    process_status(status, "", USER_REG);
}

/**
 * unregister UID pass or unr UID pass
 * The User application sends a message to the DS server, using 
 * the UDP protocol, asking to unregister the user with identifi-
 * cation UID and password pass. The DS server should unsubscribe
 * this user from all groups in which it was subscribed.
 * The result of the unregister request should be displayed.
 * 
 * @param uid the UID parameter
 * @param pass the pass parameter
 */
void User::unregister(string uid, string pass){
    string buffer;
    char answer[MAX_STRING] = {'\0'};
    
    /* 1. Parameters verification */
    if (!check_uid(uid, USER_UNREGISTER)) return;
    if (!check_pass(pass, USER_UNREGISTER)) return;

    /* 2. Conditions for valid unregister verification */
    if (m_uid == uid){
        fprintf(stderr, "Cannot unregister the logged in user. Logout first.\n");
        return;
    }

    /** 
     * 3. Construct and send message; get answer 
     * Format: UNR UID pass
    */
    buffer = string(USER_UNREGISTER_REQUEST) + " " + uid + " " + pass + "\n";

    if (sendUDP(socketUDP, buffer) == FAIL){
        return;
    }
    if (receiveUDP(socketUDP, answer) == FAIL){
        return;
    }

    /** 
     * 4. Process and display result 
     * Format: RUN status
    */
    vector<string> ans = process_string(string(answer));
    string header = ans[0];

    if (header == ERR){
        handle_error(SERVER, PROTOCOL);
    }
    if ((header != USER_UNREGISTER_ANSWER) || (ans.size() != 2)){
        handle_error(SERVER, PROTOCOL);
    }
    string status = ans[1];
    process_status(status, "", USER_UNREGISTER);
}


/**
 * The commands related to user identification and session termi-
 * nation are:
 * login UID pass
 * logout
 * showuid or su
 * exit
 */

/**
 * login UID pass
 * With this command the User application sends a message in UDP 
 * to the DS to validate the user credentials: UID and pass. 
 * The result of DS validation should be displayed to the user.
 * The User application memorizes the UID and pass in usage.
 * 
 * @param uid the UID parameter
 * @param pass the pass parameter
 */
void User::login(string uid, string pass){
    string buffer;
    char answer[MAX_STRING] = {'\0'};

    /* 1. Parameters verification */
    if (!check_uid(uid, USER_LOGIN)) return;
    if (!check_pass(pass, USER_LOGIN)) return;

    /* 2. Conditions for valid login verification */
    if (!check_not_logged_in(USER_LOGIN)) return;

    /** 
     * 3. Construct and send message; get answer 
     * Format: LOG UID pass
    */
    buffer = string(USER_LOGIN_REQUEST) + " " + uid + " " + pass + "\n";

    if (sendUDP(socketUDP, buffer) == FAIL){
        return;
    }
    if (receiveUDP(socketUDP, answer) == FAIL){
        return;
    }

    /** 
     * 4. Process and display result 
     * Format: RLO status
    */
    vector<string> ans = process_string(string(answer));
    string header = ans[0];

    if (header == ERR){
        handle_error(SERVER, PROTOCOL);
    }
    if ((header != USER_LOGIN_ANSWER) || (ans.size() != 2)){
        handle_error(SERVER, PROTOCOL);
    }
    string status = ans[1];
    if (!process_status(status, "", USER_LOGIN)){
        return;
    }
    m_uid = uid;
    m_pass = pass;
}

/**
 * logout
 * The User application (locally) forgets the credentials of the
 * previously logged in user. A new login command, with different
 * credentials, can then be issued.
 *
 */
void User::logout(){
    string buffer;
    char answer[MAX_STRING] = {'\0'};

    /* 1. Conditions for valid logout verification */
    if (!check_logged_in(USER_LOGOUT)) return;

    /** 
     * 2. Construct and send message; get answer 
     * Format: OUT UID pass
    */
    buffer = string(USER_LOGOUT_REQUEST) + " " + m_uid + " " + m_pass + "\n";

    if (sendUDP(socketUDP, buffer) == FAIL){
        return;
    }
    if (receiveUDP(socketUDP, answer) == FAIL){
        return;
    }

    /** 
     * 3. Process and display result 
     * Format: ROU status
    */
    vector<string> ans = process_string(string(answer));
    string header = ans[0];

    if (header == ERR){
        handle_error(SERVER, PROTOCOL);
    }
    if ((header != USER_LOGOUT_ANSWER) || (ans.size() != 2)){
        handle_error(SERVER, PROTOCOL);
    }
    string status = ans[1];
    if (!process_status(status, "", USER_LOGOUT)){
        return;
    }
    m_uid = "";
    m_pass = "";
}

/**
 * showuid or su
 * Following this command the User application locally displays
 * the UID of the user that is logged in.
 * 
 */
void User::showuid(){    
    /* 1. Conditions for valid showuid verification */
    if (!check_logged_in(USER_SHOWUID)) return;

    /* 2. Display result */
    fprintf(stdout, "UID: %s\n", m_uid.c_str());
}

/**
 * exit
 * The User application terminates, after making that all TCP 
 * connections are closed.
 * 
 */
void User::usr_exit(){
    fprintf(stdout, "We hope you found the DS Server useful! See you next time :)\n");
    terminate();
}

/**
 * The commands related to group management are:
 * groups or gl
 * subscribe GID GName or s GID GName
 * unsubscribe GID or u GID
 * my_groups or mgl
 * select GID or sag GID
 * showgid or sg
 * ulist or ul
 */

/**
 * groups or gl
 * Following this command the User application sends the DS ser-
 * ver a message in UDP asking for the list of available groups.
 * The reply should be displayed as a list of group IDs (GID) and
 * names (GName).
 * 
 */
void User::groups(){
    string buffer;
    char answer[MAX_STRING_UDP] = {'\0'};

    /** 
     * 1. Construct and send message; get answer 
     * Format: GLS
    */
    buffer = string(USER_GROUPS_REQUEST) + "\n";

    if (sendUDP(socketUDP, buffer) == FAIL){
        return;
    }
    if (receiveUDP(socketUDP, answer) == FAIL){
        return;
    }

    /** 
     * 2. Process and display result 
     * Format: RGL N[ GID GName MID]*
    */
    vector<string> ans = process_string(string(answer));
    string header = ans[0];

    if (header == ERR){
        handle_error(SERVER, PROTOCOL);
    }
    if (header != USER_GROUPS_ANSWER){
        handle_error(SERVER, PROTOCOL);
    }
    print_groups(ans);
}

/**
 * subscribe GID GName or s GID GName
 * Following this command the User application sends the DS ser-
 * ver a message in UDP, including the user's UID, asking to sub-
 * scribe the desired group, identified by its GID and GName. If
 * GID = 0 this corresponds to a request to create and subscribe
 * to a new group named GName.
 * The confirmation of successful subscription (or not) should be
 * displayed.
 * 
 * @param gid the GID parameter
 * @param gname the GName parameter
 */
void User::subscribe(string gid, string gname){
    string buffer;
    char answer[MAX_STRING] = {'\0'};

    /* 1. Parameters verification */
    if (!check_gid(gid, USER_SUBSCRIBE)) return;
    if (!check_gname(gname, USER_SUBSCRIBE)) return;
    
    /* 2. Conditions for valid subscribe verification */
    if (!check_logged_in(USER_SUBSCRIBE)) return;

    /** 
     * 3. Construct and send message; get answer 
     * Format: GSR UID GID GName
     */
    buffer = string(USER_SUBSCRIBE_REQUEST) + " " + m_uid + " " + gid + " " + gname + "\n";

    if (sendUDP(socketUDP, buffer) == FAIL){
        return;
    }
    if (receiveUDP(socketUDP, answer) == FAIL){
        return;
    }

    /** 
     * 4. Process and display result 
     * Format: RGS status
    */
    vector<string> ans = process_string(string(answer));
    string header = ans[0];

    if (header == ERR){
        handle_error(SERVER, PROTOCOL);
    }
    if (header != USER_SUBSCRIBE_ANSWER){
        handle_error(SERVER, PROTOCOL);
    }
    string status = ans[1];
    if (ans.size() == 3){
        gid = ans[2];
    } 
    else if (ans.size() != 2){
        handle_error(SERVER, PROTOCOL);
    }
    process_status(status, gid, USER_SUBSCRIBE);
}

/**
 * unsubscribe GID or u GID
 * Following this command the User application sends the DS ser-
 * ver a message in UDP, including the user's UID, asking to un-
 * subscribe group GID.
 * The confirmation of success (or not) should be displayed.
 * 
 * @param gid the GID parameter
 */
void User::unsubscribe(string gid){
    string buffer;
    char answer[MAX_STRING] = {'\0'};

    /* 1. Parameters verification */
    if (!check_gid(gid, USER_UNSUBSCRIBE)) return;

    /* 2. Conditions for valid unsubscribe verification */
    if (!check_logged_in(USER_UNSUBSCRIBE)) return;
    
    /**
     * 3. Construct and send message; get answer 
     * Format: GUR UID GID
     */
    buffer = string(USER_UNSUBSCRIBE_REQUEST) + " " + m_uid + " " + gid + "\n";

    if (sendUDP(socketUDP, buffer) == FAIL){
        return;
    }
    if (receiveUDP(socketUDP, answer) == FAIL){
        return;
    }

    /**
     * 4. Process and display result 
     * Format: RGU status
     */
    vector<string> ans = process_string(string(answer));
    string header = ans[0];

    if (header == ERR){
        handle_error(SERVER, PROTOCOL);
    }
    if ((header != USER_UNSUBSCRIBE_ANSWER) || (ans.size() != 2)){
        handle_error(SERVER, PROTOCOL);
    }
    string status = ans[1];
    process_status(status, "", USER_UNSUBSCRIBE);
}

/**
 * my_groups or mgl
 * Following this command the User application sends the DS ser-
 * ver a message in UDP, including the user's UID, asking the 
 * list of groups to which this user has already subscibed. 
 * The reply should be displayed as a list of group IDs and names
 * 
 */
void User::my_groups(){
    string buffer;
    char answer[MAX_STRING_UDP] = {'\0'};

    /* 1. Conditions for valid my_groups verification */
    if (!check_logged_in(USER_MY_GROUPS)) return;

    /**
     * 2. Construct and send message; get answer 
     * Format: GLM UID
     */
    buffer = string(USER_MY_GROUPS_REQUEST) + " " + m_uid + "\n";

    if (sendUDP(socketUDP, buffer) == FAIL){
        return;
    }
    if (receiveUDP(socketUDP, answer) == FAIL){
        return;
    }

    /**
     * 3. Process and display result 
     * Format: RGM N[ GID GName MID]*
     */
    vector<string> ans = process_string(string(answer));
    string header = ans[0];

    if (header == ERR){
        handle_error(SERVER, PROTOCOL);
    }
    if (header != USER_MY_GROUPS_ANSWER){
        handle_error(SERVER, PROTOCOL);
    }
    print_groups(ans);
}

/**
 * select GID or sag GID
 * Following this command the User application locally memorizes
 * GID as the ID of the active group. Subsequent ulist, post and
 * retrieve messaging commands refer to this GID.
 * 
 * @param gid the GID parameter
 */
void User::usr_select(string gid){
    /* 1. Parameters verification */
    if (!check_gid(gid, USER_SELECT)) return;

    /* 2. Conditions for valid select verification */
    if (!check_logged_in(USER_SELECT)) return;
    
    /* 3. Display result */
    m_gid = gid;
    fprintf(stdout, "selected group %s successfully.\n", m_gid.c_str());
}

/**
 * showgid or sg
 * Following this command the User application locslly displays 
 * the GID of the selected group.
 * 
 */
void User::showgid(){
    /* 1. Conditions for valid showgid verification */
    if (!check_logged_in(USER_SHOWGID)) return;
    if (!check_selected(USER_SHOWGID)) return;
    
    /* 2. Display result */
    fprintf(stdout, "GID: %s\n", m_gid.c_str());
}

/**
 * ulist or ul
 * Following this command the User application sends the DS ser-
 * ver a message in TCP asking for the list of user UIDs that are
 * subscribed to the currently subscribed group GID.
 * 
 */
void User::ulist(){
    string buffer;

    /* 1. Conditions for valid ulist verification */
    if (!check_logged_in(USER_ULIST)) return;
    if (!check_selected(USER_ULIST)) return;

    /* 2. Establish TCP connection */
    connectTCP(m_dsip, m_dsport);
    
    /**
     * 3. Construct and send message
     * Format: ULS GID
     */
    buffer = string(USER_ULIST_REQUEST) + " " + m_gid + "\n";

    if(sendTCP(socketTCP, buffer, buffer.size()) == FAIL){
        return;
    }

    /** 
     * 4. Get answer
     * Format: RUL status [GName [UID ]*]
     * This is going to happen in different stages
     */
    /**
     * 4.1. Get, process and display RUL status
     * Possible formats: "RUL OK "; "RUL NOK"
     */
    char answer_head[MAX_ANS_HEAD] = {'\0'};
    if (receiveTCP(socketTCP, answer_head, MAX_ANS_HEAD) == FAIL){
        disconnect(socketTCP);
        return;
    }

    vector<string> ans = process_string(string(answer_head));
    string header = ans[0];

    if (header == ERR){
        handle_error(SERVER, PROTOCOL);
    }
    if (header != USER_ULIST_ANSWER){
        handle_error(SERVER, PROTOCOL);
    }
    string status = ans[1];
    if (!process_status(status, "", USER_ULIST)) {
        disconnect(socketTCP);
        return;
    }

    /* 4.2. Get, process and display GName */
    char answer_gname[MAX_GNAME + 1] = {'\0'};
    if (!gname_receiveTCP(socketTCP, answer_gname, USER_ULIST)) return;

    fprintf(stdout, "Group: %s\n", answer_gname);

    /* 4.3. Get, process and display [UID ]* */
    int n_subscribers = 0;
    while (true){
        char answer_uid[MAX_UID + 1] = {'\0'};
        int res = uid_receiveTCP(socketTCP, answer_uid, USER_ULIST);
        if (!res){
            break;
        }
        if (res == FAIL){
            return;
        }

        fprintf(stdout, "\t%s\n", answer_uid);
        n_subscribers++;
    }

    if (n_subscribers == 0){
        fprintf(stdout, "This group has no subscribers.\n");
    }
    else{
        fprintf(stdout, "Total: %d subscribers.\n",n_subscribers);
    }
    disconnect(socketTCP);
}

/**
 * The commands related to messaging are:
 * post "text" [Fname]
 * retrieve MID or r MID
 * These commands can only be issued after a user has logged in 
 * and an active GID has been selected.
 */

/**
 * post "text" [Fname]
 * Following this command the User establishes a TCP session with
 * the DS server and sends a message containing text and possibly
 * also a file with name Fname.
 * The confirmation of success (or not) should be displayed, inc-
 * luding the posted message's ID MID. The TCP connection is then
 * closed.
 * 
 * @param text the text parameter (without "")
 * @param fname the Fname parameter
 * @param nparams the number of parameters received by this co-
 *                mmand, expected 2 or 3 parameters
 */
void User::post(string text, string fname, int nparams){
    string buffer;
    char answer[MAX_STRING_TCP] = {'\0'};

    /* 1. Parameters verification */
    if (!check_text(text, USER_POST)) return;
    if(fname.empty()){
        if(!check_nparam(nparams, 2, USER_POST)) return;
    }else{
        if(!check_nparam(nparams, 3, USER_POST)) return;
        if (!check_fname(fname, USER_POST)) return;
    }

    /* 2. Conditions for valid post verification */
    if (!check_logged_in(USER_POST)) return;
    if (!check_selected(USER_POST)) return;

    /* 3. Establish TCP connection */
    connectTCP(m_dsip, m_dsport);

    /**
     * 4. Construct and send message; get answer 
     * Format: PST UID GID Tsize text [Fname Fsize data]
     */
    /* First "half" of the request: PST UID GID Tsize text */
    buffer = string(USER_POST_REQUEST) + " " + m_uid + " " + m_gid + " " + to_string(text.size()) + " " + text;

    /* If there's no file to send, the message is complete */
    if (fname.empty()){
        buffer += "\n";

        if(sendTCP(socketTCP, buffer, buffer.size()) == FAIL){
            disconnect(socketTCP);
            return;
        }
    }
    /* Else add Fname Fsize data to the request and send */
    else {
        FILE * file;
        file = fopen(fname.c_str(), "rb");
        if (!file){
            disconnect(socketTCP);
            fprintf(stderr, "Unable to open file.\n");
            disconnect(socketTCP);
            return;
        }

        /* Get Fsize */
        fseek(file, 0, SEEK_END);
        int fsize = ftell(file);
        fseek(file, 0, SEEK_SET);

        /* Ensure file isn't too big */
        if (!parse_fsize(to_string(fsize))){
            fprintf(stderr, "Invalid file size.\n");
            fclose(file);
            disconnect(socketTCP);
            return;
        }
        
        /* Send PST UID GID Tsize text Fname Fsize */
        buffer += " " + fname + " " + to_string(fsize) + " ";
        if(sendTCP(socketTCP, buffer, buffer.size()) == FAIL){
            fclose(file);
            disconnect(socketTCP);
            return;
        }

        /* Send file data */
        if (sendfileTCP(socketTCP, file) == FAIL){
            fclose(file);
            disconnect(socketTCP);
            return;
        }

        /* Send '\n' (end of message) */
        if(sendTCP(socketTCP, "\n", 1) == FAIL){
            fclose(file);
            disconnect(socketTCP);
            return;
        }

        fclose(file);
    }

    if(receiveTCP(socketTCP, answer, MAX_STRING_TCP) == FAIL){
        disconnect(socketTCP);
        return;
    }

    disconnect(socketTCP);

    /**
     * 5. Process and display result 
     * Format: RPT status
     */
    vector<string> ans = process_string(string(answer));
    string header = ans[0];

    if (header == ERR){
        handle_error(SERVER, PROTOCOL);
    }
    if ((header != USER_POST_ANSWER) || (ans.size() != 2)){
        handle_error(SERVER, PROTOCOL);
    }
    string status = ans[1];
    process_status(status, "", USER_POST);
}

/**
 * retrieve MID or r MID
 * Following this command the User establishes a TCP session with
 * the DS server and sends a message asking to receive up to 20
 * messages, starting with the obe with identifier MID, for the 
 * active group GID. The DS server only sends messages that in-
 * clude at least an author UID and text - any incomplete messa-
 * ges are omitted.
 * After receiving the messages, the User application sends the 
 * DS a confirmation and then closes the TCP session. The reply
 * should be displayed as a numbered list of text messages and, 
 * if available, the associated filenames and respective sizes.
 * 
 * @param mid the MID parameter
 */
void User::retrieve(string mid){
    string buffer;
    
    /* 1. Parameters verification */
    if (!check_mid(mid, USER_RETRIEVE)) return;

    /* 2. Conditions for valid retrieve verification */
    if (!check_logged_in(USER_RETRIEVE)) return;
    if (!check_selected(USER_RETRIEVE)) return;

    /* 3. Establish TCP connection */
    connectTCP(m_dsip, m_dsport);

    /** 
     * 4. Construct and send message
     * Format: RTV UID GID MID
    */
    buffer = string(USER_RETRIEVE_REQUEST) + " " + m_uid + " " + m_gid + " " + mid + "\n";

    if(sendTCP(socketTCP, buffer, buffer.size()) == FAIL){
        disconnect(socketTCP);
        return;
    }

    /** 
     * 5. Get answer
     * Format: RRT status [N[ MID UID Tsize text[ / Fname Fsize data]]*]
     * This is going to happen in different stages.
     */

    /**
     * 5.1. Get RRT status
     * Possible formats: "RRT OK "; "RRT EOF"; "RRT NOK"
     */
    char answer_head[MAX_ANS_HEAD] = {'\0'};

    if(receiveTCP(socketTCP, answer_head, MAX_ANS_HEAD) == FAIL){
        disconnect(socketTCP);
        return;
    }
    /** 
     * 5.1.1. Process first stage and display result 
     */
    vector<string> ans = process_string(answer_head);
    string header = ans[0];

    if (header == ERR){
        handle_error(SERVER, PROTOCOL);
    }
    if (header != USER_RETRIEVE_ANSWER){
        handle_error(SERVER, PROTOCOL);
    }
    string status = ans[1];
    if (!process_status(status, "", USER_RETRIEVE)){
        disconnect(socketTCP);
        return;
    }

    /**
     * If status is OK we need to read the rest of the answer
     * Format: [N[ MID UID Tsize text[ / Fname Fsize data]]*]
     */

    /** 
     * 5.2. Get N (0 =< N =< 20)
     * Possible formats: "N " (N has 1 char); "N" (N has 2 chars)
     */
    char answer_N[MAX_N] = {'\0'};
    if(receiveTCP(socketTCP, answer_N, MAX_N) == FAIL){
        disconnect(socketTCP);
        return;
    }

    int N = stoi(answer_N);
    
    /* If N is 0, there are no messages */
    if (N == 0){
        fprintf(stdout, "There are no messages available.\n");
        disconnect(socketTCP);
        return;
    }
    /* If N has 2 chars, read the space */
    if (N >= 10){
        char aux;
        if(receiveTCP(socketTCP, &aux, 1) == FAIL){
            disconnect(socketTCP);
            return;
        }
    }

    /**
     * 5.3. Get and display the N messages
     * [ MID UID Tsize text[ / Fname Fsize data]]*
     * Format: MID UID Tsize text[ / Fname Fsize data]]*
     */
    bool read_mid = false;
    char answer_mid[MAX_MID + 1] = {'\0'};
    char answer_uid[MAX_UID + 1] = {'\0'};
    char answer_tsize[MAX_TSIZE + 1] = {'\0'};
    char answer_text[MAX_TEXT + 1] = {'\0'};
    for (int i = 0; i < N; i++){
        /* Get MID UID Tsize text */
        if (read_mid == false){
            int res1 = mid_receiveTCP(socketTCP, answer_mid, USER_RETRIEVE);
            if (res1 == FAIL) return;
            if (!res1){
                disconnect(socketTCP);
                return;
            }
        }

        int res2 = uid_receiveTCP(socketTCP, answer_uid, USER_RETRIEVE);
        if (res2 == FAIL) return;
        if (!res2){
            disconnect(socketTCP);
            return;
        }

        int res3 = tsize_receiveTCP(socketTCP, answer_tsize, USER_RETRIEVE);
        if (res3 == FAIL) return;
        if (!res3){
            disconnect(socketTCP);
            return;
        }

        int res4 = text_receiveTCP(socketTCP, answer_text, stoi(answer_tsize), USER_RETRIEVE, NULL);
        if (res4 == FAIL) return;
        if (!res4){
            disconnect(socketTCP);
            return;
        }

        string output = "Message " + string(answer_mid) + " from user " + string(answer_uid) 
                + ": " + string(answer_text) + " (" + string(answer_tsize) + " characters)\n";

        /* Check if there is a file or not by reading the next 
        word + 1 space */ 
        bool hasfile = true;
        if (i < (N - 1)){
            char answer_nextword[MAX_MID + 1] = {'\0'};
            memset(answer_nextword, '\0', MAX_MID + 1);
            int k = word_receiveTCP(socketTCP, answer_nextword, MAX_MID + 1);
            if (k == FAIL){
                disconnect(socketTCP);
                return;
            }
            answer_nextword[k] = '\0';

            /* If we read a MID + 1 space there's no file */
            if (parse_mid(string(answer_nextword))){
                hasfile = false;
                read_mid = true;
                memset(answer_mid, '\0', MAX_MID + 1);
                strcpy(answer_mid, answer_nextword);
            }
            /* Otherwise we read "/ " and can move on */
            else{
                hasfile = true;
                read_mid = false;
            }
        }

        /* If we're reading the last message, the way to see if
        there's a file is different */
        else{
            /* If we've reached the end of the message, there's 
            no file */
            char aux;
            int x = receiveTCP(socketTCP, &aux, 1);
            if (x == FAIL){
                disconnect(socketTCP);
                return;
            }
            if (!x){
                hasfile = false;
            }
            /* Else, we've read '/'; read the ' ' and move on */
            else{
                hasfile = true;
                char aux_ ;
                if (receiveTCP(socketTCP, &aux_, 1) == FAIL){
                    disconnect(socketTCP);
                    return;
                }
            }
        }
        
        /* Get Fname Fsize data (if there's a file) */
        char answer_fname[MAX_FNAME + 1] = {'\0'};
        char answer_fsize[MAX_FSIZE + 1] = {'\0'};
        if (hasfile){
            int res5 = fname_receiveTCP(socketTCP, answer_fname, USER_RETRIEVE);
            if (res5 == FAIL) return;
            if (!res5){
                disconnect(socketTCP);
                return;
            }
            int res6 = fsize_receiveTCP(socketTCP, answer_fsize, USER_RETRIEVE);
            if (res6 == FAIL) return;
            if (!res6){
                disconnect(socketTCP);
                return;
            }
            
            int fsize = stoi(answer_fsize);

            if (receivefileTCP(socketTCP, answer_fname, fsize) == FAIL) return;

            output += "Also received and saved " + string(answer_fname) + " (" + string(answer_fsize) + " bytes)\n";

            /* Read extra ' ' or '\n' after the data */
            char aux;
            if(receiveTCP(socketTCP, &aux, 1) == FAIL){
                disconnect(socketTCP);
                return;
            }            
        }  
        
        fprintf(stdout, "%s", output.c_str());
    }

    disconnect(socketTCP);
}

//::::::::::::::::::::::::::: MAIN :::::::::::::::::::::::::::://

int main(int argc, char** argv) {
    //Program will run in User's constructor
    User(argc, argv);
    return 0;
}