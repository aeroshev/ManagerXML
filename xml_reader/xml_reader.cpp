//
// Created by Артём on 06.06.2020.
//
#include "xml_reader.h"


namespace xml_rd
{
//--------------------------------------------------CombineBlock--------------------------------------------------------
    std::unique_ptr<XMLBlock> CombineBlock::create_employ(type_operation op, const std::string& dep_name)
    {
        auto handle = std::make_unique<XMLBlock>();
        handle->type = op;
        handle->oldNameDepart = dep_name;
        switch (op)
        {
        	case type_operation::write:
        		handle->newEmploy = std::make_shared<XMLEmploy>();
        		filler(handle->newEmploy, "Input data");
				break;
			case  type_operation::erase:
				handle->oldEmploy = std::make_shared<XMLEmploy>();
				filler(handle->oldEmploy, "Input data");
				break;
        	case type_operation::change:
				handle->newEmploy = std::make_shared<XMLEmploy>();
				handle->oldEmploy = std::make_shared<XMLEmploy>();
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

    void CombineBlock::filler(const std::shared_ptr<XMLEmploy>& fill_it, const std::string & title)
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
//-----------------------------------------------ManagerXML-------------------------------------------------------------
    ManagerXML::ManagerXML(size_t size_cache):
    size_cache(size_cache),
    cache_(1024),
    pointer_last_record(0),
    is_open(false)
    {}

    void ManagerXML::load_file(const std::string &path = "")
    {
        this->path = path;
        if (!xml_doc.load_file(this->path.c_str()))
            throw std::invalid_argument("Can't read this file");

        pugi::xml_node departments = xml_doc.child("departments");
        for (auto department = departments.first_child(); department; department = department.next_sibling())
        {
            auto dep_name = department.attribute("name").value();
            pugi::xml_node employments = department.child("employments");

            for (auto employment = employments.first_child(); employment; employment = employment.next_sibling())
            {
                XMLEmploy block;
                block.surname = employment.child("surname").child_value();
                block.name = employment.child("name").child_value();
                block.middleName = employment.child("middleName").child_value();
                block.function = employment.child("function").child_value();
                block.salary = std::stoi(employment.child("salary").child_value());
                tree[dep_name].insert(block);
            }
        }
        is_open = true;
    }

    void ManagerXML::show_tree(std::ostream &out) const
    {
    	if (!is_open)
		{
    		std::cerr << "File don't open" << '\n';
			return;
		}

        double avg_sal = 0;
        for (const auto& dep : tree)
        {
            avg_sal = 0;
            out << "Departament name -> " << dep.first << '\n';
            std::cout << "Count of employee in department -> " << dep.second.size() << '\n';
            for (const auto& empl : dep.second)
            {
                out << '\t' << "Surname -> " << empl.surname << '\n';
                out << '\t' << "Name -> " << empl.name << '\n';
                out << '\t' << "Middle name -> " << empl.middleName << '\n';
                out << '\t' << "Function -> " << empl.function << '\n';
                out << '\t' << "Salary -> " << empl.salary << '\n';
                out << '\n';
                avg_sal += empl.salary;
            }
            avg_sal /= (dep.second.size()) ? dep.second.size() : 1;
            std::cout << "Average salary in department -> " << std::round(avg_sal * 100) / 100 << '\n';
            std::cout << '\n';
        }
    }

    void ManagerXML::put(std::unique_ptr<XMLBlock> box)
    {
		if (!is_open)
		{
			std::cerr << "File don't open" << '\n';
			return;
		}

        if (pointer_last_record < size_cache)
        {
            switch (box->type) {
                case type_operation::write:
                    if (box->newEmploy != nullptr)
                    {
                        tree[box->oldNameDepart].insert(*box->newEmploy);
                    }
                    else
					{
                        tree.insert(std::make_pair(box->newNameDepart, std::unordered_set<XMLEmploy>()));
                    }
                    break;
                case type_operation::erase:
                    if (box->oldEmploy != nullptr)
                    {
                        tree[box->oldNameDepart].erase(*box->oldEmploy);
                    }
                    else
					{
                    	box->old_data = tree[box->oldNameDepart];
                        tree.erase(box->oldNameDepart);
                    }
                    break;
                case type_operation::change:
                    if (box->oldEmploy != nullptr)
                    {
                        auto emplHandler = tree[box->oldNameDepart].extract(*box->oldEmploy);
                        emplHandler.value() = *box->newEmploy;
                        tree[box->oldNameDepart].insert(std::move(emplHandler));
                    }
                    else
					{
                        auto nodeHandler = tree.extract(box->oldNameDepart);
                        nodeHandler.key() = box->newNameDepart;
                        tree.insert(std::move(nodeHandler));
                    }
                    break;
                    default:
						std::cerr << "Can't recognize type command" << '\n';
            }
			cache_[pointer_last_record] = *box;
			pointer_last_record++;
        }
        else
		{
			std::cerr <<"Cache is full. Please accept or discard all changes" << '\n';
		}
    }

    void ManagerXML::save()
    {
		if (!is_open)
		{
			std::cerr << "File don't open" << '\n';
			return;
		}

        cache_.clear();
        pointer_last_record = 0;
        xml_doc.remove_child("departments");
        xml_doc.append_child("departments");
        pugi::xml_node departments = xml_doc.child("departments");

        for (const auto& dep : tree) {
            auto department = departments.append_child("department");
            department.append_attribute("name") = dep.first.c_str();
            auto employments = department.append_child("employments");
            for (const auto& node : dep.second) {
                auto employment = employments.append_child("employment");
                employment.append_child("surname").append_child(pugi::node_pcdata).set_value(node.surname.c_str());
                employment.append_child("name").append_child(pugi::node_pcdata).set_value(node.name.c_str());
                employment.append_child("middleName").append_child(pugi::node_pcdata).set_value(node.middleName.c_str());
                employment.append_child("function").append_child(pugi::node_pcdata).set_value(node.function.c_str());
                employment.append_child("salary").append_child(pugi::node_pcdata).set_value(std::to_string(node.salary).c_str());
            }
        }
        xml_doc.save_file(this->path.c_str(), "\t", pugi::format_default, pugi::encoding_utf8);
        xml_doc.print(std::cout);
    }

    void ManagerXML::rollback()
    {
		if (!is_open)
		{
			std::cerr << "File don't open" << '\n';
			return;
		}

        while (pointer_last_record > 1) {
            step_back();
        }
    }

    void ManagerXML::step_back()
    {
		if (!is_open)
		{
			std::cerr << "File don't open" << '\n';
			return;
		}

        auto tr_unit = cache_[pointer_last_record - 1];
        pointer_last_record--;
        switch (tr_unit.type) {
            case type_operation::write:
                if (tr_unit.newEmploy != nullptr)
                {
                    tree[tr_unit.oldNameDepart].erase(*tr_unit.newEmploy);
                }
                else
				{
                    tree.erase(tr_unit.newNameDepart);
                }
				break;
            case type_operation::erase:
                if (tr_unit.oldEmploy != nullptr)
                {
                    tree[tr_unit.oldNameDepart].insert(*tr_unit.oldEmploy);
                }
                else
				{
                    std::copy(tr_unit.old_data.begin(),
                            tr_unit.old_data.end(),
                            std::inserter(tree[tr_unit.oldNameDepart], tree[tr_unit.oldNameDepart].begin()));
                }
                break;
            case type_operation::change:
                if (tr_unit.oldEmploy != nullptr)
                {
                    auto emplHandler = tree[tr_unit.oldNameDepart].extract(*tr_unit.newEmploy);
                    emplHandler.value() = *tr_unit.oldEmploy;
                    tree[tr_unit.oldNameDepart].insert(std::move(emplHandler));
                }
                else
				{
                    auto nodeHandler = tree.extract(tr_unit.newNameDepart);
                    nodeHandler.key() = tr_unit.oldNameDepart;
                    tree.insert(std::move(nodeHandler));
                }
                break;
            default:
                std::cerr << "Can't recognize type command" << '\n';
        }
    }

    bool ManagerXML::exist(std::string &name)
    {
		if (!is_open)
		{
			std::cerr << "File don't open" << '\n';
			return false;
		}

        return tree.end() != tree.find(name);
    }
//----------------------------------------------------Interface---------------------------------------------------------
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
        std::string command, op, dep, up;
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
						std::string dep_name;
						std::cout << "Input name department -> ";
						getline(std::cin, dep_name);
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
                up = command.substr(space);

                unsigned int start_dep = command.find_first_of('\"') + 1;
                unsigned int len = command.find_last_of('\"') - start_dep;
                dep = command.substr(start_dep, len);

                if (up == "..")
                    if (depth == 0)
                        std::cerr << "We're the top tree" << '\n';
                    else
                    {
                        depth--;
                        current_department = "";
                    }

                else
                    if (depth == 1)
                        std::cerr << "We're the bottom tree" << '\n';
                    else
                    {
                        if (manager->exist(dep))
                        {
                            depth++;
                            current_department = dep;
                        }
                        else
                            std::cerr << "This department does't exist" << '\n';
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
						dep = command.substr(start_dep, len);

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
            	// TODO
                std::cout << dep << '\n';
                unsigned int pos = dep.find('\"') + 1;
                std::string cur_name = dep.substr(0, pos);
                cur_name = cur_name.substr(0, cur_name.length() - 1);
                std::string new_name = dep.substr(pos, dep.find('\"'));
                new_name = new_name.substr(2, new_name.length());

                switch (depth)
                {
                    case 0:
                        manager->put(std::move(CombineBlock::create_dep(type_operation::change, new_name, cur_name)));
                        break;
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
