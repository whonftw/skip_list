#include <iostream>
#include "skip_list.h"
int main()
{
	{
		wh_util::skip_list<int> sl;
		sl.insert(1);
		sl.remove(2);
		sl.remove(1);
		for (int i = 1; i < 100; ++i)
			sl.insert(rand()%i);
		sl.display_elements();
		std::cout << sl.contains(-1);
	}
	system("pause");
}