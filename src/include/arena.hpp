#pragma once

class ArenaAllocater {
public:
	inline ArenaAllocater(size_t bytes) 
	       : m_size(0), cap(bytes)
	{
		m_arena = static_cast<std::byte*>(malloc(bytes));
	}

	inline ~ArenaAllocater() {
		free(m_arena);
	}	

	inline ArenaAllocater(const ArenaAllocater& other) = delete;
	inline ArenaAllocater operator=(const ArenaAllocater& other) = delete;

	template<typename T>
	inline T* alloc() {
		const size_t obj_size = sizeof(T);

		if (m_size + obj_size > cap) {
			throw std::bad_alloc();
		}

		void* obj = m_arena + m_size;
		m_size += obj_size;
		
		return static_cast<T*>(obj); 
	}
private:
	size_t m_size;
	const size_t cap;
	std::byte* m_arena;
};
