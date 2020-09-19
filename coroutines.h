#pragma once
#include<thread>
#include<condition_variable>
#include<mutex>
#include<atomic>
template<class return_type>
class coroutine {
	std::condition_variable cv;
	std::condition_variable cv_initiate;
	std::mutex mtx_initiate;
	std::mutex mtx;
	bool initiated = 0;
	std::atomic< bool> executing;
	volatile bool terminating;
	std::thread*coroutine_code;
	std::thread*initiate = 0;
	return_type return_value = {};
protected:
	coroutine() :mtx(), coroutine_code(0), terminating(0) {
		executing = 1;
	}
	~coroutine() {
		terminating = 1;
		executing = 0;
		cv.notify_all();
		if (coroutine_code) {
			coroutine_code->join();
			delete coroutine_code;
			coroutine_code = 0;
		}
	}
	void prepare() {
		initiate = new std::thread([this]() {
			std::unique_lock<std::mutex>ul(mtx_initiate);
			cv_initiate.wait(ul, [this]() {return initiated; });
		});
	}
	void set_coroutine(std::thread*code){
		coroutine_code = code;
		std::unique_lock<std::mutex>ul(mtx);
		initiated = 1;
		cv_initiate.notify_all();
		cv.wait(ul, [this]() {return !executing; });
	}
	void init_coroutine() {
		initiate->join();
		delete initiate;
		initiate = 0;
		executing = 0;
		std::unique_lock<std::mutex>ul(mtx);
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
	return_type operator()() { return call(); }
	bool is_terminated()const { return !coroutine_code||terminating; }
};
#define COROUTINE_BEGIN(return_type,cname)\
class cname : public coroutine<return_type> {\
public:\
	cname() :coroutine(){prepare();\
		set_coroutine(\
			new std::thread([this]() \
			{init_coroutine();_terminate(f());}\
		));}\
private: return_type f()
#define COROUTINE_END }
#define COROUTINE_CALLER(return_type) public:return_type operator()
#define COROUTINE_CALL return call()
#define COROUTINE_VAR private:
#define COROUTINE_SUBROUTINE private:
#define COROUTINE_YIELD(value)if(_yield(value))return value;
