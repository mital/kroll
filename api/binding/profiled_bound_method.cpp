/*
 * Appcelerator Kroll - licensed under the Apache Public License 2
 * see LICENSE in the root folder for details on the license.
 * Copyright (c) 2009 Appcelerator, Inc. All Rights Reserved.
 */

#include "../kroll.h"
#include <cstdio>
#include <cstring>
#include <Poco/Stopwatch.h>

namespace kroll
{
	ProfiledBoundMethod::ProfiledBoundMethod(SharedKMethod delegate, std::string& type) :
		ProfiledBoundObject(delegate),
		method(delegate),
		fullType(type)
	{
	}

	ProfiledBoundMethod::~ProfiledBoundMethod()
	{
	}

	SharedValue ProfiledBoundMethod::Call(const ValueList& args)
	{
		std::string type = this->GetType();

		SharedValue value;
		Poco::Stopwatch sw;
		sw.start();
		try {
			value = method->Call(args);
		} catch (...) {
			sw.stop();
			this->Log("call", type, sw.elapsed());
			throw;
		}

		sw.stop();
		this->Log("call", type, sw.elapsed());
		return this->Wrap(value, type);
	}

	void ProfiledBoundMethod::Set(const char *name, SharedValue value)
	{
		method->Set(name,value);
	}

	SharedValue ProfiledBoundMethod::Get(const char *name)
	{
		return method->Get(name);
	}

	SharedStringList ProfiledBoundMethod::GetPropertyNames()
	{
		return method->GetPropertyNames();
	}

	std::string& ProfiledBoundMethod::GetType()
	{
		return fullType;
	}
}
