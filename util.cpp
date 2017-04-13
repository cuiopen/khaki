#include "util.h"
#include <stdarg.h>
#include <stdio.h>
#include <sys/time.h>

namespace khaki
{

void util::strSplit(const std::string & src, const std::string & sep,
              std::vector<std::string>& tokens )
{
    tokens.clear();
    std::string s;
    for(std::string::const_iterator i = src.begin(); i != src.end(); i++)
    {
        if (sep.find(*i) != std::string::npos)
        {
            tokens.push_back(s);
            s = "";
        }
        else
        {
            s += *i;
        }
    }
    if ( s.length() || tokens.size() ) tokens.push_back(s);
}

std::string util::string_format( const std::string fmt, ... )
{
    int size = (static_cast<int>(fmt.size())) * 2 + 50;
    std::string str;
    va_list ap;
    while ( true ) {
        str.resize(size);
        va_start(ap, fmt);
        int n = vsnprintf(const_cast<char*>(str.data()), size, fmt.c_str(), ap);
        va_end(ap);
        if (n > -1 && n < size) {
            str.resize(n);
            return str;
        }
        if (n > -1)
            size = n + 1;
        else
            size *= 2;   
    }
    return str;
}

int util::getTime()
{
    struct timeval tm;
    gettimeofday(&tm, NULL);
    return tm.tv_sec;
}

}