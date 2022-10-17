#include "utils.hpp"
#include "constant.hpp"

namespace parsers{
    /**
     * Indicates if the actual number of given parameters matches
     * the expected (correct) number of parameters.
     * 
     * @param nparams the number of given parameters
     * @param expected the expected number of parameters
     * @return true if they match
     * @return false if they don't match
     */
    bool parse_nparam(int nparams, int expected){
        if (nparams == expected){
            return true;
        }
        return false;
    }
    
    /**
     * Indicates if a string is a UID or not.
     * A UID consists of 5 alphanumerical characters, restricted to
     * numbers.
     * 
     * @param input the string to be parsed
     * @return true if it's a UID
     * @return false if it's not a UID
     */
    bool parse_uid(string input){
        int len = input.length();
        if (len != 5){
            return false;
        }
        for (int i = 0; i < len; i++){
            if (!(is_number(input[i]))){
                return false;
            }
        }
        return true;
    }

    /**
     * Indicates if a string is a pass or not.
     * A pass consists of 8 alphanumerical characters, restricted to
     * letters and numbers.
     * 
     * @param input the string to be parsed
     * @return true if it's a pass
     * @return false if it's not a pass
     */
    bool parse_pass(string input){
        int len = input.length();
        if (len != 8){
            return false;
        }
        for (int i = 0; i < len; i++){
            if(!(is_number(input[i]) || is_letter(input[i]))){
                return false;
            }
        }
        return true;
    }

    /**
     * Indicates if a string is a GID or not.
     * A GID consists of 2 alphanumerical caracters, restricted to
     * numbers.
     * 
     * @param input the string to be parsed
     * @return true if it's a GID
     * @return false if it's not a GID
     */
    bool parse_gid(const string input){
        int len = input.length();
        if(len != 2){
            return false;
        }
        for(int i = 0; i < len; i++){
            if(!(is_number(input[i]))){
                return false;
            }
        }
        return true;
    }

    /**
     * Indicates if a string is a GName or not.
     * A GName consists of a maximum 24 alphanumerical characters, 
     * restricted to letters and numbers, plus '-' and '_'.
     * 
     * @param input the string to be parsed
     * @return true if it's a GName
     * @return false if it's not a GName
     */
    bool parse_gname(const string input){
        int len = input.length();
        if((len > 24) || (len == 0)){
            return false;
        }

        for(int i = 0; i < len; i++){
            if(!is_number(input[i]) && !is_letter(input[i]) && input[i] != '-' && input[i] != '_'){
                return false;
            }
        }
        return true;
    }

    /**
     * Indicates if a string is a MID or not.
     * A MID consists of 4 alphanumerical characters, restricted to
     * numbers.
     * 
     * @param input the string to be parsed
     * @return true if it's a MID
     * @return false if it's not a MID
     */
    bool parse_mid(const string input){
        int len = input.length();
        if(len != 4){
            return false;
        }
        for(int i = 0; i < len; i++){
            if(!(is_number(input[i]))){
                return false;
            }
        }
        return true;
    }

    /**
     * Indicates if a string is a Fname or not.
     * A Fname consists of a maximum 24 alphanumerical characters, 
     * restricted to letters and numbers, plus '-', '_' and '.', in-
     * cluding the separating dot and the 3-letter extension.
     * 
     * @param input the string to be parsed
     * @return true if it's a Fname
     * @return false if it's not a Fname
     */
    bool parse_fname(const string input){
        int len = input.length();
        if((len > 24) || (len < 5)){
            return false;
        }
        for(int i = 0; i < len - 4; i++){
            if(!is_number(input[i]) && !is_letter(input[i]) 
                && input[i] != '-' && input[i] != '_' && input[i] != '.'){
                return false;
            }
        }
        if(input[len - 4] != '.'){
            return false;
        }
        for(int i = len - 3; i < len; i++){
            if(!is_letter(input[i])){
                return false;
            }
        }
        return true;
    }

