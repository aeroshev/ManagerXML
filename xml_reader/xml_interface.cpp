// Created by Ерошев Артём

#include "xml_interface.h"


namespace xml_rd
{
	Interface::Interface(std::shared_ptr<ManagerXML> manager):
			manager(std::move(manager))
	{}

	Interface::~Interface()
	{
		manager.reset();
	}

	void Interface::start()
	{
		unsigned int depth = 0, space = 0;
		std::string command, op;
		while(true)
		{
			std::cin.clear();
			std::cout << "/" << current_department << '\n';
			getline(std::cin, command);

			space = command.find_first_of(' ');
			op = command.substr(0, space);

			if (op == "quit")
			{
				break;
			}
			else if (op == "show")
			{
				manager->show_tree();
				continue;
			}
			else if (op == "step_back")
			{
				manager->step_back();
				continue;
			}
			else if (op == "rollback")
			{
				manager->rollback();
				continue;
			}
			else if (op == "add")
			{
				switch (depth)
				{
					case 0:
					{
						unsigned int pos_f = command.find_first_of('\"') + 1;
						std::string dep_name = command.substr(pos_f);
						unsigned  int pos_l = dep_name.find_first_of('\"');
						dep_name = dep_name.substr(0, pos_l);

						manager->put(std::move(CombineBlock::create_dep(type_operation::write, dep_name, "")));
						break;
					}
					case 1:
						manager->put(std::move(CombineBlock::create_employ(type_operation::write, current_department)));
						break;
					default:
						std::cerr << "Depth incorrect" << '\n';
				}
				continue;
			}
			else if (op == "save")
			{
				manager->save();
				continue;
			}
			else if (op == "cd")
			{
				std::string up = command.substr(space);
				unsigned int dot = up.find_first_not_of(' ');
				up = up.substr(dot, 2);

				unsigned int start_dep = command.find_first_of('\"') + 1;
				unsigned int len = command.find_last_of('\"') - start_dep;
				std::string dep = command.substr(start_dep, len);

				if (up == ".." && start_dep == 0)
				{
					if (depth == 0)
						std::cerr << "We're the top tree" << '\n';
					else
					{
						depth--;
						current_department = "";
					}
				}
				else
				{
					if (depth == 1)
						std::cerr << "We're the bottom tree" << '\n';
					else
					{
						if (manager->exist_department(dep))
						{
							depth++;
							current_department = dep;
						}
						else
							std::cerr << "This department does't exist_department" << '\n';
					}
				}
			}
			else if (op == "rm")
			{
				switch (depth)
				{
					case 0:
					{
						unsigned int start_dep = command.find_first_of('\"') + 1;
						unsigned int len = command.find_last_of('\"') - start_dep;
						std::string dep = command.substr(start_dep, len);

						manager->put(std::move(CombineBlock::create_dep(type_operation::erase, "", dep)));
						break;
					}
					case 1:
						manager->put(std::move(CombineBlock::create_employ(type_operation::erase, current_department)));
						break;
					default:
						std::cerr << "Depth incorrect" << '\n';
				}
			}
			else if (op == "ch")
			{
				switch (depth)
				{
					case 0:
					{
						// Extract first name
						// First symbol after "
						unsigned int pos_f_c = command.find_first_of('\"') + 1;
						std::string cur_name = command.substr(pos_f_c);
						// Last symbol before "
						unsigned int pos_l_c = cur_name.find_first_of('\"');
						cur_name = cur_name.substr(0, pos_l_c);

						// Extract new name
						// Last symbol before "
						unsigned int pos_l_n = command.find_last_of('\"');
						std::string new_name = command.substr(0, pos_l_n);
						// First symbol after "
						unsigned int pos_f_n = new_name.find_last_of('\"') + 1;
						new_name = new_name.substr(pos_f_n);

						manager->put(std::move(CombineBlock::create_dep(type_operation::change, new_name, cur_name)));
						break;
					}
					case 1:
						manager->put(std::move(CombineBlock::create_employ(type_operation::change, current_department)));
						break;
					default:
						std::cerr << "Depth incorrect" << '\n';
				}
			}
			else if (op == "man")
			{
				std::cout << "quit - quit out this program" << '\n';
				std::cout << "show - show content in tree loaded file" << '\n';
				std::cout << "step_back - discard last command" << '\n';
				std::cout << "rollback - discard all changes" << '\n';
				std::cout << "add - add new departments or employ. If you stay in top '/' command will add department" << '\n';
				std::cout << "save - accept all changes and write in file" << '\n';
				std::cout << "cd - move to department directories" << '\n';
				std::cout << "rm - delete departement or employ. Choice equal like in add" << '\n';
				std::cout << "ch - change contain in departments or employ. For departments 'ch \"old_name\" \"new_name\"'" << '\n';
			}
			else
				std::cerr << "Can't recognise command" <<'\n';
		}
	}
}