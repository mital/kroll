/**
 * Appcelerator Kroll - licensed under the Apache Public License 2
 * see LICENSE in the root folder for details on the license.
 * Copyright (c) 2008 Appcelerator, Inc. All Rights Reserved.
 */

#include "binding.h"

using namespace kroll;

ScopeMethodDelegate::ScopeMethodDelegate(MethodDelegateType type,
                                         SharedBoundObject global,
                                         SharedBoundObject scope,
                                         SharedBoundMethod delegate) :
	type(type), global(global), scope(scope), delegate(delegate)
{
	//KR_ADDREF(global);
	//KR_ADDREF(scope);
	//KR_ADDREF(delegate);
}

ScopeMethodDelegate::~ScopeMethodDelegate()
{
	//KR_DECREF(global);
	//KR_DECREF(scope);
	//KR_DECREF(delegate);
}


void ScopeMethodDelegate::Set(const char *name, SharedValue value)
{
	delegate->Set(name,value);
}

SharedValue ScopeMethodDelegate::Get(const char *name)
{
	return delegate->Get(name);
}

SharedStringList ScopeMethodDelegate::GetPropertyNames()
{
	return delegate->GetPropertyNames();
}

bool ScopeMethodDelegate::IsGlobalKey(std::string& key)
{
	std::string::size_type pos = key.find_first_of(".");
	return (pos!=std::string::npos);
}

SharedValue ScopeMethodDelegate::Call(const ValueList& args)
{
	std::string key = args.at(0)->ToString();
	SharedBoundObject obj = IsGlobalKey(key) ? global : scope;
	if (type == GET)
	{
		// not found, look inside scope
		return obj->Get(key.c_str());
	}
	else
	{
		obj->SetNS(key.c_str(),args.at(1));
		return Value::Undefined;
	}
}

SharedPtr<StaticBoundObject> ScopeMethodDelegate::CreateDelegate(SharedBoundObject global, SharedBoundObject bo)
{
	SharedPtr<StaticBoundObject> scope = new StaticBoundObject();
	SharedStringList keys = bo->GetPropertyNames();
	StringList::iterator iter = keys->begin();

	while(iter!=keys->end())
	{
		const char *name = (*iter++);
		std::string key(name);
		SharedValue value = bo->Get(name);

		if (key == "set")
		{
			SharedBoundMethod d = new ScopeMethodDelegate(SET, global, scope,value->ToMethod());
			SharedValue v = new Value(d);
			//ScopedDereferencer d1(d);
			//ScopedDereferencer d2(v);
			scope->Set(name, v);
		}
		else if (key == "get")
		{
			SharedBoundMethod d = new ScopeMethodDelegate(GET, global, scope,value->ToMethod());
			SharedValue v = new Value(d);
			//ScopedDereferencer d1(d);
			//ScopedDereferencer d2(v);
			scope->Set(name, v);
		}
		else
		{
			scope->Set(name, value);
		}

	}
	return scope;
}

