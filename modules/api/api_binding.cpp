/**
 * Appcelerator Kroll - licensed under the Apache Public License 2
 * see LICENSE in the root folder for details on the license.
 * Copyright (c) 2008 Appcelerator, Inc. All Rights Reserved.
 */
#include "api_binding.h"
#include "application_binding.h"
#include "component_binding.h"
#include "dependency_binding.h"
#include <algorithm>

using std::string;
using std::vector;
using std::pair;
using std::map;

namespace kroll
{
	APIBinding::APIBinding(Host* host) :
		host(host),
		global(host->GetGlobalObject()),
		record(0),
		logger(Logger::Get("API")),
		installerThread(0),
		installerThreadAdapter(0)
	{
		this->installerThreadAdapter = new Poco::RunnableAdapter<APIBinding>(
			*this, &APIBinding::RunInstaller);

		/**
		 * @tiapi(method=True,name=API.set,since=0.2)
		 * @tiapi Set an attribute in the global object
		 * @tiarg[string, key] Key of the attribute to set
		 * @tiarg[string, value] New value of the attribute
		 */
		this->SetMethod("set", &APIBinding::_Set);

		/**
		 * @tiapi(method=True,name=API.get,since=0.2)
		 * @tiapi Get an attribute in the global object
		 * @tiarg[string, key] Key of the attribute to get
		 * @tiresult[value] Attribute at that key
		 */
		this->SetMethod("get", &APIBinding::_Get);

		/**
		 * @tiapi(method=True,name=API.register,since=0.2)
		 * @tiapi Register an event listener
		 * @tiarg[string, eventName] The event name to listen for
		 * @tiarg[method, callback] The callback to invoke when this message occurs
		 * @tiresult[int] An id which represents this registration
		 */
		this->SetMethod("register", &APIBinding::_Register);

		/**
		 * @tiapi(method=True,name=API.unregister,since=0.2)
		 * @tiapi Unregister an event listener
		 * @tiarg[int, id] The id of the registration to unregister
		 */
		this->SetMethod("unregister", &APIBinding::_Unregister);

		/**
		 * @tiapi(method=True,name=API.fire,since=0.2)
		 * @tiapi Fire the event with a given name
		 * @tiarg[string, name] The name of the event to fire
		 * @tiarg[value, payload] The payload for this event
		 */
		this->SetMethod("fire", &APIBinding::_Fire);

		/**
		 * @tiapi(method=True,name=API.getApplication,since=0.2)
		 * @tiapi Get the currently running application
		 * @tiresult[object] an API.Application that is the running application
		 */
		this->SetMethod("getApplication", &APIBinding::_GetApplication);

		/**
		 * @tiapi(method=True,name=API.getInstalledComponents,since=0.4)
		 * @tiapi Get a list of the currently installed Kroll components
		 * @tiresult[list] a list of API.Component of installed components of all types
		 */
		this->SetMethod("getInstalledComponents", &APIBinding::_GetInstalledComponents);

		/**
		 * @tiapi(method=True,name=API.getInstalledSDKs,since=0.4)
		 * @tiapi Get a list of the currently installed Kroll SDK components
		 * @tiresult[list] a list of API.Component of installed SDK components
		 */
		this->SetMethod("getInstalledSDKs", &APIBinding::_GetInstalledSDKs);

		/**
		 * @tiapi(method=True,name=API.getInstalledMobileSDKs,since=0.4)
		 * @tiapi Get a list of the currently installed Kroll Mobile SDK components
		 * @tiresult[list] a list of API.Component of installed Mobile SDK components
		 */
		this->SetMethod("getInstalledMobileSDKs", &APIBinding::_GetInstalledMobileSDKs);

		/**
		 * @tiapi(method=True,name=API.getInstalledModules,since=0.4)
		 * @tiapi Get a list of the currently installed Kroll module components
		 * @tiresult[list] a list of API.Component of installed module components
		 */
		this->SetMethod("getInstalledModules", &APIBinding::_GetInstalledModules);

		/**
		 * @tiapi(method=True,name=API.getInstalledRuntimes,since=0.4)
		 * @tiapi Get a list of the currently installed Kroll runtime components
		 * @tiresult[list] a list of API.Component of installed runtime components
		 */
		this->SetMethod("getInstalledRuntimes", &APIBinding::_GetInstalledRuntimes);

		/**
		 * @tiapi(method=True,name=API.getComponentSearchPaths,since=0.4)
		 * @tiapi Get a list of the paths on which Kroll searches for installed
		 * @tiapi components. This does not include paths of bundled components.
		 * @tiresult[list] a list of string of component search paths
		 */
		this->SetMethod("getComponentSearchPaths", &APIBinding::_GetComponentSearchPaths);

		/**
		 * @tiapi(method=True,name=API.readApplicationManifest,since=0.4)
		 * @tiapi Read an application manifest at a given path
		 * @tiresult[object] an API.Application which represents the application with given manifest
		 */
		this->SetMethod("readApplicationManifest", &APIBinding::_ReadApplicationManifest);

		/**
		 * @tiapi(method=True,name=API.createDependency,since=0.4)
		 * @tiapi A constructor for dependency objects
		 * @tiarg[int, type] the type of this dependency (eg API.MODULE)
		 * @tiarg[string, name] the name of this dependency
		 * @tiarg[string, version] the version requirement for this dependency
		 * @tiarg[int, requirement, optional=true] the requirement for this dependency
		 * @tiresult[object] an API.Application which represents the application with given manifest
		 */
		this->SetMethod("createDependency", &APIBinding::_CreateDependency);

		/**
		 * @tiapi(method=True,name=API.installDependencies,since=0.4)
		 * @tiapi Invoke the installer to find and install a list of dependencies
		 * @tiarg[list, dependencies] a list of API.Dependency to find and install
		 * @tiarg[method, callback] a callback to invoke when the installer is finished (may finish unsuccessfully)
		 */
		this->SetMethod("installDependencies", &APIBinding::_InstallDependencies);

		this->SetMethod("componentGUIDToComponentType", &APIBinding::_ComponentGUIDToComponentType);

		/**
		 * @tiapi(method=True,name=API.log,since=0.2)
		 * @tiapi Log a statement with a given severity
		 * @tiarg[int, type] the severity of this log statement
		 * @tiarg[string, statement] the statement to log
		 */
		this->SetMethod("log", &APIBinding::_Log);

		/**
		 * @tiapi(method=True,name=API.trace,since=0.4)
		 * @tiapi Log a statement with TRACE severity
		 * @tiarg[string, statement] the statement to log
		 */
		this->SetMethod("trace", &APIBinding::_LogTrace);

		/**
		 * @tiapi(method=True,name=API.debug,since=0.4)
		 * @tiapi Log a statement with DEBUG severity
		 * @tiarg[string, statement] the statement to log
		 */
		this->SetMethod("debug", &APIBinding::_LogDebug);

		/**
		 * @tiapi(method=True,name=API.info,since=0.4)
		 * @tiapi Log a statement with INFO severity
		 * @tiarg[string, statement] the statement to log
		 */
		this->SetMethod("info", &APIBinding::_LogInfo);

		/**
		 * @tiapi(method=True,name=API.notice,since=0.4)
		 * @tiapi Log a statement with NOTICE severity
		 * @tiarg[string, statement] the statement to log
		 */
		this->SetMethod("notice", &APIBinding::_LogNotice);

		/**
		 * @tiapi(method=True,name=API.warn,since=0.4)
		 * @tiapi Log a statement with WARN severity
		 * @tiarg[string, statement] the statement to log
		 */
		this->SetMethod("warn", &APIBinding::_LogWarn);

		/**
		 * @tiapi(method=True,name=API.error,since=0.4)
		 * @tiapi Log a statement with ERROR severity
		 * @tiarg[string, statement] the statement to log
		 */
		this->SetMethod("error", &APIBinding::_LogError);

		/**
		 * @tiapi(method=True,name=API.critical,since=0.4)
		 * @tiapi Log a statement with CRITICAL severity
		 * @tiarg[string, statement] the statement to log
		 */
		this->SetMethod("critical", &APIBinding::_LogCritical);

		/**
		 * @tiapi(method=True,name=API.fatal,since=0.4)
		 * @tiapi Log a statement with FATAL severity
		 * @tiarg[string, statement] the statement to log
		 */
		this->SetMethod("fatal", &APIBinding::_LogFatal);

		// These are properties for log severity levels

		/**
		 * @tiapi(property=True,name=API.TRACE,since=0.4)
		 * @tiapi a constant representing TRACE severity
		 */
		this->Set("TRACE", Value::NewInt(Logger::LTRACE));

		/**
		 * @tiapi(property=True,name=API.DEBUG,since=0.4)
		 * @tiapi a constant representing DEBUG severity
		 */
		this->Set("DEBUG", Value::NewInt(Logger::LDEBUG));

		/**
		 * @tiapi(property=True,name=API.INFO,since=0.4)
		 * @tiapi a constant representing INFO severity
		 */
		this->Set("INFO", Value::NewInt(Logger::LINFO));

		/**
		 * @tiapi(property=True,name=API.NOTICE,since=0.4)
		 * @tiapi a constant representing NOTICE severity
		 */
		this->Set("NOTICE", Value::NewInt(Logger::LNOTICE));

		/**
		 * @tiapi(property=True,name=API.WARN,since=0.4)
		 * @tiapi a constant representing WARN severity
		 */
		this->Set("WARN", Value::NewInt(Logger::LWARN));

		/**
		 * @tiapi(property=True,name=API.ERROR,since=0.4)
		 * @tiapi a constant representing ERROR severity
		 */
		this->Set("ERROR", Value::NewInt(Logger::LERROR));

		/**
		 * @tiapi(property=True,name=API.CRITICAL,since=0.4)
		 * @tiapi a constant representing CRITICAL severity
		 */
		this->Set("CRITICAL", Value::NewInt(Logger::LCRITICAL));

		/**
		 * @tiapi(property=True,name=API.FATAL,since=0.4)
		 * @tiapi a constant representing FATAL severity
		 */
		this->Set("FATAL", Value::NewInt(Logger::LFATAL));

		// These are properties for dependencies

		/**
		 * @tiapi(property=True,name=API.EQ,since=0.4)
		 * @tiapi a constant representing an equality dependency
		 */
		this->Set("EQ", Value::NewInt(Dependency::EQ));

		/**
		 * @tiapi(property=True,name=API.LT,since=0.4)
		 * @tiapi a constant representing a less-than dependency
		 */
		this->Set("LT", Value::NewInt(Dependency::LT));

		/**
		 * @tiapi(property=True,name=API.LTE,since=0.4)
		 * @tiapi a constant representing a less-than-or-equal dependency
		 */
		this->Set("LTE", Value::NewInt(Dependency::LTE));

		/**
		 * @tiapi(property=True,name=API.GT,since=0.4)
		 * @tiapi a constant representing a greater-than dependency
		 */
		this->Set("GT", Value::NewInt(Dependency::GT));

		/**
		 * @tiapi(property=True,name=API.GTE,since=0.4)
		 * @tiapi a constant representing a greather-than-or-equal dependency
		 */
		this->Set("GTE", Value::NewInt(Dependency::GTE));

		// Component types

		/**
		 * @tiapi(property=True,name=API.MODULE,since=0.4)
		 * @tiapi a constant representing a module component type
		 */
		this->Set("MODULE", Value::NewInt(MODULE));

		/**
		 * @tiapi(property=True,name=API.RUNTIME,since=0.4)
		 * @tiapi a constant representing a runtime component type
		 */
		this->Set("RUNTIME", Value::NewInt(RUNTIME));

		/**
		 * @tiapi(property=True,name=API.SDK,since=0.4)
		 * @tiapi a constant representing an SDK component type
		 */
		this->Set("SDK", Value::NewInt(SDK));

		/**
		 * @tiapi(property=True,name=API.MOBILESDK,since=0.4)
		 * @tiapi a constant representing a mobile SDK component type
		 */
		this->Set("MOBILESDK", Value::NewInt(MOBILESDK));

		/**
		 * @tiapi(property=True,name=API.APP_UPDATE,since=0.4)
		 * @tiapi a constant representing an app update component type
		 */
		this->Set("APP_UPDATE", Value::NewInt(APP_UPDATE));

		/**
		 * @tiapi(property=True,name=API.UNKNOWN,since=0.4)
		 * @tiapi a constant representing an UNKNOWN component type
		 */
		this->Set("UNKNOWN", Value::NewInt(UNKNOWN));

	}

