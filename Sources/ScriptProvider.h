#pragma once

#include <map>
#include <functional>

namespace Hogra {

	class AbstractScript {

	};

	std::map<int, AbstractScript*> scripts;	// <id, script*>
		
	template <int ID, typename T>
	class Script : public AbstractScript, public std::function<T>
	{
	public:

		Script() : std::function<T>() {
			
		}

	};

}

