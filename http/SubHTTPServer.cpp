/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   SubHTTPServer.cpp
 * Author: cpu10855
 * 
 * Created on July 4, 2019, 9:37 AM
 */


#include "SubHTTPServer.h"

using namespace std;
using namespace Poco;
using namespace Poco::Util;
using namespace Poco::Net;

SubHTTPServer::SubHTTPServer() {
	initCommon();
}

SubHTTPServer::~SubHTTPServer() {
	if (isRunning())
		terminate();
}

void SubHTTPServer::initCommon(){
	_listenPort = 80;
	_running = false;
}

// control methods
void SubHTTPServer::run () {
	if (_running) {
		throw Poco::Exception("Server is already start", Application::EXIT_NOHOST);
	}
	_running = true;
	return _core->start();
}

void SubHTTPServer::terminate(){
	if(_running){
		_core->stop();
	}
	_running = false;
}

// subsystem method
const char * SubHTTPServer::name () const {
	return "Subsystem-HTTP Server";
}

void SubHTTPServer::initialize(Application& app){
	app.logger().information(Logger::format("Initializing $0 .......", name()));
	try {
		// loading configurations
		_threadIdleTime = Poco::Timespan(app.config().getInt("http.threadIdleTime", 25000000));
		_maxThreads = app.config().getInt("http.maxThreads", 128);
		_maxQueued = app.config().getInt("http.maxWaitingQueue", 100);
		_minConnectionPool = app.config().getInt("http.minConnection", 16);
		_maxConnectionPool = app.config().getInt("http.maxConnection", 128);
		_timeout = Poco::Timespan(app.config().getInt("http.requestTimeout", 60000000));
		_keepAlive = app.config().getBool("http.isKeepAlive", true);
		_maxKeepAliveRequests = app.config().getInt("http.maxKeepAliveRequests", 0); // 0 = unlimited
		_keepAliveTimeout = Poco::Timespan(app.config().getInt("http.keepAliveTimeout", 15000000));
		
		// running port = 8000 in dev mode, 80 in prod mode
		_listenPort = app.config().getBool("http.isProductMode", false) ? 80 : 8000;
		
		HTTPServerParams *params = new HTTPServerParams;
		// set TCP parameters
		params->setThreadIdleTime(_threadIdleTime);
		params->setMaxThreads(_maxThreads);
		params->setMaxQueued(_maxQueued);
		
		// set HTTP parameters
		params->setServerName("ZSocial.WebServer");
		params->setTimeout(_timeout);
		params->setKeepAlive(_keepAlive);
		params->setKeepAliveTimeout(_keepAliveTimeout);
		params->setMaxKeepAliveRequests(_maxKeepAliveRequests);
		
		// thread pool for connection pool - threadpool for accepting incoming requests
		Poco::ThreadPool connectionPool("connectionPool", _minConnectionPool, _maxConnectionPool);
		// server socket
		ServerSocket listenSocket(_listenPort);
		
		boost::shared_ptr<HTTPServer> server(new HTTPServer
			(new ZRequestHandlerFactory, connectionPool, listenSocket, params));
		
		_core = server;
		
		if (!_core.get()){
			throw new Poco::Exception("Setting up HTTP Server failed");
		}
		
	} catch (Poco::Exception e){
		app.logger().error(e.message());
	}
	
	return app.logger().information(Logger::format("Initializing $0 .....OK \xe2\x99\xaa", name()));
}

void SubHTTPServer::uninitialize(){
	Application &app = Application::instance();
	app.logger().information(Logger::format("Un-initializing $0 .......", name()));
	
	// stop server if it is currently running
	terminate();

	app.logger().information(Logger::format("Un-initializing $0 .......OK \xe2\x99\xaa", name()));

}

void SubHTTPServer::defineOptions(Poco::Util::OptionSet& options){
	Subsystem::defineOptions(options);
	
	options.addOption(
		Option("httphelp", "httpH", "display help about HTTP server")
		.required(false)
		.repeatable(false));
}

void SubHTTPServer::start(){
	using apache::thrift::to_string;
	
	try {
		if (!isRunning()){
			// log info
			Application::instance().logger().information(
				Logger::format("Web Server is running at port $0", to_string(_listenPort)));
			// starting server
			run();
		} else {
			throw new Poco::Exception("Server is already running");
		}
	} catch (Poco::Exception e){
		Application::instance().logger().error(e.message());
	}
}