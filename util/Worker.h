/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   Worker.h
 * Author: cpu02331
 *
 * Created on May 21, 2019, 3:45 PM
 */

#ifndef WORKER_H
#define WORKER_H

#include "Poco/Notification.h"
#include "Poco/NotificationQueue.h"
#include "Poco/ThreadPool.h"
#include "Poco/Runnable.h"
#include "Poco/AutoPtr.h"
#include "Poco/Util/ServerApplication.h"
#include "services/SocialServiceHandler.h"
#include "src/gen-cpp/ProfileServices.h"

using Poco::Notification;
using Poco::NotificationQueue;
using Poco::ThreadPool;
using Poco::AutoPtr;

class SocialServiceHandler;

class CreatNotification : public Notification {
public:
CreatNotification(UserProfile data,int id) : _data(data) ,_id(id){
    }

    UserProfile data() const {
        return _data;
    }
    int getId() const {
        return _id;
    }
private:
    UserProfile _data;
    int _id;
};

class UpdateNotification : public Notification {
public:
UpdateNotification(int id,UserProfile data) : _id(id),_data(data) {
    }
    int getId() const {
        return _id;
    }
    UserProfile data() const {
        return _data;
    }
private:
    int _id;
    UserProfile _data;
};

class DeleteNotification : public Notification {
public:
DeleteNotification(int id) : _id(id) {
    }
    int getId() const {
        return _id;
    }
private:
    int _id;
};

class Worker : public Poco::Runnable {
public:
    NotificationQueue& _queue;
    SocialServiceHandler* H;
    Worker(NotificationQueue& queue ) : _queue(queue) {
    }

    void run() ;
};

class WorkerSystem : public Poco::Util::Subsystem{
protected:
    
    void initialize ( Poco::Util::Application &self );
    void reinitialize ( Poco::Util::Application &self );
    void uninitialize();

    const char* name() const;
public:
    NotificationQueue queue;
    Poco::SharedPtr<Poco::SharedPtr<Worker>,Poco::ReferenceCounter,Poco::ReleaseArrayPolicy<Poco::SharedPtr<Worker>>> workers;
    int NumWorker;
private:
    void shutdownWorker();
};
#endif /* WORKER_H */

