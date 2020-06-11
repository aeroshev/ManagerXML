// Created by Ерошев Артём

#pragma once
#include <string>
#include <unordered_set>


namespace xml_rd
{
	enum type_operation
	{
		write,
		erase,
		change
	};

	struct XMLEmploy
	{
		std::string surname;
		std::string name;
		std::string middleName;
		std::string function;
		unsigned int salary;

		bool operator==(const XMLEmploy& compare) const noexcept
		{
			return (surname == compare.surname &&
					name == compare.name &&
					middleName == compare.middleName &&
					function == compare.function &&
					salary == compare.salary);
		}

		size_t operator()(const XMLEmploy& hashPoint) const noexcept
		{
			size_t hash = std::hash<std::string>{}(surname);
			hash += std::hash<std::string>{}(name);
			hash += std::hash<std::string>{}(middleName);
			return hash;
		}
	};
}

namespace std
{
	template<>
	struct hash<xml_rd::XMLEmploy>
	{
		size_t operator()(const xml_rd::XMLEmploy& h) const noexcept
		{
			return h(h);
		}
	};
}

namespace xml_rd
{
	struct XMLBlock {
		type_operation type;
		std::shared_ptr<XMLEmploy> newEmploy;
		std::shared_ptr<XMLEmploy> oldEmploy;
		std::string newNameDepart;
		std::string oldNameDepart;
		std::unordered_set<XMLEmploy> old_data;
	};
}