    /**
     * Indicates if a string is a Fsize or not.
     * A Fsize consists of a maximum 10 alphanumerical characters,
     * restricted to numbers.
     * 
     * @param input 
     * @return true 
     * @return false 
     */
    bool parse_fsize(const string input){
        int len = input.length();
        if(len > 10){
            return false;
        }
        for(int i = 0; i < len; i++){
            if(!(is_number(input[i]))){
                return false;
            }
        }
        return true;
    }
    
    /**
     * Indicates if a string is a text message or not.
     * A text message text consists of a maximum 240 characters.
     * 
     * @param input the string to be parsed
     * @return true if it's a text 
     * @return false if it's not a text
     */
    bool parse_text(const string input){
        int len = input.length();
        if(len > 240){
            return false;
        }
        return true;
    }

    /**
     * Indicates if a string is a Tsize or not.
     * A Tsize consists of a maximum 3 alphanumerical characters,
     * restricted to numbers. 0 =< Tsize =< 240
     * 
     * @param input 
     * @return true 
     * @return false 
     */
    bool parse_tsize(const string input){
        int len = input.length();
        if(len > 3){
            return false;
        }
        for(int i = 0; i < len; i++){
            if(!(is_number(input[i]))){
                return false;
            }
        }
        if ((stoi(input) > 240) || (stoi(input) < 0)){
            return false;
        }
        return true;
    }

    /**
     * Indicates if a character represents a number or not.
     * 
     * @param input the character to be parsed
     * @return true if it's a number
     * @return false if it's not a number
     */
    bool is_number(char input){
        return ((int)input >= (int)'0') 
            && ((int)input <= (int)'9');
    }

    /**
     * Indicates if a character represents a letter or not.
     * 
     * @param input the character to be parsed
     * @return true if it's a letter
     * @return false if it's not a letter
     */
    bool is_letter(char input){
        return (((int)input >= (int)'A') 
            && ((int)input <= (int)'Z')) ||
            (((int)input >= (int)'a') 
            && ((int)input <= (int)'z')); 
    }
}

namespace checkers{
    /**
     * Checks wether the actual length of the parameters vector matc-
     * es the expected length of it.
     * 
     * @param nparams the number of parameters
     * @param expected the supposed length of the parameters vector
     * @param command the command which needs to perform this valida-
     * tion
     * @return true if if matches
     * @return false if it doesn't
     */
    bool check_nparam(int nparams, int expected, string command){
        if(!parsers::parse_nparam(nparams, expected)){
            fprintf(stderr, "%d arguments expected. Wrong format for %s.\n", expected, command.c_str());
            return false;
        }
        return true;
    }

    /**
     * Checks wether the uid parameter represents a valid uid.
     * 
     * @param uid the uid parameter
     * @param command the command which requires this validation
     * @return true if it's a valid uid
     * @return false if it's not
     */
    bool check_uid(string uid, string command){
        if(!parsers::parse_uid(uid)){
            fprintf(stderr, "Wrong format for UID. Unable to %s.\n", command.c_str());
            return false;
        }
        return true;
    }

    /**
     * Checks wether the pass parameter represents a valid pass.
     * 
     * @param pass the pass parameter
     * @param command the command which requires this validation
     * @return true if it's a valid pass
     * @return false if it's not 
     */
    bool check_pass(string pass, string command){
        if(!parsers::parse_pass(pass)){
            fprintf(stderr, "Wrong format for pass. Unable to %s.\n", command.c_str());
            return false;
        }
        return true;
    }

    /**
     * Checks wether the gid parameter represents a valid gid.
     * 
     * @param gid the gid parameter
     * @param command the command which requires this validation
     * @return true if it's a valid gid
     * @return false if it's not 
     */
    bool check_gid(string gid, string command){
        if(!parsers::parse_gid(gid)){
            fprintf(stderr, "Wrong format for GID. Unable to %s.\n", command.c_str());
            return false;
        }
        return true;
    }

