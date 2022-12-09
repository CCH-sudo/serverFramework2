#pragma once
#ifndef __CCH_CONFIG_H__
#define __CCH_CONFIG_H__

#include <memory>
#include <sstream>
#include <boost/lexical_cast.hpp>//for 类型转换
#include <string>
#include "log.h"
#include <yaml-cpp/yaml.h>
#include <vector>
#include <map>
#include <set>
#include <unordered_map>
#include <unordered_set>
#include <functional>

namespace cch
{
	//基类，放置公用属性
	class ConfigVarBase
	{
	public:
		typedef std::shared_ptr<ConfigVarBase> ptr;
	public:
		ConfigVarBase(const std::string &name, const std::string &description = "");
		virtual ~ConfigVarBase() {};
	public:
		const std::string &getName() const { return m_name; };
		const std::string &getDescription() const { return m_description; };
		virtual std::string toString() = 0;//转换成明文
		virtual bool fromString(const std::string &val) = 0; //解析
		virtual std::string getTypeName() const = 0;
	protected:
		std::string m_name;
		std::string m_description;
	};

	//F from_type    T to_type  把F转为T类型
	template<class F, class T>
	class LexicalCast
	{
	public:
		T operator() (const F& v)
		{
			return boost::lexical_cast<T>(v);
		}
	};

	//偏特化 vector
	template<class T>
	class LexicalCast<std::string, std::vector<T>>
	{
	public:
		std::vector<T> operator() (const std::string& v)
		{
			YAML::Node node = YAML::Load(v);
			
			typename std::vector<T> vec;
			std::stringstream ss;
			for (size_t i = 0; i < node.size(); i++)
			{
				ss.str("");
				ss << node[i];
				vec.push_back(LexicalCast<std::string, T>()(ss.str()));
			}
			return vec;
		}
	};

	//偏特化 vector
	template<class T>
	class LexicalCast<std::vector<T>, std::string>
	{
	public:
		std::string operator() (const std::vector<T>& v)
		{
			YAML::Node node;
			for (auto& i : v)
			{
				node.push_back(YAML::Node(LexicalCast<T, std::string>()(i)));
			}
	
			std::stringstream ss;
			ss << node;
			return ss.str();
		}
	};

	//偏特化 list
	template<class T>
	class LexicalCast<std::string, std::list<T>>
	{
	public:
		std::list<T> operator() (const std::string& v)
		{
			YAML::Node node = YAML::Load(v);

			typename std::list<T> vec;
			std::stringstream ss;
			for (size_t i = 0; i < node.size(); i++)
			{
				ss.str("");
				ss << node[i];
				vec.push_back(LexicalCast<std::string, T>()(ss.str()));
			}
			return vec;
		}
	};

	//偏特化 list
	template<class T>
	class LexicalCast<std::list<T>, std::string>
	{
	public:
		std::string operator() (const std::list<T>& v)
		{
			YAML::Node node;
			for (auto& i : v)
			{
				node.push_back(YAML::Node(LexicalCast<T, std::string>()(i)));
			}

			std::stringstream ss;
			ss << node;
			return ss.str();
		}
	};

	//偏特化 set
	template<class T>
	class LexicalCast<std::string, std::set<T>>
	{
	public:
		std::set<T> operator() (const std::string& v)
		{
			YAML::Node node = YAML::Load(v);

			typename std::set<T> vec;
			std::stringstream ss;
			for (size_t i = 0; i < node.size(); i++)
			{
				ss.str("");
				ss << node[i];
				vec.insert(LexicalCast<std::string, T>()(ss.str()));
			}
			return vec;
		}
	};

	//偏特化 set
	template<class T>
	class LexicalCast<std::set<T>, std::string>
	{
	public:
		std::string operator() (const std::set<T>& v)
		{
			YAML::Node node;
			for (auto& i : v)
			{
				node.push_back(YAML::Node(LexicalCast<T, std::string>()(i)));
			}

			std::stringstream ss;
			ss << node;
			return ss.str();
		}
	};

