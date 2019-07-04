// Starting point of server

#include "Poco/Util/Util.h"
#include "Poco/Util/ServerApplication.h"
#include "Poco/Util/HelpFormatter.h"
#include "Poco/Util/OptionCallback.h"
#include "Poco/Util/IntValidator.h"
#include "Poco/Util/Subsystem.h"
#include "Poco/Process.h"
#include "Poco/AutoPtr.h"
#include "Poco/Logger.h"
#include "Poco/LogFile.h"
#include "Poco/FileChannel.h"
#include "Poco/TaskManager.h"
#include "Poco/Task.h"
#include "Poco/TaskNotification.h"
#include "Poco/Exception.h"
#include "Poco/Path.h"

#include <iostream>
#include <string>
#include <vector>

#include "../http/SubHTTPServer.h"

using namespace std;
using namespace Poco;
using namespace Poco::Util;

class RunTask : public Poco::Task {
public:

	RunTask(std::string name) : Poco::Task(name) {
	}

	void runTask() {
		// implement this for server starting process
	}

	void cancel() {
		// implement this for server shutting down process
	}
	
};

class SocialServer : public ServerApplication {
public:
	SocialServer() : _helpRequested(false) {
		// must register sub-systems in here to make daemon runnable 
		this->addSubsystem(new SubHTTPServer);
	}
	
	~SocialServer() {
	}
	
protected:
	void initialize(Application& self) {
		// implement this for subsystem initialize
		ServerApplication::initialize(self);
	}
	
	void uninitialize(){
		// implement this for subsystem un-initialize
		ServerApplication::uninitialize();	
	}
	
	void defineOptions(OptionSet& options) {
		ServerApplication::defineOptions(options);

		options.addOption(
			Option("help", "h", "display help information")
			.required(false)
			.repeatable(false));

		options.addOption(
			Option("configuration-file", "f", "load configuration data from a file")
			.required(true)
			.repeatable(true)
			.argument("file"));
	}
	
	void handleOption(const std::string& name, const std::string& value) {
		ServerApplication::handleOption(name, value);

		if (name == "help") {
			_helpRequested = true;
			stopOptionsProcessing();
		} else if (name == "configuration-file"){
			Poco::Path pConfig(value);
			if (Application::findFile(pConfig)) {
				loadConfiguration(pConfig.getFileName(), 0);
			} else {
				throw Poco::Exception("Cannot find configuration file.");
			}
		}
	}
	
	void displayHelp() {
		HelpFormatter helpFormatter(options());
		helpFormatter.setCommand(commandName());
		helpFormatter.setUsage("OPTIONS");
		helpFormatter.setHeader("This is Server for project ZSocial");
		helpFormatter.format(std::cout);
	}
	
	int main(const std::vector<string>& args) {
		if (_helpRequested) {
			displayHelp();
		} else {
			// Task manager version
			Poco::ThreadPool serverPool;
			Poco::TaskManager tm(serverPool);
			tm.start(new RunTask("application"));
			waitForTerminationRequest();
			tm.cancelAll();
			tm.joinAll();
		}
		return Application::EXIT_OK;
	}
	
private:
	bool _helpRequested;
};

typedef SocialServer _SocialNetwork_;

int main(int argc, char** argv) {
	_SocialNetwork_ application;
	return application.run(argc, argv);
}