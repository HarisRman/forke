#pragma once

#include <unordered_map>


template <typename Val_t>
class Modded_map {
public:
	
	inline size_t size() {
		return map.size();
	}

	inline Val_t& at(const std::string& key) {
		return map.at(key);
	}

	inline bool contains(const std::string& key)
	{
		return map.contains(key);
	}

	inline void insert(std::string& key, const Val_t& val)
	{
		map.insert({key, val});
		elements.push_back(key);
	}

	inline Val_t pop_back() {
		const Val_t data = map.at(elements.back());
		map.erase(elements.back());
		elements.pop_back();

		return data;
	}

private:
	std::unordered_map<std::string, Val_t> map;
	std::vector<std::string> elements;
};
