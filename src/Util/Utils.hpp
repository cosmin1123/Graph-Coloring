#ifndef UTILS_H
#define UTILS_H
#include <vector>

namespace Utils
{
	template <typename T>
	inline bool IsInVector(std::vector<T> vec, T elem)
	{
		for (int i = 0; i < vec.size(); i++)
		{
			if (elem == vec[i])
			{
				return true;
			}
		}
		return false;
	}
}

#endif
