/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   SubHandler.cpp
 * Author: cpu10855
 * 
 * Created on May 22, 2019, 2:44 PM
 */

#include "SubHandler.h"

using namespace std;
using namespace Poco;
using namespace Poco::Util;
using namespace apache::thrift;
using namespace apache::thrift::transport;
using namespace apache::thrift::protocol;
using namespace apache::thrift::server;
using namespace apache::thrift::concurrency;

SubHandler::SubHandler() {
}

SubHandler::~SubHandler() {
}


const char * SubHandler::name () const {
	return "Subsystem-Server Handler";
}

void SubHandler::initialize(Poco::Util::Application& app){
	app.logger().information(Logger::format("Initializing $0 .......", name()));
	
	// construct transport & protocol layers
	try {
		// load configurations
		_port = app.config().getInt("server.port", 8080);
		_maxThread = app.config().getInt("server.maxThread", 24);
		_maxIOThread = app.config().getInt("server._maxIOThread", 8);
		_maxPendingTask = app.config().getInt("server.maxPendingTask", 0); //default 0 = no limit
		
		boost::shared_ptr<TMultiplexedProcessor> processors(new TMultiplexedProcessor());
		
		// adding processor
		processors->registerProcessor("FriendServices", 
			boost::shared_ptr<TProcessor>(new FriendServicesProcessor(
				boost::shared_ptr<HandlerFriendServices>(new HandlerFriendServices))));
		
//		processors->registerProcessor("ProfileServices", 
//			boost::shared_ptr<TProcessor>(new FriendServicesProcessor(
//				boost::shared_ptr<HandlerProfileServices>(new HandlerProfileServices))));
		
		// create Nonblocking server
		// transport , transport factory & protocol factory
		boost::shared_ptr<TProtocolFactory> protocolFactory (new TBinaryProtocolFactory());
		
		// create thread manager
		boost::shared_ptr<PosixThreadFactory> threadFactory =
			boost::shared_ptr<PosixThreadFactory>(new PosixThreadFactory());
		
		boost::shared_ptr<ThreadManager> threadManager = ThreadManager::newSimpleThreadManager(_maxThread, _maxPendingTask);
		threadManager->threadFactory(threadFactory);
		threadManager->start();
		
		boost::shared_ptr<TNonblockingServer> core(
			new TNonblockingServer(processors, protocolFactory, _port, threadManager));
		
		core->setNumIOThreads(_maxIOThread);
		
		_core = core;
		
		if (!_core)
			throw "Setting up server failed.";
	} catch (string e){
		throw new Poco::Exception(e, Application::EXIT_SOFTWARE);
	}
	
	app.logger().information(Logger::format("Initializing $0 .....OK \xe2\x99\xaa", name()));
}

void SubHandler::uninitialize() {
	// TODO
}

void SubHandler::defineOptions(Poco::Util::OptionSet& options){
	Subsystem::defineOptions(options);
	
	options.addOption(
		Option("svhelp", "svh", "display help about thrift server")
		.required(false)
		.repeatable(false));
}

void SubHandler::run(){
	if (_running) {
		throw Poco::Exception("Server is already start", Application::EXIT_NOHOST);
	}
	_running = true;
	return _core->run();
}

void SubHandler::terminate(){
	if(_running){
		_core->stop();
	}
	
	_running = false;
}

using apache::thrift::to_string;
void SubHandler::start(){
	try{
		Poco::Util::Application::instance().logger().information(Logger::format("Server is starting at port $0", to_string(_port)));
		this->run();
	} catch (Poco::Exception e){
		Poco::Util::Application::instance().logger().error(e.message());
	}
}