	APIBinding::~APIBinding()
	{
		ScopedLock lock(&mutex);

		registrations.clear();
		registrationsById.clear();
	}

	int APIBinding::GetNextRecord()
	{
		ScopedLock lock(&mutex);
		return ++this->record;
	}

	void APIBinding::_Set(const ValueList& args, SharedValue result)
	{
		const char *key = args.at(0)->ToString();
		string s = key;
		SharedValue value = args.at(1);
		string::size_type pos = s.find_first_of(".");

		if (pos==string::npos)
		{
			this->Set(key, value);
		}
		else
		{
			// if we have a period, make it relative to the
			// global scope such that <module>.<key> would resolve
			// to the 'module' with key named 'key'
			global->SetNS(key, value);
		}
	}

	void APIBinding::_Get(const ValueList& args, SharedValue result)
	{
		string s = args.at(0)->ToString();
		const char *key = s.c_str();
		SharedValue r = 0;
		string::size_type pos = s.find_first_of(".");

		if (pos==string::npos)
		{
			r = this->Get(key);
		}
		else
		{
			// if we have a period, make it relative to the
			// global scope such that <module>.<key> would resolve
			// to the 'module' with key named 'key'
			r = global->GetNS(key);
		}
		result->SetValue(r);
	}

	void APIBinding::_LogTrace(const ValueList& args, SharedValue result)
	{
		SharedValue arg1 = args.at(0);
		this->Log(Logger::LTRACE, arg1);
	}
	void APIBinding::_LogDebug(const ValueList& args, SharedValue result)
	{
		SharedValue arg1 = args.at(0);
		this->Log(Logger::LDEBUG, arg1);
	}
	void APIBinding::_LogInfo(const ValueList& args, SharedValue result)
	{
		SharedValue arg1 = args.at(0);
		this->Log(Logger::LINFO, arg1);
	}
	void APIBinding::_LogNotice(const ValueList& args, SharedValue result)
	{
		SharedValue arg1 = args.at(0);
		this->Log(Logger::LNOTICE, arg1);
	}
	void APIBinding::_LogWarn(const ValueList& args, SharedValue result)
	{
		SharedValue arg1 = args.at(0);
		this->Log(Logger::LWARN, arg1);
	}
	void APIBinding::_LogError(const ValueList& args, SharedValue result)
	{
		SharedValue arg1 = args.at(0);
		this->Log(Logger::LERROR, arg1);
	}
	void APIBinding::_LogCritical(const ValueList& args, SharedValue result)
	{
		SharedValue arg1 = args.at(0);
		this->Log(Logger::LCRITICAL, arg1);
	}
	void APIBinding::_LogFatal(const ValueList& args, SharedValue result)
	{
		SharedValue arg1 = args.at(0);
		this->Log(Logger::LFATAL, arg1);
	}

