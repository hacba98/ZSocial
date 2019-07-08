/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   PocoServer.h
 * Author: cpu02331
 *
 * Created on June 11, 2019, 4:10 PM
 */

#ifndef POCOSERVER_H
#define POCOSERVER_H

#include "Poco/ThreadPool.h"
#include "Poco/Util/ServerApplication.h"
#include "Poco/Util/Option.h"
#include "Poco/Util/OptionSet.h"
#include "Poco/Util/HelpFormatter.h"
#include "LruCache.h"
#include "Poco/Task.h"
#include "Poco/TaskManager.h"
#include "Poco/FileChannel.h"
#include "Poco/Util/LoggingConfigurator.h"
#include "Poco/ThreadPool.h"
#include "SocialServiceHandler.h"
#include <thrift/protocol/TBinaryProtocol.h>
#include <thrift/protocol/TJSONProtocol.h>
#include <thrift/server/TSimpleServer.h>
#include <thrift/server/TThreadedServer.h>
#include <thrift/server/TThreadPoolServer.h>
#include <thrift/server/TNonblockingServer.h>
#include <thrift/transport/TServerSocket.h>
#include <thrift/transport/TNonblockingServerSocket.h>
#include <thrift/concurrency/PosixThreadFactory.h>
#include <thrift/transport/TBufferTransports.h>
#include <iostream>
#include <mutex>
#include <vector>
#include <kchashdb.h>
#include <unistd.h>

using Poco::Util::ServerApplication;
using Poco::Util::Application;
using Poco::Util::Option;
using Poco::Util::OptionSet;
using Poco::Util::OptionCallback;
using Poco::Util::HelpFormatter;

using namespace ::apache::thrift;
using namespace ::apache::thrift::protocol;
using namespace ::apache::thrift::transport;
using namespace ::apache::thrift::server;
using ::apache::thrift::stdcxx::shared_ptr;

class userProfileTask : public Poco::Task {
public:
    void startSimpleServer(shared_ptr<TProcessor> processor,
            shared_ptr<TServerTransport> serverTransport,
            shared_ptr<TTransportFactory> transportFactory,
            shared_ptr<TProtocolFactory> protocolFactory);
    void startThreadedServer(shared_ptr<TProcessor> processor,
            shared_ptr<TServerTransport> serverTransport,
            shared_ptr<TTransportFactory> transportFactory,
            shared_ptr<TProtocolFactory> protocolFactory);
    void startThreadPoolServer(shared_ptr<TProcessor> processor,
            shared_ptr<TServerTransport> serverTransport,
            shared_ptr<TTransportFactory> transportFactory,
            shared_ptr<TProtocolFactory> protocolFactory);
    void startNonBlockingServer(shared_ptr<TProcessor> processor,
            shared_ptr<TProtocolFactory> protocolFactory);

    userProfileTask(int sR,int p,int nST,int nIOT,shared_ptr<SocialServiceHandler> h) : 
        Task("SampleTask"),_serverRequested(sR),port(p),numServerThread(nST),numIOThread(nIOT),handler(h){
    }
    
    void runTask();
    void cancel();
    
    int _serverRequested;
    int port;
    int numServerThread;
    int numIOThread;
    shared_ptr<SocialServiceHandler> handler;
    Poco::SharedPtr<TSimpleServer> simpleServer;
    Poco::SharedPtr<TThreadedServer> threadedServer;
    Poco::SharedPtr<TThreadPoolServer> threadPoolServer;
    Poco::SharedPtr<TNonblockingServer> nonBlockingServer;
};

class userProfileServer : public Poco::Util::ServerApplication {
public:

    userProfileServer();

    ~userProfileServer() {
    }

protected:
    
    void initialize(Application& self) ;

    void uninitialize() ;

    void defineOptions(OptionSet& options) ;

    void handleHelp(const std::string& name,
            const std::string& value) ;

    void handleServer(const std::string& name,
            const std::string& value);
    int main(const std::vector<std::string>& args) ;

private:
    bool _helpRequested;
    int _serverRequested;
    int port;
    int numServerThread;
    int numIOThread;
    shared_ptr<SocialServiceHandler> _handler;
};

POCO_SERVER_MAIN(userProfileServer)

#endif /* POCOSERVER_H */

