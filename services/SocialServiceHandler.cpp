/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

#include <Poco/Util/Application.h>

#include "src/gen-cpp/ProfileServices.h"
#include "util/Worker.h"

SocialServiceHandler::SocialServiceHandler()  {
    //hit = 0;
    //all = 0;
}

SocialServiceHandler::~SocialServiceHandler() {
}

void SocialServiceHandler::initialize ( Poco::Util::Application &self ){
    log = &self.logger();
    
    profileQueue = &self.getSubsystem<WorkerSystem>().queue;
    
    int b = self.config().getInt("CacheBound",300);
    users.setBound(b);
    log->information("Cache length: %d" , b );
    
    db = &self.getSubsystem<UserProfileDB>();
    newID = db->getNextID();
    log->information("nextID will be: %d" , newID);
};
void SocialServiceHandler::reinitialize ( Poco::Util::Application &self ){
    Poco::Util::Subsystem::reinitialize(self);
};
void SocialServiceHandler::uninitialize(){   
    log->information("uninitialize handler");
};


const char* SocialServiceHandler::name() const{
    return "SocialService Handler";
};

int32_t SocialServiceHandler::Login(const std::string& username, const std::string& password){};
ErrorCode::type SocialServiceHandler::Logout(const int32_t userId){};

void SocialServiceHandler::CreateProfile(CreateUserResult& _return, const UserProfile& profile){

    UserProfile newProfile(profile);
    mutexUpdate.lock();
    newProfile.id = newID++;
    db->setNextID(newID);
    mutexUpdate.unlock();
    
    //TODO check userName is unique
    
    now.update();
    newProfile.last_active_time = newProfile.join_date = (int)now.epochTime();
    
    _return.errorCode = db->setDB(newProfile.id, newProfile);
    db->setUsername(newProfile.username,newProfile.id);
    
    users.add(newProfile.id, newProfile);
    _return.id = newProfile.id;
    log->debug("Create");
}

void SocialServiceHandler::GetProfile(GetUserResult& _return, const int32_t userId) {
    
    if(users.get(userId,_return.profile)){
        _return.errorCode = ErrorCode::type::SUCCESS;
        _return.profile.id = userId;    
    }else{
        
        _return.errorCode = db->getDB(userId, _return.profile);
        
        if (_return.errorCode == ErrorCode::type::SUCCESS) {
            users.add(userId, _return.profile);
        }
    }
    
    log->debug("Get");
}

ErrorCode::type SocialServiceHandler::UpdateProfile(const UserProfile& profile, const int32_t userId) {
    if(db->isContainDB(userId)) return ErrorCode::type::DUPLICATED_REQUEST;
    users.remove(userId);
    
    log->debug("Update");
    return db->setDB(userId, profile);
}

ErrorCode::type SocialServiceHandler::DeleteProfile(const int32_t userId) {
    UserProfile profile;
    GetProfile(profile,userId);
    
    users.remove(userId);
    
    db->rmUsername(profile.username);
    log->debug("Delete");
    return db->removeDB(userId);
}

void SocialServiceHandler::ansyCreateProfile(CreateUserResult& _return, const UserProfile& profile) {
    mutexUpdate.lock();
    int id = newID++;
    db->setNextID(newID);
    mutexUpdate.unlock();
    profileQueue->enqueueNotification(new CreatNotification(profile, id));
    log->debug("ansyCreate");
    _return.errorCode = ErrorCode::type::SUCCESS;
    _return.id = id;
}
void SocialServiceHandler::CreateWithId(int userId,const UserProfile& profile){
    db->setDB(userId, profile);
    db->setUsername(profile.username,userId);
    log->debug("Create");
}
ErrorCode::type SocialServiceHandler::ansyUpdateProfile(const UserProfile& profile, const int32_t userId) {
    profileQueue->enqueueNotification(new UpdateNotification(userId,profile));
    log->debug("ansyUpdate");
    return ErrorCode::type::SUCCESS;
}


ErrorCode::type SocialServiceHandler::ansyDeleteProfile(const int32_t userId) {
    profileQueue->enqueueNotification(new DeleteNotification(userId));
    log->debug("ansyDelete");
    return ErrorCode::type::SUCCESS;
}

void SocialServiceHandler::getList(ListProfileResult& _return, const std::vector<int32_t> & ids){
    for(std::vector<int32_t>::const_iterator i = ids.begin();i != ids.end();i++){
        SimpleProfile res;
        GetSimpleProfile(res,*i);
        
        _return.profiles.push_back(res);
        
    }
    _return.errorCode = ErrorCode::type::SUCCESS;
    log->debug("Get List");
};
int SocialServiceHandler::GetSimpleProfile(SimpleProfile& _ret,int32_t userId){
    GetUserResult res;
    GetProfile(res,userId);
    if (res.errorCode == ErrorCode::type::SUCCESS){
        _ret.id = res.profile.id;
        _ret.gender = res.profile.gender;
        _ret.last_active_time = res.profile.last_active_time;
        _ret.name = res.profile.name;
    }
    return res.errorCode;
}
int32_t SocialServiceHandler::GetIdByName(const std::string& username){
    int ret;
    if (db->getIdByName(ret,username) == ErrorCode::SUCCESS){
        return -1;
    }
    
    log->debug("Get by Name");
    return ret;
}
