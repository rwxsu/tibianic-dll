#include "config.h"
#include "tools.h"

#include <fstream>
#include <sstream>

Config::Config(){
	m_loaded = false;
}

bool Config::load(std::string filename){
	m_file = filename;
	
	std::ifstream c_file(filename.c_str());
	
	if(!c_file.good()){
		return false;
	}

	std::string line;
	while(std::getline(c_file, line)){
		if(line.find(CONFIG_COMMENT_PRE) != std::string::npos){
			line = trim_s(line.substr(0, line.find(CONFIG_COMMENT_PRE)));
		}
		
		if(line.empty()){
			continue;
		}
		
		if(line.find(CONFIG_DELIMITER) == std::string::npos){
			continue;
		}
		
		std::string lhs = trim_s(line.substr(0, line.find(CONFIG_DELIMITER)));
		std::string rhs = trim_s(line.substr(line.find(CONFIG_DELIMITER) + std::string(CONFIG_DELIMITER).length()));
		
		if(lhs.empty() || rhs.empty()){
			continue;
		}
		
		m_values[lhs] = rhs;
	}
	
	c_file.close();	
	return m_loaded = true;
}

bool Config::save(){
	std::ofstream c_file(m_file.c_str());
	
	if(!c_file.good()){
		return false;
	}
	
	c_file << CONFIG_FILE_DESCRIPTION;
	
	for(valuesMap::iterator it = m_values.begin(); it != m_values.end(); ++it){
		c_file << "\n" << it->first << " " << CONFIG_DELIMITER << " " << it->second;
	}
	
	c_file.close();
	return true;
}

void Config::free(){
	m_values.clear();
	
	m_loaded = false;
}

std::string Config::getString(std::string key, std::string def /*= std::string()*/){
	valuesMap::iterator it = m_values.find(key);
	if(it != m_values.end()){
		return it->second;
	}
	
	return def;
}

int Config::getInteger(std::string key, int def /*= 0*/){
	valuesMap::iterator it = m_values.find(key);
	if(it != m_values.end()){
		return atoi(it->second.c_str());
	}
	
	return def;
}

bool Config::getBoolean(std::string key, bool def /*= false*/){
	valuesMap::iterator it = m_values.find(key);
	if(it != m_values.end()){
		if(iequals(it->second, "yes")){
			return true;
		} else if(iequals(it->second, "no")){
			return false;
		}
	}
	
	return def;
}

void Config::setString(std::string key, std::string value){
	m_values[key] = value;
}

void Config::setInteger(std::string key, int value){
	std::stringstream ss;
	ss << value;
	
	m_values[key] = ss.str();
}

void Config::setBoolean(std::string key, bool value){
	if(value){
		m_values[key] = std::string("yes");
	} else {
		m_values[key] = std::string("no");
	}
}

