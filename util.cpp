#include <string.h>
#include <stdio.h>
#include <unistd.h>

#include "util.hpp"

char* read_in(char** ptr, char end_char, int max) {
    char* start = *ptr;

    int n = 1;
    while (max--) {
        if (*++*ptr==end_char) break;
        n++;
    }

    if (max<0) return nullptr;

    char* buf = new char[n+1];
    buf[n] = 0;
    
    memcpy(buf,start,n);

    ++*ptr;

    return buf;
}

bool str_equals(char* s1, const char* s2) {
    while (*s1==*s2) {
        if (*s1==0) return true;
        s1++;
        s2++;
    }
    return false;
}

bool sendFile(int conn, char* file) {
    if (FILE* fp = fopen(file,"r")) {
        const char* string = "HTTP/2 200\r\nContent-Type: text/html; charset=utf-8\r\n\r\n";
        write(conn,string,strlen(string));

        char buf[1024];
        int in;
        while ((in=fread(buf,1,1024,fp))>0) {
            write(conn,buf,in);
        }
        fclose(fp);
    } else {
        const char* string = "HTTP/2 404\r\nContent-Type: text/html; charset=utf-8\r\n\r\nFile not found!";
        write(conn,string,strlen(string));
        return false;
    }
    return true;
}

bool validate(char* path) {
    while (*path != 0) {
        if (*path=='.' && path[1]=='.') return false;
        path++;
    }

    return true;
}

char* join(char* s1, char* s2) {
    int len1 = strlen(s1);
    int len2 = strlen(s2);
    char* s3 = new char[len1+len2+1];
    s3[len1+len2] = 0;

    memcpy(s3,s1,len1);
    memcpy(s3+len1,s2,len2);

    return s3;
}
