#include<iostream>
#include"log.h"
#include"util.h"
#include "config.h"
#include <yaml-cpp/yaml.h>

cch::ConfigVar<int>::ptr g_int_value_config =
cch::Config::Lookup("system.port", (int)8080, "system port");

cch::ConfigVar<float>::ptr g_float_value_config =
cch::Config::Lookup("system.value", (float)10.2f, "system value");

cch::ConfigVar<std::vector<int> >::ptr g_int_vec_value_config =
cch::Config::Lookup("system.int_vec", std::vector<int>{1,2}, "system int vec");

cch::ConfigVar<std::list<int> >::ptr g_int_list_value_config =
cch::Config::Lookup("system.int_list", std::list<int>{1, 2}, "system int list");

cch::ConfigVar<std::unordered_map<std::string, int> >::ptr g_int_umap_value_config =
cch::Config::Lookup("system.int_umap", std::unordered_map<std::string, int>{ {"k", 2}}, "system int umap");

void print_yaml(const YAML::Node& node, int level)
{
	if (node.IsScalar())
	{
		CCH_LOG_INFO(CCH_LOG_ROOT()) << std::string(level * 4, ' ') << node.Scalar()
			<< " - " << node.Type() << " - " << level;
	}
	else if (node.IsNull())
	{
		CCH_LOG_INFO(CCH_LOG_ROOT()) << std::string(level * 4, ' ') << "NULL - "
			<< node.Type() << " - " << level;
	}
	else if (node.IsMap())
	{
		for (auto it = node.begin(); it != node.end(); it++)
		{
			CCH_LOG_INFO(CCH_LOG_ROOT()) << std::string(level * 4, ' ') << it->first
				<< " - " << it->second.Type() << " - " << level;
			print_yaml(it->second, level + 1);
		}
	}
	else if (node.IsSequence())
	{
		for (size_t i = 0; i < node.size(); i++)
		{
			CCH_LOG_INFO(CCH_LOG_ROOT()) << std::string(level * 4, ' ') << i
				<< " - " << node[i].Type() << " - " << level;
			print_yaml(node[i], level + 1);
		}
	}
}

void test_yaml()
{
	YAML::Node root = YAML::LoadFile("/home/cch/projects/serverFramework/log.yml");
	print_yaml(root, 0);
	//CCH_LOG_INFO(CCH_LOG_ROOT()) << root;
}

void test_config()
{
	CCH_LOG_INFO(CCH_LOG_ROOT()) << " before " << g_int_value_config->getValue();
	CCH_LOG_INFO(CCH_LOG_ROOT()) << " before " << g_float_value_config->toString();
	auto v = g_int_umap_value_config->getValue();
	for (auto& i : v)
	{
		CCH_LOG_INFO(CCH_LOG_ROOT()) << " before " << i.first << " " << i.second;
	}
	CCH_LOG_INFO(CCH_LOG_ROOT()) << " before " << g_int_umap_value_config->toString();

	YAML::Node root = YAML::LoadFile("/home/cch/projects/serverFramework/log.yml");
	cch::Config::LoadFromYaml(root);

	CCH_LOG_INFO(CCH_LOG_ROOT()) << " after " << g_int_value_config->getValue();
	CCH_LOG_INFO(CCH_LOG_ROOT()) << " after " << g_float_value_config->toString();

	v = g_int_umap_value_config->getValue();
	for (auto& i : v)
	{
		CCH_LOG_INFO(CCH_LOG_ROOT()) << " after " << i.first << " " << i.second;
	}

	//YAML::Node root = YAML::LoadFile("/home/cch/projects/serverFramework/log.yml");
	//print_yaml(root, 0);
	//CCH_LOG_INFO(CCH_LOG_ROOT()) << root;
}

class Person {
public:
	std::string m_name;
	int m_age = 0;
	bool m_sex = 0;

	std::string toString() const {
		std::stringstream ss;
		ss << m_name << m_age << m_sex;
		return ss.str();
	}