	void APIBinding::_Log(const ValueList& args, SharedValue result)
	{
		if (args.size() == 1)
		{
			SharedValue v = args.at(0);
			this->Log(Logger::LINFO, v);
		}
		else if (args.size() == 2)
		{
			int severity = Logger::LINFO;

			SharedValue arg1 = args.at(0);
			if (arg1->IsString())
			{
				string type = arg1->ToString();
				if (type == "TRACE")
					severity = Logger::LTRACE;

				else if (type == "DEBUG")
					severity = Logger::LDEBUG;

				else if (type == "INFO")
					severity = Logger::LINFO;

				else if (type == "NOTICE")
					severity = Logger::LNOTICE;

				else if (type == "WARN")
					severity = Logger::LWARN;

				else if (type == "ERROR")
					severity = Logger::LERROR;

				else if (type == "CRITICAL")
					severity = Logger::LCRITICAL;

				else if (type == "FATAL")
					severity = Logger::LFATAL;
			}
			else if (arg1->IsInt())
			{
				severity = arg1->ToInt();
			}
			SharedValue v = args.at(1);
			this->Log(severity, v);
		}
	}

	void APIBinding::_Register(const ValueList& args, SharedValue result)
	{
		string event = args.at(0)->ToString();
		SharedKMethod method = args.at(1)->ToMethod();

		int id = this->Register(event, method);
		result->SetInt(id);
	}

