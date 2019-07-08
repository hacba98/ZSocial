
#include <iostream>
#include <sstream>
#include <chrono>
#include <thrift/protocol/TBinaryProtocol.h>
#include <thrift/protocol/TJSONProtocol.h>
#include <thrift/transport/TSocket.h>
#include <thrift/transport/TTransportUtils.h>
#include "Poco/Random.h"
#include "Poco/Runnable.h"
#include "Poco/ThreadPool.h"
#include "Poco/Util/Application.h"
#include "Poco/Util/Option.h"
#include "Poco/Util/OptionSet.h"
#include "Poco/Util/HelpFormatter.h"
#include "Poco/Util/AbstractConfiguration.h"
#include "Poco/AutoPtr.h"
#include "userProfileService.h"
#include <Poco/DateTimeFormatter.h>

using namespace std;
using Poco::Random;
using namespace apache::thrift;
using namespace apache::thrift::protocol;
using namespace apache::thrift::transport;
using Poco::Util::Application;
using Poco::Util::Option;
using Poco::Util::OptionSet;
using Poco::Util::HelpFormatter;
using Poco::Util::AbstractConfiguration;
using Poco::Util::OptionCallback;
using Poco::AutoPtr;


class callRequest : public Poco::Runnable{
public:
    int num;
    int numReq,port,_transport;
    string host;
    callRequest(int n,int nR,int p,int tr,string h):num(n),numReq(nR),port(p),_transport(tr),host(h){
        
    }

    int getTime(int d, int m, int y) {
        time_t rawtime;
        struct tm * timeinfo;

        time(&rawtime);
        timeinfo = localtime(&rawtime);
        timeinfo->tm_year = y - 1900;
        timeinfo->tm_mon = m - 1;
        timeinfo->tm_mday = d;

        return int(mktime(timeinfo));
    }

    void getDMY(int birth,int &d, int &m, int &y) {
        time_t b = (time_t)birth;
        tm *ltm = localtime(&b);
        d = ltm->tm_mday;
        m = ltm->tm_mon + 1;
        y = ltm->tm_year + 1900;
    }

    virtual void run() {
        std::shared_ptr<TTransport> socket(new TSocket(host, port));
        std::shared_ptr<TTransport> transport;
        if(_transport == 1){
            transport = std::shared_ptr<TTransport>(new TBufferedTransport(socket));
        }else{        
            transport = std::shared_ptr<TTransport>(new TFramedTransport(socket));
        }
        std::shared_ptr<TProtocol> protocol(new TBinaryProtocol(transport));
        userProfileServiceClient client(protocol);

        try {
            transport->open();
            Random rnd;
            rnd.seed();
            //chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();
            userProfile profile;
            profile.__set_name("Client");
            profile.__set_gender(true);
            //profile.__set_username("Client???");
            profile.__set_password("123123123");
            profile.__set_phoneNumber(1900100315);
            profile.__set_birth(getTime(1,1,1991));
            
            //for (int i = 0; i < numReq; i++) {

            //    if (i % 2000 < 1000) {
            //        userProfileResult ret;
            //        client.Get(ret,150150 + rnd.next(150));
            //        client.Update(150150 + rnd.next(150), profile);
            //    } else {
            //        client.Update(150150 + rnd.next(150), profile);
            //    }
            //}

            for (int i = 0; i < numReq; i++) {
                userProfileResult ret;
                client.Get(ret, 150150 + rnd.next(150));
            }
            //z
            //for (int i = 0; i < numReq; i++) {
            //        client.Update(150150 + rnd.next(150),profile);
            //}

            //chrono::steady_clock::time_point end = chrono::steady_clock::now();
            //cout << "Time difference = " << chrono::duration_cast< chrono::microseconds>(end - begin).count() / 1000000.0 << endl;
            transport->close();
        } catch (TException& tx) {
            cout << "ERROR: " << tx.what() << endl;
        }
    }
};


class multiThreadClient: public Application
{
public:
	multiThreadClient(): _helpRequested(false),SO_THREAD(1)
	{
	}

protected:	
	void initialize(Application& self)
	{
            if(config().getInt("isLoad",0) == 0){
		loadConfiguration("client.properties");
            } 
            if(config().getInt("numberThread",0) > 0){
                SO_THREAD = config().getInt("numberThread",1);
            }
            Application::initialize(self);
	}
	
