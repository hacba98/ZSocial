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
#include "ProfileServices.h"
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

class TOOL {
public:
    static void loadUI(std::ostream& ostr);
    
    static int getTime(int d, int m, int y) {
        time_t rawtime;
        struct tm * timeinfo;

        time(&rawtime);
        timeinfo = localtime(&rawtime);
        timeinfo->tm_year = y - 1900;
        timeinfo->tm_mon = m - 1;
        timeinfo->tm_mday = d;

        return int(mktime(timeinfo));
    }

    static void getDMY(int birth,int &d, int &m, int &y) {
        time_t b = (time_t)birth;
        tm *ltm = localtime(&b);
        d = ltm->tm_mday;
        m = ltm->tm_mon + 1;
        y = ltm->tm_year + 1900;
    }
    
    static void setProfile(UserProfile& profile , string name,string gender ,int d ,int m ,int y ,long phoneNumber,string username ,string password){
        if(d != 0 && m != 0 && y != 0) profile.__set_birth(TOOL::getTime(d,m,y));
        if(gender != "") profile.__set_gender(gender == "Nam");
        if(name != "No one will name like this") profile.__set_name(name);
        if(password != "") profile.__set_password(password);
        if(phoneNumber != 0l)profile.__set_phoneNumber(phoneNumber);
        if(username != "") profile.__set_username(username);
    };
};

class GETRequestHandler : public HTTPRequestHandler {
public:

    GETRequestHandler(ProfileServicesClient* c) : client(c) {
    }

    void handleRequest(HTTPServerRequest& request, HTTPServerResponse& response);
private:
    ProfileServicesClient* client;
};

class CREATERequestHandler : public HTTPRequestHandler {
public:

    CREATERequestHandler(ProfileServicesClient* c) : client(c) {
    }

    void handleRequest(HTTPServerRequest& request, HTTPServerResponse& response);
private:
    ProfileServicesClient* client;
};

class UPDATERequestHandler : public HTTPRequestHandler {
public:

    UPDATERequestHandler(ProfileServicesClient* c) : client(c) {
    }

    void handleRequest(HTTPServerRequest& request, HTTPServerResponse& response);
private:
    ProfileServicesClient* client;
};

class DELETERequestHandler : public HTTPRequestHandler {
public:

    DELETERequestHandler(ProfileServicesClient* c) : client(c) {
    }

    void handleRequest(HTTPServerRequest& request, HTTPServerResponse& response);
private:
    ProfileServicesClient* client;
};

class LoginRequestHandler : public HTTPRequestHandler {
public:

    LoginRequestHandler(ProfileServicesClient* c) : client(c) {
    }

    void handleRequest(HTTPServerRequest& request, HTTPServerResponse& response);
private:
    ProfileServicesClient* client;
};

class FormRequestHandler : public HTTPRequestHandler {
public:

    FormRequestHandler() {
    }

    void handleRequest(HTTPServerRequest& request, HTTPServerResponse& response);
};

#endif /* HANDLERS_H */

