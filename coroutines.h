#pragma once
#include<thread>
#include<condition_variable>
#include<mutex>
#include<atomic>
template<class return_type>
class coroutine {
	std::condition_variable cv;
	std::mutex mtx;
	volatile bool initiated = 0;
	volatile bool executing = 0;
	volatile bool terminating = 0;
	std::thread*coroutine_code;
	return_type return_value = {};
protected:
	coroutine() :mtx(), coroutine_code(0), terminating(0),executing(1) {}
	~coroutine() {
		destroy_this();
	}
	void prepare_reset() {
		terminating = 0;
		executing = 1;
	}
	void destroy_this() {
		terminating = 1;
		executing = 0;
		cv.notify_all();
		if (coroutine_code) {
			coroutine_code->join();
			delete coroutine_code;
			coroutine_code = 0;
		}
	}
	void set_coroutine(std::thread*code){
		coroutine_code = code;
	}
	void init_coroutine() {
		std::unique_lock<std::mutex>ul(mtx);
		executing = 0;
		cv.notify_all();
		cv.wait(ul, [this]() {return executing||terminating; });
	};
	bool _yield(return_type rv) {
		if (terminating)return true;
		return_value = rv;
		executing = 0;
		std::unique_lock<std::mutex>ul(mtx);
		cv.notify_all();
		cv.wait(ul, [this]() {return executing||terminating; });
		ul.unlock();
		if (terminating)return true;
		return false;
	}
	return_type call() {
		if (!coroutine_code||terminating) {//error return 0
			unsigned char i[sizeof(return_type)] = {};
			return *(return_type*)&i;
		}
		executing = 1;
		std::unique_lock<std::mutex>ul(mtx);
		cv.notify_all();
		cv.wait(ul, [this]() {return !executing; });
		ul.unlock();
		return return_value;
	}
	void _terminate(return_type rv){
		if(!terminating)return_value = rv;
		terminating = 1;
		if (coroutine_code) {
			executing = 0;
			cv.notify_all();
		}
	}
public:
	coroutine(const coroutine&rhs) = delete;
	coroutine&operator=(const coroutine&rhs) = delete;
	operator bool()const{ return coroutine_code && !terminating; }
	return_type operator()() { return call(); }
	bool is_terminated()const { return !coroutine_code||terminating; }
};

//Interface
#define COROUTINE_SET set_coroutine(new std::thread([this](){init_coroutine();_terminate(f());}))
#define COROUTINE_RESET destroy_this();prepare_reset();COROUTINE_SET
#define COROUTINE_BEGIN(return_type,cname)class cname : public coroutine<return_type> {public:cname(){COROUTINE_SET;}void reset(){COROUTINE_RESET;}private: return_type f()
#define COROUTINE_END }
#define COROUTINE_CALLER(return_type) public:return_type operator()
#define COROUTINE_CALL return call()
#define COROUTINE_VAR private:
#define COROUTINE_SUBROUTINE private:
#define COROUTINE_YIELD(value)if(_yield(value))return value;
#define COROUTINE_YIELD2(value,extra)if(_yield(value)){extra;return value;}//executes extra code on termination
#define COROUTINE_STD_CALLER(return_type) public:return_type operator()(){return call();}
#define FRIEND_COROUTINE(cname)friend class cname