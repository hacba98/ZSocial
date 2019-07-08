/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

#include "services/SocialServiceHandler.h"
#include "Worker.h"

void Worker::run() {
    AutoPtr<Notification> pNf(_queue.waitDequeueNotification());
    while (pNf) {
        CreatNotification* pCreateNf = dynamic_cast<CreatNotification*> (pNf.get());
        if (pCreateNf) {
            H->CreateWithId(pCreateNf->getId(),pCreateNf->data());
        }

        UpdateNotification* pUpdateNf = dynamic_cast<UpdateNotification*> (pNf.get());
        if (pUpdateNf) {
            H->UpdateProfile(pUpdateNf->data(),pUpdateNf->getId());
        }
        
        DeleteNotification* pDeleteNf = dynamic_cast<DeleteNotification*> (pNf.get());
        if (pDeleteNf) {
            H->DeleteProfile(pDeleteNf->getId());
        }
        pNf = _queue.waitDequeueNotification();
    }
    printf("already DONE!\n");
}

void WorkerSystem::initialize ( Poco::Util::Application &self ){
    NumWorker = self.config().getInt("NumWorker",2);
    self.logger().information("Number of Asynchronous worker: %d" , NumWorker );
    workers = new Poco::SharedPtr<Worker>[NumWorker];
    for(int i = 0 ;i < NumWorker ; i++ ){
        workers[i] = new Worker(queue);
        workers[i]->H = &self.getSubsystem<SocialServiceHandler>();
        ThreadPool::defaultPool().start(*workers[i]);
    }
};
void WorkerSystem::reinitialize ( Poco::Util::Application &self ){
    Poco::Util::Subsystem::reinitialize(self);
};
void WorkerSystem::uninitialize(){
    //Application::instance().logger().information("uninitialize worker");
    shutdownWorker();
};

const char* WorkerSystem::name() const{
    return "worker system";
};

void WorkerSystem::shutdownWorker(){
    while (!queue.empty()) {
        Poco::Thread::sleep(100);
    }
    Poco::Thread::sleep(100);
    queue.wakeUpAll();
    
};