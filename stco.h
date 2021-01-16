//rv is the value to yield uid_int is a 32Bit unsigned integer and must be unique for each staco_yield statement in a staco region
#define co_yield_(rv,uid_int){coroutine_index=uid_int;return rv;[[fallthrough]]case uid_int:;}
#define co_yield(rv)co_yield_(rv,__COUNTER__+1)
#define co_return(rv){coroutine_index=0;return rv;}


//all variables should be predeclared before staco_init is called
//under circumstances it is possible to declare a variable inside of a staco-region
//warning should not be useed for non static member functions
#define co_init static unsigned int coroutine_index = 0;switch (coroutine_index){default:break;case 0:;
#define co_end coroutine_index=0;}

//all variables should be defined as member variables of the class or similar
//co_member_id should be written in the class member list
//co_member_id should be written once for every instance of co_member_init
//uid_str should not be the same for two diffrent coroutines
//uid_str must be a valid variable name
#define co_member_id(uid_str)unsigned int uid_str=0;
#define co_member_init(uid_str)auto&coroutine_index=uid_str;switch (coroutine_index){default:break;case 0:;