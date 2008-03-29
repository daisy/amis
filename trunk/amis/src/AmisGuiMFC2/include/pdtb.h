#ifndef PDTB_H
#define PDTB_H

#include "ambulant/net/url.h"

bool importUserKeysIntoRegistry(std::string);
void addUserKeysToAmbulantPluginEngine();
bool registerBookKeyFile(const ambulant::net::url*, const ambulant::net::url*);
bool isUserKeyFile(std::string);
#endif