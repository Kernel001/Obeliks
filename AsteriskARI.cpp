#include "stdafx.h"
#include "AsteriskARI.h"

using namespace web;
using namespace web::http;
using namespace web::http::client;
using namespace web::json;
using namespace utility;

//Param types are checked in caller
AsteriskARI::AsteriskARI(tVariant servAddr, tVariant user, tVariant pass)
{
	
	http_client_config config;
	config.set_credentials(credentials(string_t(user.pwstrVal), string_t(pass.pwstrVal)));
	m_client = new http_client(uri(string_t(servAddr.pwstrVal)), config);

	string_t path = uri_builder(U("asterisk")).append_path(U("info")).to_string();
	m_client->request(methods::GET, path)
		.then([=](http_response responce) -> pplx::task<json::value>
	{
		if (responce.status_code() == status_codes::OK)
			return responce.extract_json();
		else
			return pplx::task_from_result(json::value(L"Error"));
	})
		.then([=](pplx::task<json::value> prevTask)
	{
		const json::value& v = prevTask.get();
		if (!v.is_null()) {
			astVersion = v.at(L"system").at(L"version").as_string();
			kernelVersion = v.at(L"build").at(L"kernel").as_string();
		}
	});
}

AsteriskARI::~AsteriskARI()
{
	if (m_client)
		delete m_client;
}

bool AsteriskARI::Dial(tVariant from, tVariant to, tVariant callerID)
{
	
	if (!m_client)
		return false;
	string_t path = uri_builder(U("channels"))
		.append_query(L"endpoint", string_t(from.pwstrVal))
		.append_query(L"extension", string_t(to.pwstrVal))
		.append_query(L"callerId", string_t(callerID.pwstrVal))
		.append_query(L"context", L"Long-Distance")
		.to_string();
	m_client->request(methods::POST, path); //TODO remove from final!
	
	return true;
}

string_t AsteriskARI::getAsteriskVersion() { return astVersion; }
string_t AsteriskARI::getKernelVersion() { return kernelVersion; }
