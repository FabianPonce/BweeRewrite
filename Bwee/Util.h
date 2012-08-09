#ifndef Bwee_Util_h
#define Bwee_Util_h

#include "Common.h"

class Util
{
public:
    static const char* getVersionString() {
        std::stringstream st;
        st << "Bwee " << BWEE_MAJOR_VERSION << "." << BWEE_MINOR_VERSION << "." << BWEE_BUGFIX_VERSION;
        return st.str().c_str();
    }
};

#endif