	//偏特化 unordered_set
	template<class T>
	class LexicalCast<std::string, std::unordered_set<T>>
	{
	public:
		std::unordered_set<T> operator() (const std::string& v)
		{
			YAML::Node node = YAML::Load(v);

			typename std::unordered_set<T> vec;
			std::stringstream ss;
			for (size_t i = 0; i < node.size(); i++)
			{
				ss.str("");
				ss << node[i];
				vec.insert(LexicalCast<std::string, T>()(ss.str()));
			}
			return vec;
		}
	};

	//偏特化 unordered_set
	template<class T>
	class LexicalCast<std::unordered_set<T>, std::string>
	{
	public:
		std::string operator() (const std::unordered_set<T>& v)
		{
			YAML::Node node;
			for (auto& i : v)
			{
				node.push_back(YAML::Node(LexicalCast<T, std::string>()(i)));
			}

			std::stringstream ss;
			ss << node;
			return ss.str();
		}
	};

	//偏特化 map
	template<class T>
	class LexicalCast<std::string, std::map<std::string, T>>
	{
	public:
		std::map<std::string, T> operator() (const std::string& v)
		{
			YAML::Node node = YAML::Load(v);

			typename std::map<std::string, T> vec;
			std::stringstream ss;
			for(auto it=node.begin(); it!=node.end(); it++)
			{
				ss.str("");
				ss << it->second;
				vec.insert(std::make_pair(it->first.Scalar(),
					LexicalCast<std::string, T>()(ss.str())));
			}
			return vec;
		}
	};

	//偏特化 map
	template<class T>
	class LexicalCast<std::map<std::string, T>, std::string>
	{
	public:
		std::string operator() (const std::map<std::string, T>& v)
		{
			YAML::Node node;
			for (auto& i : v)
			{
				node[i.first] = YAML::Load(LexicalCast<T, std::string>()(i.second));
			}

			std::stringstream ss;
			ss << node;
			return ss.str();
		}
	};

	//偏特化 unordered_map
	template<class T>
	class LexicalCast<std::string, std::unordered_map<std::string, T>>
	{
	public:
		std::unordered_map<std::string, T> operator() (const std::string& v)
		{
			YAML::Node node = YAML::Load(v);

			typename std::unordered_map<std::string, T> vec;
			std::stringstream ss;
			for (auto it = node.begin(); it != node.end(); it++)
			{
				ss.str("");
				ss << it->second;
				vec.insert(std::make_pair(it->first.Scalar(),
					LexicalCast<std::string, T>()(ss.str())));
			}
			return vec;
		}
	};

	//偏特化 unordered_map
	template<class T>
	class LexicalCast<std::unordered_map<std::string, T>, std::string>
	{
	public:
		std::string operator() (const std::unordered_map<std::string, T>& v)
		{
			YAML::Node node;
			for (auto& i : v)
			{
				node[i.first] = YAML::Load(LexicalCast<T, std::string>()(i.second));
			}

			std::stringstream ss;
			ss << node;
			return ss.str();
		}
	};

	//实现类
	//配置项
	template<class T, class FromStr = LexicalCast<std::string, T>, class ToStr = LexicalCast<T, std::string> >
	class ConfigVar : public ConfigVarBase
	{
	public:
		typedef std::shared_ptr<ConfigVar> ptr;
		typedef std::function<void(const T& old_value, const T& new_value)> on_change_cb;
	public:
		ConfigVar(const std::string &name, const T &default_value, const std::string &description = "")
			:ConfigVarBase(name, description), m_val(default_value) {};
	public:
		//其他类型转为string
		std::string toString() override
		{	//使用try catch原因：lexical_cast无法转换时会抛出异常
			try{
				//return boost::lexical_cast<std::string>(m_val);
				return ToStr()(m_val);
			}
			catch (std::exception &e){
				CCH_LOG_ERROR(CCH_LOG_ROOT()) << "ConfigVar::toString exception "
					<< e.what() << " convert: " << typeid(m_val).name() << " to string";
			}
			return "";
		}
		//string转为其他类型
		bool fromString(const std::string &val) override
		{
			try {
				//m_val = boost::lexical_cast<T>(val); 
				setValue(FromStr()(val));
			}
			catch (std::exception &e) {
				CCH_LOG_ERROR(CCH_LOG_ROOT()) << "ConfigVar::fromString exception "
					<< e.what() << " convert: string to " << typeid(m_val).name();
			}
			return false;
		}

