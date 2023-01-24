#pragma once
#include <string>

namespace Hogra {
	class Identifiable
	{
	public:
		Identifiable();

		void SetId(unsigned int _id);

		unsigned int GetId() const;

		void SetName(const std::string& _name);

		std::string GetName() const;

	protected:
		uint32_t id;
		static uint32_t nextId;	// For generating new ids.
		std::string name = "";
	};
}

