/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   Connection.h
 * Author: cpu10855
 *
 * Created on July 8, 2019, 5:30 PM
 */

#ifndef CONNECTION_H
#define CONNECTION_H

#include <thrift/protocol/TBinaryProtocol.h>
#include <thrift/transport/TSocket.h>
#include <thrift/transport/TTransportUtils.h>

#include "Poco/Util/Application.h"

#include "../gen-cpp/ProfileServices.h"

class ProfileConnection {
public:
	ProfileConnection(){
		int profile_services_port = Poco::Util::Application::instance().config().getInt("services.profile.port", 2020);
		boost::shared_ptr<apache::thrift::transport::TTransport> socket(new apache::thrift::transport::TSocket("localhost", profile_services_port));
		boost::shared_ptr<apache::thrift::transport::TTransport> transport(new apache::thrift::transport::TFramedTransport(socket)); 
		boost::shared_ptr<apache::thrift::protocol::TProtocol> protocol(new apache::thrift::protocol::TBinaryProtocol(transport));
		try{
                    transport->open();
                }catch(std::exception e){
                    Poco::Util::Application::instance().logger().information(e.what());
                }
		boost::shared_ptr<ProfileServicesClient> client(new ProfileServicesClient(protocol));
		_client = client;
}
	
	~ProfileConnection(){
		_client->getInputProtocol()->getTransport()->close();
	}
	
	ProfileServicesClient * client() {
		return _client.get();
	}
private:
	boost::shared_ptr<ProfileServicesClient> _client;
};

#include "../gen-cpp/FriendServices.h"
class FriendConnection {
public:
	FriendConnection(){
		int profile_services_port = Poco::Util::Application::instance().config().getInt("services.friend.port", 3030);
		boost::shared_ptr<apache::thrift::transport::TTransport> socket(new apache::thrift::transport::TSocket("localhost", profile_services_port));
		boost::shared_ptr<apache::thrift::transport::TTransport> transport(new apache::thrift::transport::TFramedTransport(socket)); 
		boost::shared_ptr<apache::thrift::protocol::TProtocol> protocol(new apache::thrift::protocol::TBinaryProtocol(transport));
		
		transport->open();
		boost::shared_ptr<FriendServicesClient> client(new FriendServicesClient(protocol));
		_client = client;
	}
	
	~FriendConnection(){
		_client->getInputProtocol()->getTransport()->close();
	}
	
	FriendServicesClient * client() {
		return _client.get();
	}
private:
	boost::shared_ptr<FriendServicesClient> _client;
};

#include "../gen-cpp/NewsFeedService.h"
class NewsFeedConnection{
public:
	NewsFeedConnection(){
		int newsfeed_services_port = Poco::Util::Application::instance().config().getInt("services.feed.port", 4040);
		boost::shared_ptr<apache::thrift::transport::TTransport> socket(new apache::thrift::transport::TSocket("localhost", newsfeed_services_port));
		boost::shared_ptr<apache::thrift::transport::TTransport> transport(new apache::thrift::transport::TFramedTransport(socket)); 
		boost::shared_ptr<apache::thrift::protocol::TProtocol> protocol(new apache::thrift::protocol::TBinaryProtocol(transport));
		
		transport->open();
		boost::shared_ptr<NewsFeedServiceClient> client(new NewsFeedServiceClient(protocol));
		_client = client;
	}
	
	~NewsFeedConnection(){
		_client->getInputProtocol()->getTransport()->close();
	}
	
	NewsFeedServiceClient * client(){
		return _client.get();
	}
	
private:
	boost::shared_ptr<NewsFeedServiceClient> _client;
};

#endif /* CONNECTION_H */

