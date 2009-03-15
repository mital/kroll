/*
 * Appcelerator Kroll - licensed under the Apache Public License 2
 * see LICENSE in the root folder for details on the license.
 * Copyright (c) 2008 Appcelerator, Inc. All Rights Reserved.
 */

#include "../kroll.h"
#include <sstream>
#include <cmath>

namespace kroll
{

	void SetAt(unsigned int index, SharedValue value)
	{
		while (index >= this->Size())
		{
			// now we need to create entries between current
			// size and new size  and make the entries undefined.
			this->Append(Value::Undefined);
		}

	}

	SharedString KList::DisplayString(int levels)
	{
		std::ostringstream oss;
		if (levels == 0)
		{
			oss << "<KList at " << this << ">";
		}
		else
		{

			oss << "[";
			for (unsigned int i = 0; i < this->Size(); i++)
			{
				SharedValue list_val = this->At(i);
				SharedString list_str = list_val->DisplayString(levels-1);
				oss << " " << *list_str << ",";
			}
			//int before_last_comma = oss.tellp() - 1;
			//oss.seekp(before_last_comma);
			oss << " ]";
		}
		return new std::string(oss.str());
	}

	std::string KList::IntToChars(unsigned int value)
	{
		std::stringstream ss;
		ss << value;
		return ss.str();
	}

	bool KList::IsInt(const char* name)
	{
		for (size_t i = 0; i < strlen(name); i++)
		{
			if (!isdigit(name[i]))
				return false;
		}
		return true;
	}



}

