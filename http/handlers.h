/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   handlers.h
 * Author: cpu02331
 *
 * Created on June 25, 2019, 11:03 AM
 */

#ifndef HANDLERS_H
#define HANDLERS_H

#include "Poco/Net/HTTPServer.h"
#include "Poco/Net/HTTPRequestHandler.h"
#include "Poco/Net/HTTPRequestHandlerFactory.h"
#include "Poco/Net/HTTPServerParams.h"
#include "Poco/Net/HTTPServerRequest.h"
#include "Poco/Net/HTTPServerResponse.h"
#include "Poco/Net/HTTPServerParams.h"
#include "Poco/Net/HTMLForm.h"
#include "Poco/Net/PartHandler.h"
#include "Poco/Net/MessageHeader.h"
#include "Poco/Net/ServerSocket.h"
#include "Poco/CountingStream.h"
#include "Poco/NullStream.h"
#include "Poco/StreamCopier.h"
#include "Poco/Exception.h"
#include "Poco/Util/ServerApplication.h"

#include "Poco/URI.h"
#include "userProfileService.h"
#include <iostream>
#include <thrift/protocol/TBinaryProtocol.h>
#include <thrift/protocol/TJSONProtocol.h>
#include <thrift/transport/TSocket.h>
#include <thrift/transport/TTransportUtils.h>


using Poco::Net::ServerSocket;
using Poco::Net::HTTPRequestHandler;
using Poco::Net::HTTPRequestHandlerFactory;
using Poco::Net::HTTPServer;
using Poco::Net::HTTPServerRequest;
using Poco::Net::HTTPServerResponse;
using Poco::Net::HTTPServerParams;
using Poco::Net::MessageHeader;
using Poco::Net::HTMLForm;
using Poco::Net::NameValueCollection;
using Poco::Util::ServerApplication;
using Poco::Util::Application;
using Poco::CountingInputStream;
using Poco::NullOutputStream;
using Poco::StreamCopier;

using namespace apache::thrift;
using namespace apache::thrift::protocol;
using namespace apache::thrift::transport;
using namespace std;

class UIcontainer {
public:
    static void loadUI(std::ostream& ostr);
};

class GETRequestHandler : public HTTPRequestHandler {
public:

    GETRequestHandler(userProfileServiceClient* c) : client(c) {
    }

    void handleRequest(HTTPServerRequest& request, HTTPServerResponse& response);
private:
    userProfileServiceClient* client;
};

class CREATERequestHandler : public HTTPRequestHandler {
public:

    CREATERequestHandler(userProfileServiceClient* c) : client(c) {
    }

    void handleRequest(HTTPServerRequest& request, HTTPServerResponse& response);
private:
    userProfileServiceClient* client;
};

class UPDATERequestHandler : public HTTPRequestHandler {
public:

    UPDATERequestHandler(userProfileServiceClient* c) : client(c) {
    }

    void handleRequest(HTTPServerRequest& request, HTTPServerResponse& response);
private:
    userProfileServiceClient* client;
};

class DELETERequestHandler : public HTTPRequestHandler {
public:

    DELETERequestHandler(userProfileServiceClient* c) : client(c) {
    }

    void handleRequest(HTTPServerRequest& request, HTTPServerResponse& response);
private:
    userProfileServiceClient* client;
};

class FormRequestHandler : public HTTPRequestHandler {
public:

    FormRequestHandler() {
    }

    void handleRequest(HTTPServerRequest& request, HTTPServerResponse& response);
};

#endif /* HANDLERS_H */

