#pragma once

#include<iostream>
#include<map>
#include<any>

class Config
{
public:
	static Config* getInstance();
	template<typename T>				//template methods have to be implemented in class body
	T getValue(const std::string& key)
	{
		T value = T();
		if (m_config.find(key) == m_config.end())	//if doesn't exist return default value
			return value;
		return std::any_cast<T>(m_config.find(key)->second);
	}
	template<typename T>
	void setValue(const std::string& key, T value)
	{
		m_config[key] = value;
	}
private:
	static Config* instance;
	Config();
	std::map<std::string, std::any> m_config;
};