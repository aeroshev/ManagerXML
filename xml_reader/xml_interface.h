// Created by Ерошев Артём

#pragma once
#include "xml_reader.h"


namespace xml_rd
{
	class Interface {
	public:
		explicit Interface(std::shared_ptr<ManagerXML>);
		~Interface();

		void start();
	private:
		std::shared_ptr<ManagerXML> manager;
		std::string current_department;
	};
}
