// Created by Ерошев Артём

#pragma once
#include "xml_components.h"
#include <iostream>

namespace xml_rd
{
	struct CombineBlock {
		static std::unique_ptr<XMLBlock> create_employ(type_operation, const std::string&);
		static std::unique_ptr<XMLBlock> create_dep(type_operation, const std::string&, const std::string&);
	private:
		static void filler(const std::unique_ptr<XMLEmploy>&, const std::string&);
	};
}

