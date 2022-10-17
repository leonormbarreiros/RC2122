#ifndef __H_CONSTANT
#define __H_CONSTANT

#define GROUP_NUMBER 12

#define USER "user"
#define SERVER "server"

#define DSIP_DEFAULT ""
#define DSPORT_DEFAULT "58012"

#define USERS "USERS"
#define GROUPS "GROUPS"

#define PASS "pass"
#define LOGIN "login"

#define max(A, B) ((A) >= (B) ? (A) : (B))

#define PROTOCOL 123
#define SYS_CALL 321
#define OTHER 111

#define FAIL -1
#define SUCCESS 0
#define VALID 1
#define INVALID -1
#define NOT_FOUND 1
#define NOT_LOGIN 2
#define NOT_SUBSCRIBED 3
#define NO_FILE -2

#define MAX_ANS_HEAD 7
#define MAX_N 2
#define MAX_TSIZE 3
#define MAX_TEXT 240

//::::::::::::::::::::::::::: SIZES ::::::::::::::::::::::::::://
#define MAX_MESSAGE 256
#define MAX_STRING_TCP 512
#define MAX_HEAD_TCP 4
#define MAX_STRING_UDP 4096
#define MAX_REQUEST_UDP 128
#define MAX_ULIST 100033
#define MAX_DIRNAME 64
#define MAX_PATHNAME 128
#define MAX_RETRIEVE 6000 //max 5762
#define MAX_STRING 128
#define MAX_TSIZE 3 //len("240")
#define MAX_FNAME 24
#define MAX_FSIZE 10
#define MAX_MID 4
#define MAX_UID 5
#define MAX_GID 2
#define MAX_GNAME 24
#define MAX_PASS 8
#define MAX_NAME 32
#define MAX_ID 16
#define MAX_NGROUPS 99
#define MAX_INPUT_SIZE 512

//::::::::::::::::::::::::::: INPUT ::::::::::::::::::::::::::://
#define USER_REG "reg" //reg
#define USER_UNREGISTER "unregister" //unregister
#define USER_UNREGISTER_ALT "unr" //unregister
#define USER_LOGIN "login" //login
#define USER_LOGOUT "logout" //logout
#define USER_SHOWUID "showuid" //showuid
#define USER_SHOWUID_ALT "su" //showuid
#define USER_EXIT "exit" //exit
#define USER_GROUPS "groups" //groups
#define USER_GROUPS_ALT "gl" //groups
#define USER_SUBSCRIBE "subscribe" //subscribe
#define USER_SUBSCRIBE_ALT  "s" //subscribe
#define USER_UNSUBSCRIBE "unsubscribe" //unsubscribe
#define USER_UNSUBSCRIBE_ALT "u" //unsubscribe
#define USER_MY_GROUPS "my_groups" //my_groups
#define USER_MY_GROUPS_ALT "mgl" //my_groups
#define USER_SELECT "select" //select
#define USER_SELECT_ALT "sag" //select
#define USER_SHOWGID "showgid" //showgid
#define USER_SHOWGID_ALT "sg" //showgid
#define USER_ULIST "ulist" //ulist
#define USER_ULIST_ALT "ul" //ulist
#define USER_POST "post" //post
#define USER_RETRIEVE "retrieve" //retrieve
#define USER_RETRIEVE_ALT "r" //retrieve

//:::::::::::::::::::::::::: REQUEST :::::::::::::::::::::::::://
#define USER_REG_REQUEST "REG" //reg
#define USER_UNREGISTER_REQUEST "UNR" //unregister
#define USER_LOGIN_REQUEST "LOG" //login
#define USER_LOGOUT_REQUEST "OUT" //logout
#define USER_GROUPS_REQUEST "GLS" //groups
#define USER_SUBSCRIBE_REQUEST "GSR" //subscribe
#define USER_UNSUBSCRIBE_REQUEST "GUR" //unsubscribe
#define USER_MY_GROUPS_REQUEST "GLM" //my_groups
#define USER_ULIST_REQUEST "ULS" //ulist
#define USER_POST_REQUEST "PST" //post
#define USER_RETRIEVE_REQUEST "RTV" //retrieve

//:::::::::::::::::::::::::: ANSWER ::::::::::::::::::::::::::://
#define USER_REG_ANSWER "RRG" //reg
#define USER_UNREGISTER_ANSWER "RUN" //unregister
#define USER_LOGIN_ANSWER "RLO" //login
#define USER_LOGOUT_ANSWER "ROU" //logout
#define USER_GROUPS_ANSWER "RGL" //groups
#define USER_SUBSCRIBE_ANSWER "RGS" //subscribe
#define USER_UNSUBSCRIBE_ANSWER "RGU" //unsubscribe
#define USER_MY_GROUPS_ANSWER "RGM" //my_groups
#define USER_ULIST_ANSWER "RUL" //ulist
#define USER_POST_ANSWER "RPT" //post
#define USER_RETRIEVE_ANSWER "RRT" //retrieve

//:::::::::::::::::::::::::: STATUS ::::::::::::::::::::::::::://
#define NEW "NEW"
#define OK "OK"
#define DUP "DUP"
#define NOK "NOK"
#define ERR "ERR"
#define E_USR "E_USR"
#define E_GRP "E_GRP"
#define E_GNAME "E_GNAME"
#define E_FULL "E_FULL"
#define ZERO "0"
#define EOF_ "EOF"

#endif