#include "config.h"
#include "log.h"
#include <yaml-cpp/yaml.h>

cch::ConfigVar<int>::ptr g_int_value_config = 
	cch::Config::Lookup("system.port", (int)8080, "system port");

cch::ConfigVar<float>::ptr g_float_value_config =
	cch::Config::Lookup("system.value", (float)10.2f, "system value");

void print_yaml(const YAML::Node& node, int level)
{
	if (node.IsScalar())
	{
		CCH_LOG_INFO(CCH_LOG_ROOT()) << std::string(' ', level + 4) << node.Scalar()
			<< " - " << node.Tag() << " - " << level;
	}else if (node.IsNull())
	{
		CCH_LOG_INFO(CCH_LOG_ROOT()) << std::string(' ', level + 4)  << "NULL - "
			<< node.Tag() << " - " << level;
	}else if (node.IsMap())
	{
		for (auto it = node.begin(); it != node.end(); it++)
		{
			CCH_LOG_INFO(CCH_LOG_ROOT()) << std::string(' ', level + 4) << it->first
				<< " - " << it->second.Tag() << " - " << level;
			print_yaml(it->second, level + 1);
		}
	}else if (node.IsMap())
	{
		for (size_t i=0; i<node.size(); i++)
		{
			CCH_LOG_INFO(CCH_LOG_ROOT()) << std::string(' ', level + 4) << i
				<< " - " << node[i].Tag() << " - " << level;
			print_yaml(node[i], level + 1);
		}
	}
}

void test_yaml()
{
	YAML::Node root = YAML::LoadFile("log.yml");
	print_yaml(root, 0);
	//CCH_LOG_INFO(CCH_LOG_ROOT()) << root;
}

int main(int argc, char** argv)
{
	CCH_LOG_INFO(CCH_LOG_ROOT()) << g_int_value_config->getValue();
	CCH_LOG_INFO(CCH_LOG_ROOT()) << g_float_value_config->toString();
	test_yaml();
	return 0;
}