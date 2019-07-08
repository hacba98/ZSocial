#include <iostream>

#include <thrift/protocol/TBinaryProtocol.h>
#include <thrift/transport/TSocket.h>
#include <thrift/transport/TTransportUtils.h>

#include "services/SocialServiceHandler.h"

using namespace std;
using namespace apache::thrift;
using namespace apache::thrift::protocol;
using namespace apache::thrift::transport;

void createTask(userProfileServiceClient client , bool asyn = false){ 
    string name, gender;
    int d,m,y;
    cout << "Enter Your Name: " ;
    cin >> name;
    cout << endl <<"Enter Your Gender: " ;
    cin >> gender;
    cout << endl << "Enter Your BirthDay: " << endl << "Day : " ;
    cin >> d;
    cout  << "Month : " ;
    cin >> m;
    cout  << "Year : " ;
    cin >> y;
    UserProfile profile(name, gender, d, m, y);
    if (profile.birth == -1) {
        cout << "you enter a invalid birthday" << endl;
        return;
    }
    if(!asyn){
        userProfileResult ret;
        client.Create(ret, profile);
        if (ret.errorCode == 0) {
            cout << "success create new UserProfile with id :"
                    << ret.profile.id << endl;
        }
    }else{
        int id = client.ansyCreate(profile);
        cout << "success send request, your user may have id : " << id << endl;
    }
}

void updateTask(userProfileServiceClient client , bool asyn = false){
    int id;
    cout << "Enter id: " ;
    cin >> id;
    string name, gender;
    int d,m,y;
    cout << endl << "Enter Your Name: " ;
    cin >> name;
    cout << endl <<"Enter Your Gender: " ;
    cin >> gender;
    cout << endl << "Enter Your BirthDay: " << endl << "Day : " ;
    cin >> d;
    cout  << "Month : " ;
    cin >> m;
    cout  << "Year : " ;
    cin >> y;
    userProfile profile(name,gender,d,m,y);
    if(!asyn){
        int retCode = client.Update(id , profile);
        if(retCode == 0){
            cout << "success update id :" << id << endl;
        }else if(retCode == 1){
            cout << "this user id is not in List" << endl;
        }
    }else{
        client.ansyUpdate(id , profile);
        cout << "success send request" << endl;
    }
};

void getTask(userProfileServiceClient client){
    int id;
    cout << "Enter id: " ;
    cin >> id;
    userProfileResult ret;
    client.Get(ret , id);
    if(ret.errorCode == 0){
        ret.profile.toString();
    }else if(ret.errorCode == 1){
        cout << "this user id is not in List" << endl;
    }
};

void deleteTask(userProfileServiceClient client ,bool asyn = false){
    int id;
    cout << "Enter id: " ;
    cin >> id;
    
    if(!asyn){
        int retCode = client.Delete(id);
        if(retCode == 0){
            cout << "success delete id :" << id << endl;
        }else if(retCode == 1){
            cout << "this user id is not in List" << endl;
        }
    }else{
        client.ansyDelete(id);
        cout << "success send request" << endl;
    }
};

void getListTask(userProfileServiceClient client){
    int n;
    cout << "How many user : ";
    cin >> n;
    vector<int> ids;
    for (int i =0 ; i<n;i++){
        int val;
        cout << "ids[" << i <<"]= ";
        cin >> val;
        ids.push_back(val);
    }
    userProfileListResult ret;
    client.getList(ret,ids);
    for(vector<userProfileResult>::iterator i = ret.profiles.begin(); i != ret.profiles.end(); i++){
        if ((*i).errorCode == 0){
            (*i).profile.toString();
        }else{
            cout << "this user id is not in List" << endl;
        }
    }
}


int main() {
  std::shared_ptr<TTransport> socket(new TSocket("localhost", 9090));
  //std::shared_ptr<TTransport> transport(new TFramedTransport(socket));
  std::shared_ptr<TTransport> transport(new TBufferedTransport(socket));
  std::shared_ptr<TProtocol> protocol(new TBinaryProtocol(transport));
  userProfileServiceClient client(protocol);

  try {
      transport->open();
      int QUIT = 0;
      while(QUIT == 0){
          cout << "___________________________" << endl
                  <<"Enter 1 to create:" << endl
                  << "Enter 2 to get:" << endl
                  << "Enter 3 to update:" << endl
                  << "Enter 4 to delete:" << endl
                  << "Enter 5 to asynchronous create" << endl
                  << "Enter 6 to asynchronous update" << endl
                  << "Enter 7 to asynchronous delete" << endl
                  << "Enter 8 to get List user" << endl
                  << "Enter other number to quit:" << endl;
          int choose;
          cin >> choose;
          if(choose == 1){createTask(client);}
          else if(choose == 2){ getTask(client); }
          else if(choose == 3) { updateTask(client);}
          else if(choose == 4){deleteTask(client);}
          else if(choose == 5){createTask(client,true);}
          else if(choose == 6){updateTask(client,true);}
          else if(choose == 7){deleteTask(client,true);}
          else if(choose == 8){getListTask(client);}
          else {
              QUIT = 1;
          }
      }
      
      

      transport->close();
  } catch (TException& tx) {
    cout << "ERROR: " << tx.what() << endl;
  }
}