	void APIBinding::_Unregister(const ValueList& args, SharedValue result)
	{
		int id = args.at(0)->ToInt();
		this->Unregister(id);
	}

	void APIBinding::_Fire(const ValueList& args, SharedValue result)
	{
		const char* event = args.at(0)->ToString();
		this->Fire(event,args.at(1));
	}

	//---------------- IMPLEMENTATION METHODS
	void APIBinding::Log(int severity, SharedValue value)
	{
		// optimize these calls since they're called a lot
		if (false == logger->IsEnabled((Logger::Level)severity))
		{
			return;
		}
		
		if (value->IsString())
		{
			string message = value->ToString();
			logger->Log((Logger::Level) severity, message);
		}
		else
		{
			SharedString message = value->DisplayString();
			logger->Log((Logger::Level) severity, *message);
		}
	}

	int APIBinding::Register(string& event, SharedKMethod callback)
	{
		ScopedLock lock(&mutex);
		int record = GetNextRecord();
		/* Fetch the records for this event. If the EventRecords
		 * doesn't exist in registrations, the STL map
		 * implementation will insert it into the map */
		string en(event);
		EventRecords records = this->registrations[en];
		records.push_back(callback);

		BoundEventEntry e;
		e.method = callback;
		e.event = en;
		this->registrationsById[record] = e;
		this->registrations[event] = records;

		logger->Debug("Register called for event: %s\n", event.c_str());
		return record;
	}