		const T getValue() const { return m_val; }
		void setValue(const T &v) 
		{ 
			if (v == m_val)
				return;
			for (auto& i : m_cbs)
			{
				i.second(m_val, v);
			}
			m_val = v; 
		}
		std::string getTypeName() const { return typeid(T).name();}

		//添加监听
		void addListener(uint64_t key, on_change_cb cb)
		{
			m_cbs[key] = cb;
		}
		void delListener(uint64_t key)
		{
			m_cbs.erase(key);
		}
		on_change_cb getListener(uint64_t key)
		{
			auto it = m_cbs.find(key);
			return it == m_cbs.end() ? nullptr : it->second;
		}

		void clearListener()
		{
			m_cbs.clear();
		}
	private:
		T m_val;
		std::map<uint64_t, on_change_cb> m_cbs;//变更回调函数组，uint64_t key要求唯一，一般可以用hash
	};

	//管理类
	class Config
	{
	public:
		typedef std::unordered_map<std::string, ConfigVarBase::ptr> ConfigVarMap;

		template<class T>
		static typename ConfigVar<T>::ptr Lookup(const std::string &name,
			const T &default_value, const std::string &description = "")
		{	//如果没有则创建，有的话就使用有的
			auto it = s_datas.find(name);
			if (it != s_datas.end())
			{	//有
				auto tmp = std::dynamic_pointer_cast<ConfigVar<T>>(it->second);
				if (tmp)
				{
					CCH_LOG_INFO(CCH_LOG_ROOT()) << "Lookup name=" << name << " exists";
					return tmp;
				}
				else
				{
					CCH_LOG_ERROR(CCH_LOG_ROOT()) << "Lookup name=" << name << " exists but type not "
						<< typeid(T).name() << " real_type=" << it->second->getTypeName()
						<< " " << it->second ->toString();
					return nullptr;
				}
			}

			//没有则创建
			//find_first_not_of 正向查找在原字符串中第一个与指定字符串（或字符）中的任一字符都不匹配的字符，
			//返回它的位置。
			//若查找失败，则返回npos。
			if (name.find_first_not_of("abcdefghijklmnopqrstuvwxyz._0123456789") != std::string::npos)
			{
				//如果name中包含"abcdefghijklmnopqrstuvwxyz._0123456789"外的字符，则进入if
				CCH_LOG_ERROR(CCH_LOG_ROOT()) << "Lookup name invalid " << name;
				throw std::invalid_argument(name);
			}

			typename ConfigVar<T>::ptr v(new ConfigVar<T>(name, default_value, description));
			s_datas[name] = v;
			return v;
		}

		template<class T>
		static typename ConfigVar<T>::ptr Lookup(const std::string &name)
		{	//查找
			auto it = s_datas.find(name);
			if (it == s_datas.end())
				return nullptr;

			return std::dynamic_pointer_cast<ConfigVar<T>>(it->second);
		}

		static void LoadFromYaml(const YAML::Node &root);

		static ConfigVarBase::ptr LookupBase(const std::string &name);
	private:
		static ConfigVarMap s_datas;
		static ConfigVarMap& GetDatas()
		{
			static ConfigVarMap s_datas;
			return s_datas;
		}
	};

}
#endif // !__CCH_CONFIG_H__



