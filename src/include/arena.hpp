#pragma once

class ArenaAllocater {
public:
	inline ArenaAllocater(size_t bytes) 
	       : m_size(bytes)
	{
		m_arena = static_cast<std::byte*>(malloc(bytes));
		m_offset = m_arena;
	}

	inline ~ArenaAllocater() {
		free(m_arena);
	}	

	inline ArenaAllocater(const ArenaAllocater& other) = delete;
	inline ArenaAllocater operator=(const ArenaAllocater& other) = delete;

	template<typename T>
	inline T* alloc() {
		void* offset = m_offset;
		m_offset += sizeof(T);
		
		return static_cast<T*> (offset); 
	}
private:
	size_t m_size;
	std::byte* m_arena;
	std::byte* m_offset;
};
