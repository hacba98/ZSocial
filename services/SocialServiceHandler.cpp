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

void SocialServiceHandler::Login(loginResult& _return, const std::string& username, const std::string& password){
    int userId = GetIdByName(username);
    if (userId == -1){
        _return.code = ErrorCode::USER_NOT_FOUND;
        return;
    }
    GetUserResult profile;
    GetProfile(profile,userId);
    
    if (profile.profile.password == hash(password , userId)){
        _return.profile.id = profile.profile.id;
        _return.profile.gender = profile.profile.gender;
        _return.profile.last_active_time = profile.profile.last_active_time = -1;
        _return.profile.name = profile.profile.name;
        
        users.remove(userId);
        _return.code = db->setDB(userId,profile.profile);
    }else{
        _return.code = ErrorCode::USER_NOT_FOUND;
    }
    log->debug("Login");
};
ErrorCode::type SocialServiceHandler::Logout(const int32_t userId){
    GetUserResult profile;
    GetProfile(profile,userId);
    if (profile.errorCode != ErrorCode::SUCCESS) return profile.errorCode;
    profile.profile.last_active_time = (int)now.epochTime();
    
    users.remove(userId);
    log->debug("Logout");
    return db->setDB(userId,profile.profile);
};

void SocialServiceHandler::CreateProfile(CreateUserResult& _return, const UserProfile& profile){

    UserProfile newProfile(profile);
    mutexUpdate.lock();
    newProfile.id = newID++;
    db->setNextID(newID);
    mutexUpdate.unlock();
    
    //TODO check userName is unique
    
    now.update();
    newProfile.last_active_time = newProfile.join_date = (int)now.epochTime();
    newProfile.password = hash(newProfile.password,newProfile.id);
    
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
    
    GetUserResult newProfile;
    GetProfile(newProfile,userId);
    
    if(newProfile.errorCode != ErrorCode::SUCCESS) return ErrorCode::type::DUPLICATED_REQUEST;
    
    users.remove(userId);
    
    if (profile.birth != 0){
        newProfile.profile.__set_birth(profile.birth);
    }
    
    //if (profile.__isset.gender){
    //    newProfile.profile.__set_gender(profile.gender);
    //}
    
    if (profile.password != ""){
        newProfile.profile.__set_password(hash(profile.password,userId));
    }
    
    if (profile.phoneNumber != 0){
        newProfile.profile.__set_phoneNumber(profile.phoneNumber);
    }
    
    if (profile.name != ""){
        newProfile.profile.__set_name(profile.name);
    }
    
    if (profile.__isset.last_active_time != 0){
        newProfile.profile.__set_last_active_time(profile.last_active_time);
    }
    
    log->debug("Update");
    return db->setDB(userId, newProfile.profile);
}

ErrorCode::type SocialServiceHandler::DeleteProfile(const int32_t userId) {
    GetUserResult profile;
    GetProfile(profile,userId);
    
    users.remove(userId);
    
    db->rmUsername(profile.profile.username);
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
void SocialServiceHandler::CreateWithId(int userId,const UserProfile& _profile){
    UserProfile profile(_profile);
    profile.last_active_time = profile.join_date = (int)now.epochTime();
    profile.password = hash(profile.password,userId);
    
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
    if (db->getIdByName(ret,username) != ErrorCode::SUCCESS){
        return -1;
    }
    
    log->debug("Get by Name");
    return ret;
}

bool SocialServiceHandler::chechExist(const int32_t userId){
    return db->isContainDB(userId);
};