// Created by Ерошев Артём

#include "xml_combiner.h"

namespace xml_rd
{
	std::unique_ptr<XMLBlock> CombineBlock::create_employ(type_operation op, const std::string& dep_name)
	{
		auto handle = std::make_unique<XMLBlock>();
		handle->type = op;
		handle->oldNameDepart = dep_name;
		switch (op)
		{
		case type_operation::write:
			handle->newEmploy = std::make_unique<XMLEmploy>();
			filler(handle->newEmploy, "Input data");
			break;
		case  type_operation::erase:
			handle->oldEmploy = std::make_unique<XMLEmploy>();
			filler(handle->oldEmploy, "Input data");
			break;
		case type_operation::change:
			handle->newEmploy = std::make_unique<XMLEmploy>();
			handle->oldEmploy = std::make_unique<XMLEmploy>();
			filler(handle->oldEmploy, "Old data");
			filler(handle->newEmploy, "New data");
			break;
		default:
			std::cerr << "Can't recognize type command" << '\n';
		}
		return handle;
	}

	std::unique_ptr<XMLBlock> CombineBlock::create_dep(type_operation op, const std::string& new_dep, const std::string& old_dep)
	{
		auto handle = std::make_unique<XMLBlock>();
		handle->type = op;
		handle->oldNameDepart = old_dep;
		handle->newNameDepart = new_dep;

		return handle;
	}

	void CombineBlock::filler(const std::unique_ptr<XMLEmploy>& fill_it, const std::string & title)
	{
		std::string salary;
		bool error = false;

		std::cout << title << '\n';
		std::cout << "Employ surname -> ";
		getline(std::cin, fill_it->surname);
		std::cout << "Employ name -> ";
		getline(std::cin, fill_it->name);
		std::cout << "Employ middleName -> ";
		getline(std::cin, fill_it->middleName);
		std::cout << "Employ function -> ";
		getline(std::cin, fill_it->function);

		std::cout << "Employ salary -> ";
		do {
			getline(std::cin, salary);

			try
			{
				fill_it->salary = std::abs(std::stoi(salary));
				error = false;
			}
			catch (std::invalid_argument& e)
			{
				std::cerr << "Enter only digit" << '\n';
				error = true;
			}
			catch (std::out_of_range& e)
			{
				std::cerr << "Too much value" << '\n';
				error = true;
			}
		} while (error);
	}
}
