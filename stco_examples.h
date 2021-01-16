#include"stco.h"
#include<iostream>

/**/
int gloable_stco() {//state of funciton stored gloably
	static int i;// all variables must be defined statically before the actual coroutine begins
	static int j;
	co_init;
	co_yield(10);
	for (i = 0; i < 3; i++)
		for (j = 0; j < 2; j++)
			co_yield(i + j);
	co_yield(43);
	co_yield(53);
	co_return(101);
	co_end;
}
#include<string>
std::string gloable_stco_with_abstract_return_type() {//state of funciton stored gloably
	static int i;
	static int j;
	co_init;
	co_yield("first yield");
	for (i = 0; i < 3; i++) {
		for (j = 0; j < 2; j++) {
			co_yield("this should be called 6 times");
		}
	}
	co_yield("hello");
	co_yield("another yield");
	co_return("return");
	co_end;
}

struct stco_vars2 {
	int stco_member_function();
private:
	co_member_id(ID);
	int i = 0;
	int j = 0;
};
int stco_vars2::stco_member_function() {//state of funciton stored local to stco_vars2 instace
	co_member_init(ID);
	co_yield(10);
	for (i = 0; i < 3; i++)
		for (j = 0; j < 2; j++)
			co_yield(i + j);
	co_yield(43);
	co_yield(53);
	co_return(101);
	co_end;
}
struct stco_vars1 {
	co_member_id(ID);
	int i = 0;
	int j = 0;
};
int stco_alike_member_function(stco_vars1&r) {//state of funciton stored local to passed stco_vars object
	co_member_init(r.ID);
	co_yield(10);
	for (r.i = 0; r.i < 3; r.i++)
		for (r.j = 0; r.j < 2; r.j++)
			co_yield(r.i + r.j);
	co_yield(43);
	co_yield(53);
	co_return(101);
	co_end;
}

void example() {
	for (int i = 0; i < 24; i++)
		std::cout << i << " :	" << gloable_stco() << std::endl;
	for (int i = 0; i < 24; i++)
		std::cout << i << " :	" << gloable_stco() << std::endl;

	for (int i = 0; i < 24; i++)
		std::cout << i << " :	" << gloable_stco_with_abstract_return_type() << std::endl;
	for (int i = 0; i < 24; i++)
		std::cout << i << " :	" << gloable_stco_with_abstract_return_type() << std::endl;

	stco_vars2 i2;
	for (int i = 0; i < 24; i++)
		std::cout << i << " :	" << i2.stco_member_function() << std::endl;
	stco_vars2 j2;
	for (int i = 0; i < 24; i++)
		std::cout << i << " :	" << j2.stco_member_function() << std::endl;

	stco_vars1 j;
	for (int i = 0; i < 24; i++)
		std::cout << i << " :	" << stco_alike_member_function(j) << std::endl;
	stco_vars1 c;
	for (int i = 0; i < 24; i++)
		std::cout << i << " :	" << stco_alike_member_function(c) << std::endl;
	system("pause");
}