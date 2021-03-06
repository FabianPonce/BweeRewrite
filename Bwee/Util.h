#ifndef Bwee_Util_h
#define Bwee_Util_h

#include "Common.h"

class Util
{
public:
    static std::string getVersionString() {
        std::stringstream st;
        st << "Bwee " << BWEE_MAJOR_VERSION << "." << BWEE_MINOR_VERSION << "." << BWEE_BUGFIX_VERSION \
			<< " | " << PLATFORM_TEXT;
        return st.str();
    }
};

#endif