	bool operator==(const Person& p) const
	{
		return m_name == p.m_name
			&& m_age == p.m_age
			&& m_sex == p.m_sex;
	}
};

namespace cch
{
	template<>
	class LexicalCast<std::string, Person>
	{
	public:
		Person operator() (const std::string& v)
		{
			YAML::Node node = YAML::Load(v);
			Person p;
			p.m_name = node["name"].as<std::string>();
			p.m_age = node["age"].as<int>();
			p.m_sex = node["sex"].as<bool>();
			return p;
		}
	};

	template<>
	class LexicalCast<Person, std::string>
	{
	public:
		std::string operator() (const Person& v)
		{
			YAML::Node node;
			node["name"] = v.m_name;
			node["age"] = v.m_age;
			node["sex"] = v.m_sex;

			std::stringstream ss;
			ss << node;
			return ss.str();
		}
	};
}

cch::ConfigVar<Person>::ptr g_person_config =
cch::Config::Lookup("class.person", Person(), "system person");

void test_class()
{
	CCH_LOG_INFO(CCH_LOG_ROOT()) << g_person_config->getValue().toString() << " - "
		<< g_person_config->toString();

	g_person_config->addListener(10, [](const Person& old_value, const Person& new_value) {
		CCH_LOG_INFO(CCH_LOG_ROOT()) << "old value=" << old_value.toString()
			<< " new value=" << new_value.toString();
	});

	YAML::Node root = YAML::LoadFile("/home/cch/projects/serverFramework/log.yml");
	cch::Config::LoadFromYaml(root);

	CCH_LOG_INFO(CCH_LOG_ROOT()) << g_person_config->getValue().toString() << " - "
		<< g_person_config->toString();
}

void test_log()
{
	static cch::Logger::ptr system_log = CCH_LOG_NAME("system");
	CCH_LOG_INFO(system_log) << "hh sss" << std::endl;
	std::cout << cch::loggerMgr::GetInstance()->toYamlString() << std::endl;
	YAML::Node root = YAML::LoadFile("/home/cch/projects/serverFramework/log.yml");
	cch::Config::LoadFromYaml(root);

	std::cout << "*************" << std::endl;
	std::cout << cch::loggerMgr::GetInstance()->toYamlString() << std::endl;
	CCH_LOG_INFO(system_log) << "hh sss" << std::endl;

	system_log->setFormatter("%d - %m%n");//还有问题，未生效，之后再看P19
	CCH_LOG_INFO(system_log) << "hh sss" << std::endl;
}

int main(int argc, char** argv)
{
	//CCH_LOG_INFO(CCH_LOG_ROOT()) << g_int_value_config->getValue();
	//CCH_LOG_INFO(CCH_LOG_ROOT()) << g_float_value_config->toString();
	//test_yaml();
	//test_config();
	//test_class();
	test_log();
	return 0;
}
/*
int main(int argc, char** argv)
{
	cch::Logger::ptr logger(new cch::Logger);
	logger->addAppender(cch::LogAppender::ptr(new cch::StdoutLogAppender));
	cch::LogEvent::ptr event(new cch::LogEvent(logger, logger->getLevel(), __FILE__, __LINE__, 0, cch::GetThreadId(), cch::GetFiberId(), time(0)));
	event->getSS() << "hello cch log2";
	logger->log(cch::LogLevel::DEBUG, event);
	//std::cout << event->getContent();

	//cch::LogFormatter::ptr fmt(new cch::LogFormatter("%d%T%p%T%m%n"));
	//CCH_LOG_INFO(logger) << "ddsdsfsd";
	//CCH_LOG_ERROR(logger) << "2324324";

	//CCH_LOG_FMT_ERROR(logger, "degfbvjk %s", "d334");

	//auto i = cch::loggerMgr::GetInstance()->getLogger("xx");
	//CCH_LOG_INFO(i) << "xxxx";
	return 0;
}*/