    /**
     * Checks wether the gname parameter represents a valid gname.
     * 
     * @param gname the gname parameter
     * @param command the command which requires this validation
     * @return true if it's a valid gname
     * @return false if it's not 
     */
    bool check_gname(string gname, string command){
        if(!parsers::parse_gname(gname)){
            fprintf(stderr, "Wrong format for GName. Unable to %s.\n", command.c_str());
            return false;
        }
        return true;
    }

    /**
     * Checks wether the mid parameter represents a valid mid.
     * 
     * @param mid the mid parameter
     * @param command the command which requires this validation
     * @return true if it's a valid mid
     * @return false if it's not 
     */
    bool check_mid(string mid, string command){
        if (!parsers::parse_mid(mid)){
            fprintf(stderr, "Wrong format for mid. Unable to %s.\n", command.c_str());
            return false;
        }
        return true;
    }

    /**
     * Checks wether the fname parameter represents a valid fname.
     * 
     * @param fname the fname parameter
     * @param command the command which requires this validation
     * @return true if it's a valid fname
     * @return false if it's not 
     */
    bool check_fname(string fname, string command){
        if (!parsers::parse_fname(fname)){
            fprintf(stderr, "Wrong format for fname. Unable to %s.\n", command.c_str());
            return false;
        }
        return true;
    }

    /**
     * Checks wether the fsize parameter represents a valid fsize.
     * 
     * @param fsize the fsize parameter
     * @param command the command which requires this validation
     * @return true if it's a valid fsize
     * @return false if it's not
     */
    bool check_fsize(string fsize, string command){
         if (!parsers::parse_fsize(fsize)){
            fprintf(stderr, "Wrong format for fsize. Unable to %s.\n", command.c_str());
            return false;
        }
        return true;
    }

    /**
     * Checks wether the text parameter represents a valid text.
     * 
     * @param text the text parameter
     * @param command the command which requires this validation
     * @return true if it's a valid text
     * @return false if it's not 
     */
    bool check_text(string text, string command){
        if (!parsers::parse_text(text)){
            fprintf(stderr, "Wrong format for text. Unable to %s.\n", command.c_str());
            return false;
        }
        return true;
    }

    /**
     * Checks wether the tsize parameter represents a valid tsize.
     * 
     * @param tsize the tsize parameter
     * @param command the command which requires this validation
     * @return true if it's a valid tsize
     * @return false if it's not
     */
    bool check_tsize(string tsize, string command){
        if (!parsers::parse_tsize(tsize)){
            fprintf(stderr, "Wrong format for tsize. Unable to %s.\n", command.c_str());
            return false;
        }
        return true;
    }
}

namespace protocols{
    /**
     * Disconnects a socket and frees the structure associated to
     * it.
     * 
     * @param s the pointer to the socket structure
     */
    void disconnect(SOCKET * s){
        close(s->fd);
        free(s);
    }

    //:::::::::::::::::::: GENERIC SENDERS :::::::::::::::::::://
    /**
     * Sends a message using a UDP socket.
     * 
     * @param s the pointer to the socket structure
     * @param message the message to be sent
     * @return int SUCCESS or FAIL
     */
    int sendUDP(SOCKET * s, string message){
        /* The way to send a UDP message is different depending 
        on who sends it (USER or SERVER) */
        if (!strcmp(s->owner, USER)){
            if(sendto(s->fd, message.c_str(), message.length(), 0, (s->res)->ai_addr, (s->res)->ai_addrlen) == FAIL){
                fprintf(stderr, "Unable to send message, please try again!\n");
                return FAIL;
            }
        }
        else if (!strcmp(s->owner, SERVER)) {
            socklen_t addrlen = sizeof(s->addr);
            if (sendto(s->fd, message.c_str(), message.length(), 0,(struct sockaddr*) &(s->addr), addrlen) == FAIL){
                return FAIL;
            }
        }
        return SUCCESS;
    }

