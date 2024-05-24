#ifndef _TYPES_MAP_HPP_
#define _TYPES_MAP_HPP_

#include <map>
#include <typeinfo>
#include <typeindex>
#include "comm_types.h"
#include "cam_types.h"

inline std::map<std::type_index, unsigned char> get_types_map()
{
	std::map<std::type_index, unsigned char> types_map;
	types_map[typeid(int)] = UDSUNI_TYPE_INT;
	types_map[typeid(SharedFrame)] = UDSUNI_TYPE_SHARED_FRAME;

	return types_map;
};

#endif //_TYPES_MAP_HPP_
