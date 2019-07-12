#include "PocoServer.h"

userProfileServer::userProfileServer() : _helpRequested(false), _serverRequested(3) {
    
}

void userProfileServer::initialize(Application& self) {
    if (_helpRequested) return;
    
    loadConfiguration("app.properties");
    
    port = config().getInt("port",9090);
    numServerThread = config().getInt("numServerThread",8);
    numIOThread = config().getInt("numIOThread",2);
    
    addSubsystem(new UserProfileDB());
    addSubsystem(new SocialServiceHandler());
    addSubsystem(new WorkerSystem());
    
    ServerApplication::initialize(self);
    
    std::string logfile = config().getString("logFile","none");
    
    if(logfile != "none"){
        Poco::AutoPtr<Poco::FileChannel> pFC(new Poco::FileChannel);
        pFC->setProperty("path", logfile);
        pFC->setProperty("rotation", config().getString("rotation","2 M"));
        pFC->setProperty("archive", config().getString("archive","timestamp"));
        
        logger().setChannel(pFC);
    }
    
    logger().setLevel(config().getString("loggerLevel","trace"));
}

void userProfileServer::uninitialize() {
    if (_helpRequested) return;
    
    logger().information("SHUT DOWN");
    ServerApplication::uninitialize();
}

void userProfileServer::defineOptions(OptionSet& options) {
    ServerApplication::defineOptions(options);

    options.addOption(
            Option("help", "h", "display argument help information")
            .required(false)
            .repeatable(false)
            .callback(OptionCallback<userProfileServer>(
            this, &userProfileServer::handleHelp)));
    options.addOption(
            Option("start", "s", "choose type of server you want to start\nvaule=0: SimpleServer(default),\nvaule=1: ThreadedServer,\nvaule=2: ThreadPoolServer,\nvaule=3: NonBlockingServer")
            .required(false)
            .repeatable(false)
            .argument("vaule")
            .callback(OptionCallback<userProfileServer>(
            this, &userProfileServer::handleServer)));
}

void userProfileServer::handleHelp(const std::string& name,
        const std::string& value) {
    HelpFormatter helpFormatter(options());
    helpFormatter.setCommand(commandName());
    helpFormatter.setUsage("OPTIONS");
    helpFormatter.setHeader(
            "UserProfile Manager Server");

    helpFormatter.format(std::cout);
    stopOptionsProcessing();
    _helpRequested = true;
}

void userProfileServer::handleServer(const std::string& name,
        const std::string& value) {
    _serverRequested = atoi(value.c_str());
    if (_serverRequested < 0 or _serverRequested > 3) {
        _serverRequested = 0;
    }
}

void userProfileTask::startSimpleServer(boost::shared_ptr<TProcessor> processor,boost::shared_ptr<TServerTransport> serverTransport,
        boost::shared_ptr<TTransportFactory> transportFactory,boost::shared_ptr<TProtocolFactory> protocolFactory) {
    
    ServerApplication::instance().logger().information("start SimpleServer");
    simpleServer = new TSimpleServer(processor, serverTransport, transportFactory, protocolFactory);
    simpleServer->serve();
};

void userProfileTask::startThreadedServer(boost::shared_ptr<TProcessor> processor,boost::shared_ptr<TServerTransport> serverTransport,
        boost::shared_ptr<TTransportFactory> transportFactory,boost::shared_ptr<TProtocolFactory> protocolFactory) {
    
    ServerApplication::instance().logger().information("start ThreadedServer");
    boost::shared_ptr<::apache::thrift::concurrency::PosixThreadFactory> threadFactory =
            boost::shared_ptr<::apache::thrift::concurrency::PosixThreadFactory>(new ::apache::thrift::concurrency::PosixThreadFactory());
    threadedServer = new TThreadedServer(processor, serverTransport, transportFactory, protocolFactory, threadFactory);
    threadedServer->serve();
};

void userProfileTask::startThreadPoolServer(boost::shared_ptr<TProcessor> processor,boost::shared_ptr<TServerTransport> serverTransport,
        boost::shared_ptr<TTransportFactory> transportFactory,boost::shared_ptr<TProtocolFactory> protocolFactory) {
    
    ServerApplication::instance().logger().information("start ThreadPoolServer");
    boost::shared_ptr<ThreadManager> threadManager = ThreadManager::newSimpleThreadManager(numServerThread);
    boost::shared_ptr<::apache::thrift::concurrency::PosixThreadFactory> threadFactory =
            boost::shared_ptr<::apache::thrift::concurrency::PosixThreadFactory>(new ::apache::thrift::concurrency::PosixThreadFactory());
    threadManager->threadFactory(threadFactory);
    threadManager->start();
    threadPoolServer = new TThreadPoolServer(processor, serverTransport, transportFactory, protocolFactory, threadManager);
    threadPoolServer->serve();
};

void userProfileTask::startNonBlockingServer(boost::shared_ptr<TProcessor> processor,boost::shared_ptr<TProtocolFactory> protocolFactory) {
    
    ServerApplication::instance().logger().information("start NonBlockingServer");
    //boost::shared_ptr<TNonblockingServerTransport> nonblockserverTransport(new TNonblockingServerSocket(port));
    boost::shared_ptr<ThreadManager> threadManager = ThreadManager::newSimpleThreadManager(numServerThread);
    boost::shared_ptr<::apache::thrift::concurrency::PosixThreadFactory> threadFactory =
            boost::shared_ptr<::apache::thrift::concurrency::PosixThreadFactory>(new ::apache::thrift::concurrency::PosixThreadFactory());
    threadManager->threadFactory(threadFactory);
    threadManager->start();
    nonBlockingServer = new TNonblockingServer(processor, protocolFactory, port, threadManager);
    nonBlockingServer->setNumIOThreads(numIOThread);
    nonBlockingServer->serve();
};

void userProfileTask::runTask() {
    
    boost::shared_ptr<TProcessor> processor(new ProfileServicesProcessor(handler));
    boost::shared_ptr<TServerTransport> serverTransport(new TServerSocket(port));
    boost::shared_ptr<TTransportFactory> transportFactory(new TBufferedTransportFactory());
    boost::shared_ptr<TProtocolFactory> protocolFactory(new TBinaryProtocolFactory());

    int choose = _serverRequested;
    if (choose == 0) {
        startSimpleServer(processor, serverTransport, transportFactory, protocolFactory);
    } else if (choose == 1) {
        startThreadedServer(processor, serverTransport, transportFactory, protocolFactory);
    } else if (choose == 2) {
        startThreadPoolServer(processor, serverTransport, transportFactory, protocolFactory);
    } else if (choose == 3) {
        startNonBlockingServer(processor, protocolFactory);
    }
    
}
void userProfileTask::cancel(){
    if (_serverRequested == 0) {
        simpleServer->stop();
    } else if (_serverRequested == 1) {
        threadedServer->stop();
    } else if (_serverRequested == 2) {
        threadPoolServer->stop();
    } else if (_serverRequested == 3) {
        nonBlockingServer->stop();
    }
    
    //handler->shutdownWorker();
    
    Poco::Task::cancel();
}
int userProfileServer::main(const ArgVec& args) {
    if (_helpRequested) return Application::EXIT_OK;
    
    _handler = boost::shared_ptr<SocialServiceHandler>(&getSubsystem<SocialServiceHandler>());
        
    Poco::ThreadPool pool;
    Poco::TaskManager tm(pool);
    
    tm.start(new userProfileTask(_serverRequested,port,numServerThread,numIOThread,_handler));
    waitForTerminationRequest();
    
    tm.cancelAll();
    
    tm.joinAll();
    return Application::EXIT_OK;
}