    /**
     * Sends a message using a TCP socket.
     * 
     * @param s the pointer to the socket structure
     * @param message the message to be sent
     * @param nbytes the number of bytes (characters) we want to
     * send
     * @return int SUCCESS or FAIL
     */
    int sendTCP(SOCKET * s, string message, int nbytes){
        ssize_t nleft, n;
        int nwritten = 0;
        char * ptr, buffer[MAX_STRING_TCP];

        ptr = strcpy(buffer, message.c_str());

        nleft=nbytes;
        while(nleft > 0){
            n = write(s->fd, ptr, nleft);
            if(n <= 0){
                fprintf(stderr, "Unable to send message, please try again!\n");
                return FAIL;
            }
            nleft -= n;
            ptr += n;
            nwritten += n;
        }
        return nwritten;
    }

    //::::::::::::::::::: GENERIC RECEIVERS ::::::::::::::::::://
    /**
     * Waits for a UDP socket to receive a message.
     * 
     * @param s the pointer to the socket structure
     * @param message the message to be received
     * @return int SUCCESS or FAIL
     */
    int receiveUDP(SOCKET * s, char * message){
        int tries = 0;
        struct timeval tv;
        fd_set rfds;
        socklen_t addrlen = sizeof(s->addr);
        int recVal = 0;
        /*
        bool timePassed = false;
        time_t startListenTime = time(NULL);
        */

        /* source: https://stackoverflow.com/questions/25888914/linux-c-socket-select-loop */
        FD_ZERO(&rfds);
        FD_SET(s->fd, &rfds);
        tv.tv_usec = 0.0;
        tv.tv_sec = (long) 3;
        while (tries < 3){
            fd_set rfds;
            FD_ZERO(&rfds);
            FD_SET(s->fd, &rfds);
            recVal = select(s->fd + 1, &rfds, NULL, NULL, &tv);
            switch(recVal){
                case(0):{
                    fprintf(stdout, "Timeout. %d out of 3 tries completed.\n", tries);
                    break;
                }
                case(-1):{
                    fprintf(stderr, "Error.\n");
                    exit(EXIT_FAILURE);
                }
                default:{
                    int i = recvfrom(s->fd, message, MAX_STRING_UDP, 0, (struct sockaddr*)&(s->addr), &addrlen);
	                if(i == FAIL){
                        fprintf(stderr, "Unable to receive message, please try again!");
                        return FAIL;
                    }

                    return SUCCESS;
                }
            }
            tries++;
        }
        return FAIL;
    }

    /**
     * Waits for a TCP socket to receive a message.
     * 
     * @param s the pointer to the socket structure
     * @param message the message to be received
     * @param nbytes the number of bytes (characters) we want to
     * receive
     * @return int FAIL or the number of characters/bytes read
     */
    int receiveTCP(SOCKET * s, char * message, int nbytes){
        ssize_t nleft, nread;
        char *ptr;
        nleft = nbytes;
        ptr = message;

        while(nleft > 0){
            nread = read(s->fd, ptr, nleft);
            if(nread == FAIL){
                break;  
            }
            else if(nread == 0) break;
            nleft -= nread;
            ptr += nread;
        }
        nread = nbytes - nleft;
        return nread;
    }

    //:::::::::::::::::::: UDP AUXILIARIES :::::::::::::::::::://
    /**
     * Sends the reply command and status using a UDP socket.
     * 
     * @param s the pointer to the socket structure
     * @param command the reply command
     * @param status the status
     * @return int SUCCESS or FAIL
     */
    int sendstatusUDP(SOCKET * s, string command, string status){
        string buffer;
        if(status == ERR){
            buffer = status + "\n";
        }else{
            buffer = command + " " + status + "\n";
        }

        if(sendUDP(s, buffer) == FAIL){
           return FAIL;
        }

        return SUCCESS;
    }

    /**
     * Sends the reply command and status using a TCP socket.
     * 
     * @param s the pointer to the socket structure
     * @param command the reply command
     * @param status the status
     * @return int SUCCESS or FAIL
     */
    int sendstatusTCP(SOCKET * s, string command, string status){
        string buffer;
        if(status == ERR){
            buffer = status + "\n";
        }else{
            buffer = command + " " + status + "\n";
        }

        if(sendTCP(s, buffer, buffer.length()) == FAIL){
           return FAIL;
        }

        return SUCCESS;
    }
    

