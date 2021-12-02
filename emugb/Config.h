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
		return std::any_cast<T>(m_config.find(key)->second);
	}
	template<typename T>
	void setValue(const std::string& key, T value)
	{
		m_config[key] = value;
	}
private:
	static Config* instance;
	Config() {}
	std::map<std::string, std::any> m_config;
};