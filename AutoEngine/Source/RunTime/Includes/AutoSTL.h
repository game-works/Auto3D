#pragma once
#include <list>
#include <vector>
#include <set>
#include <map>
#include <unordered_map>
#include <string>
#include<algorithm>

namespace Auto3D {

template <typename _Ty>
using LIST = std::list<_Ty>;

template <typename _Ty>
using SET = std::set<_Ty, std::less<_Ty>>;

template <typename _Ty>
using VECTOR = std::vector<_Ty>;

template <typename _First, typename _Second>
using PAIR = std::pair<_First, _Second>;

template <typename _Id, typename _Ty>
using PAIR_LIST = std::list<std::pair<_Id, _Ty>>;

template <typename _Id, typename _Ty>
using PAIR_SET = std::set<std::pair<_Id, _Ty>, std::less<std::pair<_Id, _Ty>>>;

template <typename _Id, typename _Ty>
using PAIR_VECTOR = std::vector<std::pair<_Id, _Ty>>;

template <typename _Kty, typename _Ty>
using PAIR_MAP = std::map<_Kty, _Ty, std::less<_Kty>>;

template <typename _Kty, typename _Ty>
using HASH_MAP = std::unordered_map<_Kty, _Ty>;

using STRING = std::string;

using WSTRING = std::wstring;

#define MAKE_PAIR(_First,_Second)	std::make_pair(_First,_Second)

}