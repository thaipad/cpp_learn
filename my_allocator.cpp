#include <iostream> 
#include <string>
#include <map>

class SmallAllocator {
private:
	char Memory[1048576];
	std::multimap<char*, unsigned int> used_blocks;

public:
	SmallAllocator() {
		used_blocks.insert(std::pair<char*, unsigned int>(Memory + sizeof(Memory), 0));
	}
	
	void *Alloc(unsigned int size) {
		char *ptr_end_of_prev = Memory;
		for (auto it : used_blocks) {
			if (it.first - ptr_end_of_prev >= size) {
				std::pair<char*, unsigned int> val{ ptr_end_of_prev, size };
				used_blocks.insert(std::pair<char*, unsigned int>(ptr_end_of_prev, size));
				return ptr_end_of_prev;
			}
			else {
				ptr_end_of_prev = it.first + it.second;
			}	
		}
		return NULL;
	};

	void *ReAlloc(void *pointer, unsigned int size) {
		char *old_ptr = (char*) pointer;
		auto ptr_for_free = used_blocks.find(old_ptr);
		if (ptr_for_free == used_blocks.end()) {
			return NULL;
		}
		unsigned int old_size = ptr_for_free->second;
		if (old_size < size) {
			bool flag_move = false;
			for (auto i = 0; i < size - old_size; i++) {
				if (used_blocks.find(old_ptr + old_size + i) != used_blocks.end()) {
					flag_move = true;
					i = size - old_size;
				}
			}
			if (flag_move) {
				Free(pointer);
				char *new_ptr = (char*) Alloc(size);
				for (auto i = 0; i < old_size; i++) {
					*(new_ptr + i) = *(old_ptr + i);
				}
				old_ptr = new_ptr; 
			}
		}
		ptr_for_free->second = size;
		return old_ptr;
	};
	
	void Free(void *pointer) {
		auto ptr_for_free = used_blocks.find((char*) pointer);
		if (ptr_for_free != used_blocks.end()) {
			used_blocks.erase(ptr_for_free); 
		}		
	};
};


int main(void) {
	SmallAllocator A1;
	int * A1_P1 = (int *)A1.Alloc(sizeof(int));
	std::cout << "A1_P1: " << (int*) A1_P1 << std::endl;
	A1_P1 = (int *) A1.ReAlloc(A1_P1, 2 * sizeof(int));
	A1.Free(A1_P1);
	SmallAllocator A2;
	int * A2_P1 = (int *)A2.Alloc(10 * sizeof(int));
	std::cout << "A2_P1: " << A2_P1 << std::endl;
	for (unsigned int i = 0; i < 10; i++) A2_P1[i] = i;
	for (unsigned int i = 0; i < 10; i++) if (A2_P1[i] != i) std::cout << "ERROR 1" << std::endl;
	int * A2_P2 = (int *)A2.Alloc(10 * sizeof(int));
	std::cout << "A2_P2: " << A2_P2 << std::endl;
	for (unsigned int i = 0; i < 10; i++) A2_P2[i] = -1;
	for (unsigned int i = 0; i < 10; i++) if (A2_P1[i] != i) std::cout << "ERROR 2" << std::endl;
	for (unsigned int i = 0; i < 10; i++) if (A2_P2[i] != -1) std::cout << "ERROR 3" << std::endl;
	A2_P1 = (int *) A2.ReAlloc(A2_P1, 20 * sizeof(int));
	std::cout << "new A2_P1: " << A2_P1 << std::endl;
	for(unsigned int i = 10; i < 20; i++) A2_P1[i] = i;
	for(unsigned int i = 0; i < 20; i++) if(A2_P1[i] != i) std::cout << "ERROR 4" << std::endl;
	for(unsigned int i = 0; i < 10; i++) if(A2_P2[i] != -1) std::cout << "ERROR 5" << std::endl;
	A2_P1 = (int *) A2.ReAlloc(A2_P1, 5 * sizeof(int));
	std::cout << "new A2_P1: " << A2_P1 << std::endl;
	for(unsigned int i = 0; i < 5; i++) if(A2_P1[i] != i) std::cout << "ERROR 6" << std::endl;
	for(unsigned int i = 0; i < 10; i++) if(A2_P2[i] != -1) std::cout << "ERROR 7" << std::endl;
	A2.Free(A2_P1);
	A2.Free(A2_P2);
	return 0;
}

