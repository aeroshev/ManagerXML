// Created by Ерошев Артём

#include "xml_reader.h"


namespace xml_rd
{
    ManagerXML::ManagerXML(size_t size_cache):
    size_cache(size_cache),
    cache_(1024),
    pointer_last_record(0),
    is_open(false)
    {}

    void ManagerXML::load_file(const std::string &path)
    {
        this->path = std::filesystem::absolute(path);
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

        double avg_sal;
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
			cache_[pointer_last_record] = std::move(*box);
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

        auto tr_unit = std::move(cache_[pointer_last_record - 1]);
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
}
