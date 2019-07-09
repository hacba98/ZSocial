//
// HTTPFormServer.cpp
//
// This sample demonstrates the HTTPServer and HTMLForm classes.
//
// Copyright (c) 2006, Applied Informatics Software Engineering GmbH.
// and Contributors.
//
// SPDX-License-Identifier:	BSL-1.0
//


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
#include "Poco/Util/Option.h"
#include "Poco/Util/OptionSet.h"
#include "Poco/Util/HelpFormatter.h"
#include "Poco/URI.h"
#include <iostream>
#include <thrift/protocol/TBinaryProtocol.h>
#include <thrift/protocol/TJSONProtocol.h>
#include <thrift/transport/TSocket.h>
#include <thrift/transport/TTransportUtils.h>
#include "handlers.h"


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
using Poco::Util::Option;
using Poco::Util::OptionSet;
using Poco::Util::HelpFormatter;
using Poco::CountingInputStream;
using Poco::NullOutputStream;
using Poco::StreamCopier;

using namespace apache::thrift;
using namespace apache::thrift::protocol;
using namespace apache::thrift::transport;
using namespace std;


class UPRequestHandlerFactory : public HTTPRequestHandlerFactory {
public:

    UPRequestHandlerFactory(boost::shared_ptr<TProtocol> protocol) : RPCClient(protocol) {
    }

    HTTPRequestHandler* createRequestHandler(const HTTPServerRequest& request) {
        Poco::URI uri(request.getURI());
        
        if(uri.getPath() == "/get"){
            return new GETRequestHandler(&RPCClient);
        }else if(uri.getPath() == "/create"){
            return new CREATERequestHandler(&RPCClient);
        }else if(uri.getPath() == "/update"){
            return new UPDATERequestHandler(&RPCClient);
        }else if(uri.getPath() == "/delete"){
            return new DELETERequestHandler(&RPCClient);
        }else{
            return new FormRequestHandler();
        }
    }
private:
    ProfileServicesClient RPCClient;
};

class HTTPUPServer : public Poco::Util::ServerApplication {
public:

    HTTPUPServer() : _helpRequested(false) {
    }

    ~HTTPUPServer() {
    }

protected:

    void initialize(Application& self) {
        loadConfiguration("http.properties"); // load default configuration files, if present
        ServerApplication::initialize(self);
    }

    void uninitialize() {
        logger().information("SHUT DOWN SERVER");
        ServerApplication::uninitialize();
    }

    void defineOptions(OptionSet& options) {
        ServerApplication::defineOptions(options);

        options.addOption(
                Option("help", "h", "display help information on command line arguments")
                .required(false)
                .repeatable(false));
    }

    void handleOption(const std::string& name, const std::string& value) {
        ServerApplication::handleOption(name, value);

        if (name == "help")
            _helpRequested = true;
    }

    void displayHelp() {
        HelpFormatter helpFormatter(options());
        helpFormatter.setCommand(commandName());
        helpFormatter.setUsage("OPTIONS");
        helpFormatter.setHeader("A web server that shows how to work with HTML forms.");
        helpFormatter.format(std::cout);
    }

    int main(const std::vector<std::string>& args) {
        if (_helpRequested) {
            displayHelp();
        } else {
            string host = config().getString("host","localhost");
            int backendPort = config().getInt("backendPort",9090);
            int _transport = config().getInt("Btrans",1);
            boost::shared_ptr<TTransport> socket(new TSocket(host, backendPort));
            
            boost::shared_ptr<TTransport> transport;
            if(_transport == 1){
                transport = boost::shared_ptr<TTransport>(new TBufferedTransport(socket));
            }else{        
                transport = boost::shared_ptr<TTransport>(new TFramedTransport(socket));
            }
            
            boost::shared_ptr<TProtocol> protocol(new TBinaryProtocol(transport));
            transport->open();
            int port = config().getInt("httpPort", 9980);


            ServerSocket svs(port);

            HTTPServer srv(new UPRequestHandlerFactory(protocol), svs, new HTTPServerParams);

            srv.start();

            waitForTerminationRequest();

            srv.stop();
            transport->close();
        }
        return Application::EXIT_OK;
    }

private:
    bool _helpRequested;
};

int main(int argc, char** argv) {
    HTTPUPServer app;
    return app.run(argc, argv);
}