    //:::::::::::::::::::: TCP AUXILIARIES :::::::::::::::::::://
    /**
     * Sends a file using a TCP socket.
     * 
     * @param s the pointer to the socket structure
     * @param fp the pointer to the file to be sent
     * @return int SUCCESS or FAIL
     */
    int sendfileTCP(SOCKET * s, FILE * file){
        /* source: https://coderedirect.com/questions/200858/send-binary-file-over-tcp-ip-connection */

        fseek(file, 0, SEEK_END);
        int fsize = ftell(file);
        fseek(file, 0, SEEK_SET);

        if (fsize == EOF){
            return FAIL;
        }
        char buffer[MAX_STRING_TCP] = {'\0'};
        /* Send piece by piece (each one is 512 bytes at most) */
        while (fsize > 0){
            int n = min(fsize, MAX_STRING_TCP);
            n = fread(buffer, 1, n, file);
            if (n < 1){
                return FAIL;
            }

            /* Send each piece */
            unsigned char * pbuf = (unsigned char *) buffer;
            int num = n;
            while (num > 0){
                int nread = send(s->fd, pbuf, num, 0);
                if (nread == FAIL){
                    fprintf(stderr, "Unable to send file, please try again!\n");
                    return FAIL;
                }
                pbuf += nread;
                num -= nread;
            }

            fsize -= n;
            bzero(buffer, MAX_STRING_TCP);
        }
        return SUCCESS;
    }

    /**
     * Waits for a TCP socket to receive a file.
     * 
     * @param s the pointer to the socket structure
     * @param path the name of the path fo the file
     * @param fsize the size (in bytes) of the received file
     * @return int SUCCESS or FAIL
     */
    int receivefileTCP(SOCKET * s, char * path, int fsize){
        /* source: https://coderedirect.com/questions/200858/send-binary-file-over-tcp-ip-connection */

        FILE * file = fopen(path, "wb");
        if (!file){
            fprintf(stderr, "Unable to open file.\n");
            disconnect(s);
            return FAIL;
        }

        char buffer[MAX_STRING_TCP] = {'\0'};
        while (fsize > 0){
            int n = min(fsize, MAX_STRING_TCP);

            /* Receive each piece of message from socket */
            unsigned char *pbuf = (unsigned char *) buffer;
            int num = n;
            while(num > 0){
                int nread = recv(s->fd, pbuf, num, 0);
                if (nread == FAIL){
                    fprintf(stderr, "Unable to receive file, please try again!\n");
                    fclose(file);
                    remove(path);
                    return FAIL;
                }

                pbuf += nread;
                num -= nread;
            }

            /* Save each piece of message to the file */
            int offset = 0;
            while (offset < n){
                size_t nwritten = fwrite(&buffer[offset], 1, n - offset, file);
                if (nwritten < 1){
                    fprintf(stderr, "Unable to write to file, please try again!\n");
                    fclose(file);
                    remove(path);
                    return FAIL;
                }
                offset += nwritten;
            }

            fsize -= n;
            bzero(buffer, MAX_STRING_TCP);
        }
        fclose(file);
        return SUCCESS;
    }

    /**
     * Waits for a TCP socket to receive a word. A word is sepa-
     * rated by ' ' or '\n'.
     * 
     * @param s the pointer to the socket structure
     * @param answer the word to be received
     * @param limit the maximum length of the word
     * @return int the number of characters/bytes read
     */
    int word_receiveTCP(SOCKET * s, char * answer, int limit){
        char buffer[MAX_STRING];
        int nread = 0;
        int i;
        for(i = 0; i < limit; i++){
            if(receiveTCP(s, buffer, 1) == FAIL){
                return FAIL;
            }
            nread++;
            if(buffer[0] == ' ' || buffer[0] == '\n'){
                break;
            }
            answer[i] = buffer[0];
        }

        answer[i] = '\0';
        return nread;
    }

