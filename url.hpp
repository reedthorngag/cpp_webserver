
#ifndef _URL
#define _URL
class URL {
    public:
        char* host;
        char* path;
        char* protocol;
        char*** query_params;

        URL(char* url);

        char* get_param(char* param_name);
};

#endif