	void uninitialize()
	{
		Application::uninitialize();
	}
	
	void reinitialize(Application& self)
	{
		Application::reinitialize(self);
	}
	
	void defineOptions(OptionSet& options)
	{
		Application::defineOptions(options);

		options.addOption(
			Option("help", "h", "display help information on command line arguments")
				.required(false)
				.repeatable(false)
				.callback(OptionCallback<multiThreadClient>(this, &multiThreadClient::handleHelp)));

		options.addOption(
			Option("define", "D", "define a configuration property")
				.required(false)
				.repeatable(true)
				.argument("name=value")
				.callback(OptionCallback<multiThreadClient>(this, &multiThreadClient::handleDefine)));
				
		options.addOption(
			Option("config-file", "f", "load configuration data from a file")
				.required(false)
				.repeatable(true)
				.argument("file")
				.callback(OptionCallback<multiThreadClient>(this, &multiThreadClient::handleConfig)));

		options.addOption(
			Option("bind", "b", "bind option value to test.property")
				.required(false)
				.repeatable(false)
				.argument("value")
				.binding("test.property"));
                options.addOption(
                    Option("thread", "t", "number of thread")
                        .required(false)
                        .repeatable(false)
                        .argument("value")
                        .callback(OptionCallback<multiThreadClient>(
                        this, &multiThreadClient::handleNumThread)));
	}
	
	void handleHelp(const std::string& name, const std::string& value)
	{
		_helpRequested = true;
		displayHelp();
		stopOptionsProcessing();
	}
	
	void handleDefine(const std::string& name, const std::string& value)
	{
		defineProperty(value);
	}
	
	void handleConfig(const std::string& name, const std::string& value)
	{
		loadConfiguration(value);
	}

        void handleNumThread(const std::string& name,
                const std::string& value) {
                SO_THREAD = atoi(value.c_str());
        }
		
	void displayHelp()
	{
		HelpFormatter helpFormatter(options());
		helpFormatter.setCommand(commandName());
		helpFormatter.setUsage("OPTIONS");
		helpFormatter.setHeader("A sample application that demonstrates some of the features of the Poco::Util::Application class.");
		helpFormatter.format(std::cout);
	}
	
	void defineProperty(const std::string& def)
	{
		std::string name;
		std::string value;
		std::string::size_type pos = def.find('=');
		if (pos != std::string::npos)
		{
			name.assign(def, 0, pos);
			value.assign(def, pos + 1, def.length() - pos);
		}
		else name = def;
		config().setString(name, value);
	}

	int main(const ArgVec& args)
	{
                //printProperties("");
		if (!_helpRequested)
		{
			    
                    Poco::ThreadPool Pool;
                    int i;
                    int p = config().getInt("port",9090);
                    int nR = config().getInt("numberRequest",10000);
                    int tr = config().getInt("transport",1);
                    string host = config().getString("host","localhost");
                    
                    for (i = 0; i < SO_THREAD; i++) {
                        callRequest *cr = new callRequest(i, nR, p, tr, host);
                        Pool.start(*cr);
                    }
                    Pool.joinAll();
                    
                    logger().information("Time is " + Poco::DateTimeFormatter::format(uptime()));
		}
                
		return Application::EXIT_OK;
	}
	
	void printProperties(const std::string& base)
	{
		AbstractConfiguration::Keys keys;
		config().keys(base, keys);
		if (keys.empty())
		{
			if (config().hasProperty(base))
			{
				std::string msg;
				msg.append(base);
				msg.append(" = ");
				msg.append(config().getString(base));
				logger().information(msg);
			}
		}
		else
		{
			for (AbstractConfiguration::Keys::const_iterator it = keys.begin(); it != keys.end(); ++it)
			{
				std::string fullKey = base;
				if (!fullKey.empty()) fullKey += '.';
				fullKey.append(*it);
				printProperties(fullKey);
			}
		}
	}
	
private:
	bool _helpRequested;
        int SO_THREAD;
};


POCO_APP_MAIN(multiThreadClient)