    /**
     * Waits for a TCP socket to receive a UID. Performs valida-
     * tion in order to assess wether the received message is a
     * UID.
     * Note this function will read not only the wanted word, but
     * also an extra character after. If that character needs to 
     * be processed later, this function is not applicable.
     * 
     * @param s the pointer to the socket structure
     * @param uid the UID to be received
     * @param command 
     * @return 0 (false), 1 (true) or FAIL (-1)
     */
    int uid_receiveTCP(SOCKET * s, char * uid, string command){
        memset(uid, '\0', MAX_UID + 1);
        int j = word_receiveTCP(s, uid, MAX_UID + 1);
        if (j == FAIL){
            disconnect(s);
            return FAIL;
        }
        uid[MAX_UID] = '\0';
        
        return (int) parsers::parse_uid(string(uid));
    }

    /**
     * Waits for a TCP socket to receive a GID. Performs valida-
     * tion in order to assess wether the received message is a
     * GID.
     * Note this function will read not only the wanted word, but
     * also an extra character after. If that character needs to 
     * be processed later, this function is not applicable.
     * 
     * @param s the pointer to the socket structure
     * @param gid the GID to be received
     * @param command 
     * @return 0 (false), 1 (true) or FAIL (-1)
     */
    int gid_receiveTCP(SOCKET * s, char * gid, string command){
        memset(gid, '\0', MAX_GID + 1);
        if (word_receiveTCP(s, gid, MAX_GID + 1) == FAIL){
            disconnect(s);
            return FAIL;
        }
        gid[MAX_GID] = '\0';
    
        return (int) parsers::parse_gid(string(gid));
    }

    /**
     * Waits for a TCP socket to receive a GName. Performs vali-
     * dation in order to assess wether the received message is a
     * GName.
     * Note this function will read not only the wanted word, but
     * also an extra character after. If that character needs to 
     * be processed later, this function is not applicable.
     * 
     * @param s the pointer to the socket structure
     * @param gname the GName to be received
     * @param command 
     * @return 0 (false), 1 (true) or FAIL (-1)
     */
    int gname_receiveTCP(SOCKET * s, char * gname, string command){
        memset(gname, '\0', MAX_GNAME + 1);
        int j = word_receiveTCP(s, gname, MAX_GNAME + 1);
        if (j == FAIL){
            disconnect(s);
            return FAIL;
        }
        gname[j] = '\0';
        
        return (int) parsers::parse_gname(string(gname));
    }

    /**
     * Waits for a TCP socket to receive a MID. Performs valida-
     * tion in order to assess wether the received message is a
     * MID.
     * Note this function will read not only the wanted word, but
     * also an extra character after. If that character needs to 
     * be processed later, this function is not applicable.
     * 
     * @param s the pointer to the socket structure
     * @param mid the MID to be received
     * @param command 
     * @return 0 (false), 1 (true) or FAIL (-1)
     */
    int mid_receiveTCP(SOCKET * s, char * mid, string command){
        memset(mid, '\0', MAX_MID + 1);
        if (word_receiveTCP(s, mid, MAX_MID + 1) == FAIL){
            disconnect(s);
            return FAIL;
        }
        mid[MAX_MID] = '\0';
        
        return (int) parsers::parse_mid(string(mid));
    }

    /**
     * Waits for a TCP socket to receive a Fname. Performs vali-
     * dation in order to assess wether the received message is a
     * Fname.
     * Note this function will read not only the wanted word, but
     * also an extra character after. If that character needs to 
     * be processed later, this function is not applicable.
     * 
     * @param s the pointer to the socket structure
     * @param fname the Fname to be received
     * @param command 
     * @return 0 (false), 1 (true) or FAIL (-1)
     */
    int fname_receiveTCP(SOCKET * s, char * fname, string command){
        memset (fname, '\0', MAX_FNAME + 1);
        int a = word_receiveTCP(s, fname, MAX_FNAME + 1);
        if (a == FAIL){
            disconnect(s);
            return FAIL;
        }
        fname[a] = '\0';
        
        return (int) parsers::parse_fname(string(fname));
    }

