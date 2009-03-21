/*
 * Appcelerator Kroll - licensed under the Apache Public License 2
 * see LICENSE in the root folder for details on the license.
 * Copyright (c) 2008 Appcelerator, Inc. All Rights Reserved.
 */

#ifndef _KR_SCOPE_METHOD_DELEGATE_H_
#define _KR_BOUND_METHOD_DELEGATE_H_

namespace kroll {

	enum MethodDelegateType
	{
		GET, /**< Getter method */
		SET /**< Setter method */
	};

	/**
	 * class that can be used to change the delegation of a method
	 * call's Get or Set method to first check to see if the key has
	 * namespace dots (such as ti.foo.bar) and if so, delegate to a
	 * differently supplied scope object for delegation.
	 */
	class KROLL_API ScopeMethodDelegate : public KMethod
	{
	public:

		ScopeMethodDelegate(MethodDelegateType type, SharedKObject global,
		                    SharedKObject scope, SharedKMethod delegate);
		virtual ~ScopeMethodDelegate();


		void Set(const char *name, SharedValue value);
		SharedValue Get(const char *name);
		SharedStringList GetPropertyNames();


		bool IsGlobalKey(std::string& key);
		SharedValue Call(const ValueList& args);

		/**
		 * create a delegate from a KObject to a wrapped
		 * StaticBoundObject and delegate set/get to the new
		 * static bound object
		 */
		static SharedPtr<StaticBoundObject> CreateDelegate(SharedKObject global, SharedKObject bo);

	private:
		MethodDelegateType type;
		SharedKObject global;
		SharedKObject scope;
		SharedKMethod delegate;


	private:
		DISALLOW_EVIL_CONSTRUCTORS(ScopeMethodDelegate);
	};

}

#endif
