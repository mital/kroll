/**
 * Appcelerator Kroll - licensed under the Apache Public License 2
 * see LICENSE in the root folder for details on the license.
 * Copyright (c) 2008 Appcelerator, Inc. All Rights Reserved.
 */

#ifndef _KR_VALUE_H_
#define _KR_VALUE_H_

namespace kroll
{
	/*
		Class: Value
	  Value is a container object which internally contains
	  a value which can be boxed/unboxed based on the type.
	 */
	class KROLL_API Value
	{
	public:

		/*
			Enum: Type

			INT - Integer
			DOUBLE - Double
			BOOL - Boolean
			STRING - String
			LIST - <ValueList>
			OBJECT - <BoundObject>
			METHOD - <BoundMethod>
			NULLV - Null
			UNDEFINED - Undefined
		 */
		enum Type {
			INT = 1,
			DOUBLE = 2,
			BOOL = 3,
			STRING = 4,
			LIST = 5,
			OBJECT = 6,
			METHOD = 7,
			NULLV = 0,
			UNDEFINED = -1
		};

		/*
			Function: Undefined

		  return a system-defined shared UNDEFINED type.
		 */
		static SharedValue Undefined;

		/*
			Function: Null

		  return a system-defined shared NULL type.
		 */
		static SharedValue Null;

		/*
			Constructor: Value

		  construct an <UNDEFINED> type
		 */
		Value();

		/*
			Constructor: Value

		  construct an <INT> type
		 */
		static SharedValue NewInt(int value);

		/**
			Constructor: Value

		  construct a <DOUBLE> type
		 */
		static SharedValue NewDouble(double value);

		/*
			Constructor: Value

		  construct a <BOOL> type
		 */
		static SharedValue NewBool(bool value);

		/*
			Constructor: Value

		  construct a <STRING> type
		 */
		static SharedValue NewString(const char* value);

		/*
			Constructor: Value

		  construct an <STRING> type
		 */
		static SharedValue NewString(std::string& value);

		/*
			Constructor: Value

		  construct a <LIST> type
		 */
		static SharedValue NewList(SharedBoundList value);

		/*
			Constructor: Value

		  construct an <OBJECT> type
		 */
		static SharedValue NewObject(SharedBoundObject value);

		static SharedValue NewObject(SharedPtr<StaticBoundObject> value);

		/*
			Constructor: Value

		  construct a <METHOD> type
		 */
		static SharedValue NewMethod(SharedBoundMethod value);

		/*
			Constructor: Value
		 */
		Value(SharedValue value);
		/*
			Constructor: Value

		  construct a copy constructor for an existing Value
		 */
		Value(const Value& value);

		/**
		 * destructor
		 */
		virtual ~Value();

	protected:
		static SharedValue CreateUndefined();
		static SharedValue CreateNull();

	public:

		/*
			Function: operator==
		*/
		bool operator== (Value);

		/*
			Function: IsInt

		  return true if the internal value is an INT
		 */
		bool IsInt() const;

		/*
			Function: IsDouble

		  return true if the internal value is a DOUBLE
		 */
		bool IsDouble() const;

		/*
			Function: IsNumber

		  return true if the internal value is an INT or a DOUBLE
		 */
		bool IsNumber() const;

		/*
			Function: IsBool

		  return true if the internal value is a BOOL
		 */
		bool IsBool() const;

		/*
			Function: IsString

		  return true if the internal value is a STRING
		 */
		bool IsString() const;

		/*
			Function: IsList

		  return true if the internal value is a LIST
		 */
		bool IsList() const;

		/*
			Function: IsObject

		  return true if the internal value is a OBJECT
		 */
		bool IsObject() const;

		/*
			Function: IsMethod

		  return true if the internal value is a METHOD
		 */
		bool IsMethod() const;

		/*
			Function: IsNull

		  return true if the internal value is a NULL
		 */
		bool IsNull() const;

		/*
			Function: IsUndefined

		  return true if the internal value is an UNDEFINED
		 */
		bool IsUndefined() const;

		/*
			Function: ToInt

		  return the value as an int
		 */
		int ToInt() const;

		/*
			Function: ToDouble

		  return the value as a double
		 */
		double ToDouble() const;

		/*
			Function: ToNumber

		  Return the value of a value as a double
		  whether it is an int or a double.
		 */
		double ToNumber() const;


		/*
			Function: ToBool

		  return the value as a bool
		 */
		bool ToBool() const;

		/*
			Function: ToString

		  return the value as a const char*
		 */
		const char* ToString() const;

		/*
			Function: ToList

		  return the value as a BoundList
		 */
		SharedBoundList ToList() const;

		/*
			Function: ToObject

		  return the value as a BoundObject*
		 */
		SharedBoundObject ToObject() const;

		/*
			Function: ToMethod

		  return the value as a BoundMethod*
		 */
		SharedBoundMethod ToMethod() const;

		/*
			Function: ToTypeString

		  return the type name of the internal value
		 */
		const char* ToTypeString();

		/*
			Function: StringRepr

		  return a string representation of this value.
		*/
		SharedString DisplayString(int levels=3);

		/*
			Function: Set

		  changes the internal value of this instance.
		  copy the internal value of other into this object.
		  this instance will make a reference of the internal
		  value of other but not other itself
		 */
		void SetValue(Value *other);

		/*
			Function: Set

		  changes the internal value of this instance.
		  copy the internal value of other into this object.
		  this instance will make a reference of the internal
		  value of other but not other itself
		 */
		void SetValue(SharedValue other);

		/*
			Function: Set

		  change the internal value of this instance to value
		 */
		void SetInt(int value);

		/*
			Function: Set

		  change the internal value of this instance to value
		 */
		void SetDouble(double value);

		/*
			Function: Set

		  change the internal value of this instance to value
		 */
		void SetBool(bool value);

		/*
			Function: Set

		  change the internal value of this instance to value
		 */
		void SetString(const char* value);

		/*
			Function: Set

		  change the internal value of this instance to value
		 */
		void SetString(std::string& value) {
			SetString(value.c_str());
		}
		/*
			Function: Set

		  change the internal value of this instance to value
		 */
		void SetList(SharedBoundList value);

		/*
			Function: Set

			change the internal value of this instance to value
		 */
		void SetStaticBoundObject(SharedPtr<StaticBoundObject> value);

		/*
			Function: Set

		  change the internal value of this instance to value
		 */
		void SetObject(SharedBoundObject value);

		/**
			Function: Set

		  change the internal value of this instance to value
		 */
		void SetMethod(SharedBoundMethod value);

		/*
			Function: SetNull

		  change the internal value of this instance to NULL
		 */
		void SetNull();

		/*
			Function: SetUndefined

		  change the internal value of this instance to UNDEFINED
		 */
		void SetUndefined();

	private:
		Type type;
		// had to get rid of union: SharedPtr can't be stored in a union
		double numberValue;
		bool boolValue;
		const char *stringValue;
		SharedBoundObject objectValue;

		void defaults();
		void init();

	};

	class KROLL_API ValueReleasePolicy : public Poco::ReleasePolicy<Value> {
	public:
		static void release(Value* pObj)
		{
			delete pObj;
			pObj = NULL;
		}
	};

}

#endif

