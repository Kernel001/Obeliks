#pragma once
#include "types.h"
#include <cpprest/http_client.h>
#include <cpprest/json.h>
#include <cpprest/uri.h>

class AsteriskARI
{
public:
	AsteriskARI(tVariant servAddr, tVariant user, tVariant pass);
	~AsteriskARI();
	bool Dial(tVariant from, tVariant to, tVariant callerID);
	utility::string_t getAsteriskVersion();
	utility::string_t getKernelVersion();
private:
	web::http::client::http_client *m_client = NULL;
	utility::string_t astVersion;
	utility::string_t kernelVersion;
};

