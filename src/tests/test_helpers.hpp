#pragma once

#include <string>
#include <boost/algorithm/string/trim.hpp>
#include "../exceptions/internalException.h"

inline std::string instance_path(std::string filepath)
{
	boost::trim(filepath);
#ifndef TEST_MODELS_PATH 
	throw InternalException("CMake variable 'TEST_MODELS_PATH' not set. Cannot access instance path.");
#endif

#ifdef TEST_MODELS_PATH 
	std::string instance_folder = TEST_MODELS_PATH;
	boost::trim(instance_folder);
	
	if(instance_folder.empty())
		throw InternalException("CMake variable 'TEST_MODELS_PATH' not set. Cannot access instance path.");

	if(instance_folder.back() != '/')
		instance_folder = instance_folder + "/";
	if(filepath.front() == '/')
		filepath = filepath.substr(1);
	if(filepath.empty())
		return instance_folder;
	else
		return instance_folder + "/" + filepath;
#endif
}
