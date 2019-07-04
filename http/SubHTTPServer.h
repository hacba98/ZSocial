/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   SubHTTPServer.h
 * Author: cpu10855
 *
 * Created on July 4, 2019, 9:37 AM
 */

#ifndef SUBHTTPSERVER_H
#define SUBHTTPSERVER_H

#include "Poco/Util/Application.h"
#include "Poco/Util/Subsystem.h"
#include "Poco/Util/OptionSet.h"
#include "Poco/Logger.h"
#include "Poco/Exception.h"
#include "Poco/Net/HTTPServer.h"
#include "Poco/ThreadPool.h"
#include "Poco/Timespan.h"

#include "thrift/TToString.h"

#include <boost/shared_ptr.hpp>

#include "./ZRequestHandler.h"

class SubHTTPServer : public Poco::Util::Subsystem {
public:
	SubHTTPServer();
	virtual ~SubHTTPServer();
	
	// control method
	void start(); // start process
	void run();
	void terminate();
	inline bool isRunning(){ return _running; }
	
protected:
	// method of subsystems
	virtual void initialize (Poco::Util::Application& app);
	virtual void uninitialize ();
	virtual void defineOptions(Poco::Util::OptionSet& options);
	virtual const char * name() const;
		
	// fast initialize
	void initCommon();
private:
	// env
	int _listenPort;
	bool _running;
	
	// tcp server parameters
	Poco::Timespan _threadIdleTime;
	int _maxThreads;
	int _maxQueued;
	
	// http server parameters
	int _minConnectionPool;
	int _maxConnectionPool;
	Poco::Timespan _timeout; // micro seconds
	bool _keepAlive;
	int _maxKeepAliveRequests;
	Poco::Timespan _keepAliveTimeout; //micro seconds
	
	boost::shared_ptr<Poco::Net::HTTPServer> _core;
};

#endif /* SUBHTTPSERVER_H */

