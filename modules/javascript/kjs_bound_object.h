/**
 * Appcelerator Titanium - licensed under the Apache Public License 2
 * see LICENSE in the root folder for details on the license.
 * Copyright (c) 2008 Appcelerator, Inc. All Rights Reserved.
 */

#ifndef _KJS_BOUND_OBJECT_H_
#define _KJS_BOUND_OBJECT_H_

#include "kjs.h"

#include <vector>
#include <string>
#include <map>

namespace kroll
{
	class KJSBoundObject : public kroll::BoundObject
	{
	public:
		KJSBoundObject(JSContextRef context, JSObjectRef js_object);
		~KJSBoundObject();

		void Set(const char *name, kroll::Value* value);
		kroll::Value* Get(const char *name);
		void GetPropertyNames(std::vector<const char *> *property_names);

		JSObjectRef GetJSObject();

	protected:
		JSContextRef context;
		JSObjectRef object;

	};
}

#endif