	void APIBinding::Unregister(int id)
	{
		ScopedLock lock(&mutex);
		map<int, BoundEventEntry>::iterator i = registrationsById.find(id);
		if (i == registrationsById.end())
		{
			return;
		}

		BoundEventEntry entry = i->second;
		EventRecords records = this->registrations[entry.event];
		EventRecords::iterator fi = records.begin();
		while (fi != records.end())
		{
			SharedKMethod callback = (*fi);
			if (callback.get() == entry.method.get())
			{
				records.erase(fi);
				break;
			}
			fi++;
		}
		if (records.size()==0)
		{
			this->registrations.erase(entry.event);
		}
		registrationsById.erase(id);
	}

	void APIBinding::Fire(const char* event, SharedValue value)
	{
		// Lots of debug output really slows down things on Win32,
		// so log this at the trace level
		if (logger->IsTraceEnabled())
		{
			logger->Trace(string("FIRING: ") + event);
		}

		if (this->registrations.size() == 0)
		{
			return;
		}

		EventRecords records = this->registrations[event];
		if (records.size() > 0)
		{
			EventRecords::iterator i = records.begin();
			while (i != records.end())
			{
				SharedKMethod method = (*i++);
				method->Call(event,value);
			}
		}
	}

	void APIBinding::_GetApplication(const ValueList& args, SharedValue result)
	{
		SharedKObject app = new ApplicationBinding(host->GetApplication(), true);
		result->SetObject(app);
	}

	void APIBinding::_GetInstalledComponentsImpl(
		KComponentType type, const ValueList& args, SharedValue result)
	{
		bool force = args.GetBool(0, false);
		vector<SharedComponent>& components = BootUtils::GetInstalledComponents(force);
		SharedKList componentList = ComponentVectorToKList(components, type);
		result->SetList(componentList);
	}

	void APIBinding::_GetInstalledComponents(const ValueList& args, SharedValue result)
	{
		args.VerifyException("getInstalledComponents", "?b");
		_GetInstalledComponentsImpl(UNKNOWN, args, result);
	}

	void APIBinding::_GetInstalledModules(const ValueList& args, SharedValue result)
	{
		args.VerifyException("getInstalledModules", "?b");
		_GetInstalledComponentsImpl(MODULE, args, result);
	}

	void APIBinding::_GetInstalledRuntimes(const ValueList& args, SharedValue result)
	{
		args.VerifyException("getInstalledRuntimes", "?b");
		_GetInstalledComponentsImpl(RUNTIME, args, result);
	}

	void APIBinding::_GetInstalledSDKs(const ValueList& args, SharedValue result)
	{
		args.VerifyException("getInstalledSDKs", "?b");
		_GetInstalledComponentsImpl(SDK, args, result);
	}

	void APIBinding::_GetInstalledMobileSDKs(const ValueList& args, SharedValue result)
	{
		args.VerifyException("getInstalledMobileSDKs", "?b");
		_GetInstalledComponentsImpl(MOBILESDK, args, result);
	}

	void APIBinding::_GetComponentSearchPaths(const ValueList& args, SharedValue result)
	{
		vector<string>& paths = BootUtils::GetComponentSearchPaths();
		SharedKList pathList = StaticBoundList::FromStringVector(paths);
		result->SetList(pathList);
	}

	void APIBinding::_ReadApplicationManifest(const ValueList& args, SharedValue result)
	{
		args.VerifyException("readApplicationManifest", "s,?s");
		string manifestPath = args.at(0)->ToString();
		string appPath = args.GetString(1, FileUtils::Dirname(manifestPath));

		SharedApplication app = Application::NewApplication(manifestPath, appPath);
		if (!app.isNull())
		{
			result->SetObject(new ApplicationBinding(app));
		}
		else
		{
			result->SetNull();
		}
	}

