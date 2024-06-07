#ifndef _TYPES_MAP_HPP_
#define _TYPES_MAP_HPP_

#include <map>
#include <typeinfo>
#include <typeindex>
#include "comm_types.h"
#include "cam_types.h"
#include "ai_types.h";

struct UdsUniTypeInfo
{
	unsigned char code;
	unsigned char size;
};

inline std::map<std::type_index, unsigned char> get_types_map()
{
	std::map<std::type_index, unsigned char> types_map;
	types_map[typeid(int)] = UDSUNI_TYPE_INT;
	types_map[typeid(SharedFrame)] = UDSUNI_TYPE_SHARED_FRAME;

	return types_map;
};

inline unsigned char get_size_by_code(unsigned char code)
{
	switch (code) {
	case UDSUNI_TYPE_INT: 			return sizeof(int);
	case UDSUNI_TYPE_SHARED_FRAME : return sizeof(SharedFrame);
	}

	return 0;
};

// inline std::map<unsigned char, std::type_index> get_types_map_reversed()
// {
// 	std::map<unsigned char, std::type_index> types_map;
// 	types_map[UDSUNI_TYPE_INT] = typeid(int);
// 	types_map[UDSUNI_TYPE_SHARED_FRAME] = typeid(SharedFrame);

// 	return types_map;
// };

// template <typename K, typename V>
// inline std::map<V, K> get_reversed_map(const std::map<K, V> & _map)
// {
// 	std::map<V, K> _rmap;
//     for (const auto& p : _map)
//         _rmap[p.second] = p.first;
//     return _rmap;
// }

#endif //_TYPES_MAP_HPP_
