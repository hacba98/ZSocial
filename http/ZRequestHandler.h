/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   ZRequestHandler.h
 * Author: cpu10855
 *
 * Created on July 4, 2019, 10:04 AM
 */

#ifndef ZREQUESTHANDLER_H
#define ZREQUESTHANDLER_H

#include "Poco/Net/HTTPRequestHandlerFactory.h"
#include "Poco/Net/HTTPRequestHandler.h"
#include "Poco/Net/HTTPServerRequest.h"
#include "Poco/Net/HTTPServerResponse.h"

class ZRequestHandler : public Poco::Net::HTTPRequestHandler {
public:
	// main control
	virtual void handleRequest(
		Poco::Net::HTTPServerRequest &req, 
		Poco::Net::HTTPServerResponse &res);
	
	// handler of specify request
private:

};

class ZRequestHandlerFactory : public Poco::Net::HTTPRequestHandlerFactory {
public:
	virtual Poco::Net::HTTPRequestHandler * createRequestHandler(const Poco::Net::HTTPServerRequest &req){
		return new ZRequestHandler;
	}
};

#endif /* ZREQUESTHANDLER_H */