    /**
     * Waits for a TCP socket to receive a Fsize. Performs vali-
     * dation in order to assess wether the received message is a
     * Fsize.
     * Note this function will read not only the wanted word, but
     * also an extra character after. If that character needs to 
     * be processed later, this function is not applicable.
     * 
     * @param s the pointer to the socket structure
     * @param fsize the Fsize to be received
     * @param command 
     * @return true if it received a valid Fsize
     * @return false if it didn't
     */
    int fsize_receiveTCP(SOCKET * s, char * fsize, string command){
        memset (fsize, '\0', MAX_FSIZE + 1);
        int b = word_receiveTCP(s, fsize, MAX_FSIZE + 1);
        if (b == FAIL){
            disconnect(s);
            return FAIL;
        }
        fsize[b] = '\0';
        
        return (int) parsers::parse_fsize(string(fsize));
    }

    /**
     * Waits for a TCP socket to receive a text. Performs valida-
     * tion in order to assess wether the received message is a
     * text.
     * 
     * @param s the pointer to the socket structure
     * @param text the text to be received
     * @param command 
     * @param last_caracter
     * @return 0 (false), 1 (true) or FAIL (-1)
     */
    int text_receiveTCP(SOCKET * s, char * text, int tsize, string command, char * last_caracter){
        memset(text, '\0', MAX_TEXT + 1);
        if (receiveTCP(s, text, tsize + 1) == FAIL){
            disconnect(s);
            return FAIL;
        }
        
        if(last_caracter != NULL){
            last_caracter[0] = text[tsize];
        }

        text[tsize] = '\0';

        return checkers::check_text(text, command);
    }

    /**
     * Waits for a TCP socket to receive a Tsize. Performs vali-
     * dation in order to assess wether the received message is a
     * Tsize.
     * Note this function will read not only the wanted word, but
     * also an extra character after. If that character needs to 
     * be processed later, this function is not applicable.
     * 
     * @param s the pointer to the socket structure
     * @param tsize the Tsize to be received
     * @param command 
     * @return 0 (false), 1 (true) or FAIL (-1)
     */
    int tsize_receiveTCP(SOCKET * s, char * tsize, string command){
        memset(tsize, '\0', MAX_TSIZE + 1);
        if (word_receiveTCP(s, tsize, MAX_TSIZE + 1) == FAIL){
            disconnect(s);
            return FAIL;
        }
        tsize[MAX_TSIZE] = '\0';
        
        return (int) parsers::parse_tsize(string(tsize));
    }
}

namespace auxiliaries{
    /**
     * Given a certain string, divides it in words. Each word is
     * separated from the rest by ' '.
     * 
     * @param input the string whose words we'll obtain
     * @return vector<string> the vector of the string's words
     */
    vector<string> process_string(const string input){
        vector<string> vec_input;
        string temp;
        stringstream stream(input);
    
        while(getline(stream, temp, ' ') && !stream.eof())
            vec_input.push_back(move(temp));
    
        if(!temp.empty())
            vec_input.push_back(move(temp));

        if(vec_input.back().back()=='\n'){
            vec_input.back().pop_back();
        }
        return vec_input;
    }

    /**
     * Terminates the program which called this function, in case
     * of an error which no longer allows it to carry on executi-
     * on.
     * 
     * @param program either USER or SERVER, corresponding on the
     * program which generated the error which no longer allows
     * continuation.
     * @param type either PROTOCOL, SYS_CALL or OTHER, correspon-
     * ding on the type of error verified
     */
    void handle_error(string program, int type){
        switch (type){
            case PROTOCOL:
                fprintf(stderr, "Wrong protocol message received from %s.\n", program.c_str());
                exit(EXIT_FAILURE);
                break;
            case SYS_CALL:
                fprintf(stderr, "Error condition from system call.\n");
                exit(EXIT_FAILURE);
                break;
            default:
                fprintf(stderr, "Something went wrong. Terminating.\n");
                exit(EXIT_FAILURE);
                break;
        }
    }
}

