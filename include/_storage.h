#include <Preferences.h>

Preferences prefs;

/**
 * @param filename: name space is limited to 15 characters
*/
void setupStorage(const char* filename)
{
    prefs.begin(filename);
}