	void APIBinding::_CreateDependency(const ValueList& args, SharedValue result)
	{
		args.VerifyException("readApplicationManifest", "i,s,s,?i");
		int type = args.GetInt(0, UNKNOWN);
		string name = args.GetString(1);
		string version = args.GetString(2);
		int requirement = (int) args.GetNumber(3, Dependency::EQ);

		if (type != MODULE && type != RUNTIME
			&& type != SDK && type != MOBILESDK
			&& type != APP_UPDATE)
		{
			throw ValueException::FromString(
				"Tried to create a dependency with an unknown dependency type");
		}
		else if (requirement != Dependency::EQ
			&& requirement != Dependency::GT
			&& requirement != Dependency::LT
			&& requirement != Dependency::GTE
			&& requirement != Dependency::LTE)
		{
			throw ValueException::FromString(
				"Tried to create a dependency with an unknown requirement type");
		}
		else
		{
			SharedDependency d = Dependency::NewDependencyFromValues(
				static_cast<KComponentType>(type), name, version);
			SharedKObject dBinding = new DependencyBinding(d);
			result->SetObject(dBinding);
		}
	}

	void APIBinding::_InstallDependencies(const ValueList& args, SharedValue result)
	{
		args.VerifyException("installDependencies", "l,m");
		SharedKList dependenciesList = args.GetList(0);
		SharedKMethod callback = args.GetMethod(1, 0);
		vector<SharedDependency> dependencies;

		for (unsigned int i = 0; i < dependenciesList->Size(); i++)
		{
			if (!dependenciesList->At(i)->IsObject())
			{
				continue;
			}

			SharedPtr<DependencyBinding> d =
				dependenciesList->At(i)->ToObject().cast<DependencyBinding>();
			if (!d.isNull())
			{
				dependencies.push_back(d->GetDependency());
			}
		}

		if (dependencies.size() > 0)
		{
			if (!this->installerMutex.tryLock())
			{
				throw ValueException::FromString(
					"Tried to launch more than one instance of the installer");
			}

			if (this->installerThread)
			{
				delete this->installerThread;
			}
			this->installerDependencies = dependencies;
			this->installerCallback = callback;
			this->installerThread = new Poco::Thread();
			this->installerThread->start(*this->installerThreadAdapter);
		}
	}
	
	void APIBinding::_ComponentGUIDToComponentType(const ValueList& args, SharedValue result)
	{
		std::string type = args.at(0)->ToString();
		if (type == RUNTIME_UUID)
		{
			result->SetInt(RUNTIME);
		}
		else if (type == MODULE_UUID)
		{
			result->SetInt(MODULE);
		}
		else if (type == SDK_UUID)
		{
			result->SetInt(SDK);
		}
		else if (type == MOBILESDK_UUID)
		{
			result->SetInt(MOBILESDK);
		}
		else if (type == APP_UPDATE_UUID)
		{
			result->SetInt(APP_UPDATE);
		}
		else
		{
			result->SetInt(UNKNOWN);
		}
	}

	void APIBinding::RunInstaller()
	{
		SharedApplication app = host->GetApplication();
		BootUtils::RunInstaller(
			this->installerDependencies, app, "", app->runtime->path, true);

		if (!this->installerCallback.isNull())
		{
			host->InvokeMethodOnMainThread(
				this->installerCallback, ValueList(), false);
		}
		this->installerMutex.unlock();
	}

	SharedKList APIBinding::ComponentVectorToKList(
		vector<SharedComponent>& components,
		KComponentType filter)
	{
		SharedKList componentList = new StaticBoundList();
		vector<SharedComponent>::iterator i = components.begin();
		while (i != components.end())
		{
			SharedComponent c = *i++;
			if (filter == UNKNOWN || filter == c->type)
			{
				SharedValue cValue = Value::NewObject(new ComponentBinding(c));
				componentList->Append(cValue);
			}
		}

		return componentList;
	}

	SharedKList APIBinding::DependencyVectorToKList(std::vector<SharedDependency>& deps)
	{
		SharedKList dependencyList = new StaticBoundList();
		std::vector<SharedDependency>::iterator i = deps.begin();
		while (i != deps.end())
		{
			SharedValue dValue = Value::NewObject(new DependencyBinding(*i++));
			dependencyList->Append(dValue);
		}
		return dependencyList;
	}

	SharedKList APIBinding::ManifestToKList(vector<pair<string, string> >& manifest)
	{
		SharedKList list = new StaticBoundList();
		vector<pair<string, string> >::iterator i = manifest.begin();
		while (i != manifest.end())
		{
			SharedKList entry = new StaticBoundList();
			entry->Append(Value::NewString(i->first));
			entry->Append(Value::NewString(i->second));
			list->Append(Value::NewList(entry));
			*i++;
		}
		return list;
	}

}
