#include "thread_pool.h"

/*

Copyright (c) 2020 Péter Magyar

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.

*/

#include "core/engine.h"
#include "core/os/os.h"
#include "core/project_settings.h"

ThreadPool *ThreadPool::_instance;

ThreadPool *ThreadPool::get_singleton() {
	return _instance;
}

bool ThreadPool::get_use_threads() const {
	return _use_threads;
}
void ThreadPool::set_use_threads(const bool value) {
	_use_threads = value;
}

int ThreadPool::get_thread_count() const {
	return _thread_count;
}
void ThreadPool::set_thread_count(const bool value) {
	_thread_count = value;
}

float ThreadPool::get_max_work_per_frame_percent() const {
	return _max_work_per_frame_percent;
}
void ThreadPool::set_max_work_per_frame_percent(const bool value) {
	_max_work_per_frame_percent = value;
}

float ThreadPool::get_max_time_per_frame() const {
	return _max_time_per_frame;
}
void ThreadPool::set_max_time_per_frame(const bool value) {
	_max_time_per_frame = value;
}

Ref<ThreadPoolJob> ThreadPool::get_job(const Variant &object, const StringName &method, const bool create_if_needed) {
	return Ref<ThreadPoolJob>();
}

void ThreadPool::add_job(const Ref<ThreadPoolJob> &job) {
}

Ref<ThreadPoolJob> ThreadPool::create_job(const Variant &obj, const StringName &p_method, VARIANT_ARG_DECLARE) {
	Ref<ThreadPoolJob> job;
	job.instance();

	job->setup(obj, p_method, p_arg1, p_arg2, p_arg3, p_arg4, p_arg5);

	ERR_FAIL_COND_V(job->get_complete(), job);

	add_job(job);

	return job;
}

#if VERSION_MAJOR < 4
Variant ThreadPool::_create_job_bind(const Variant **p_args, int p_argcount, Variant::CallError &r_error) {
#else
Variant ThreadPool::_create_job_bind(const Variant **p_args, int p_argcount, Callable::CallError &r_error) {
#endif
	if (p_argcount < 2) {
#if VERSION_MAJOR < 4
		r_error.error = Variant::CallError::CALL_ERROR_TOO_FEW_ARGUMENTS;
#else
		r_error.error = Callable::CallError::CALL_ERROR_TOO_FEW_ARGUMENTS;
#endif

		r_error.argument = 1;
		return Variant();
	}

	if (p_args[0]->get_type() != Variant::OBJECT) {
#if VERSION_MAJOR < 4
		r_error.error = Variant::CallError::CALL_ERROR_INVALID_ARGUMENT;
#else
		r_error.error = Callable::CallError::CALL_ERROR_INVALID_ARGUMENT;
#endif

		r_error.argument = 0;
		r_error.expected = Variant::OBJECT;
		return Variant();
	}

	if (p_args[1]->get_type() != Variant::STRING) {
#if VERSION_MAJOR < 4
		r_error.error = Variant::CallError::CALL_ERROR_INVALID_ARGUMENT;
#else
		r_error.error = Callable::CallError::CALL_ERROR_INVALID_ARGUMENT;
#endif

		r_error.argument = 1;
		r_error.expected = Variant::STRING;
		return Variant();
	}

	Ref<ThreadPoolJob> job;
	job.instance();

	ERR_FAIL_COND_V(job->get_complete(), job);

	job->_setup_bind(p_args, p_argcount, r_error);

	ERR_FAIL_COND_V(job->get_complete(), job);

	add_job(job);

	return job;
}

ThreadPool::ThreadPool() {
	_instance = this;

	_use_threads = GLOBAL_DEF("thread_pool/use_threads", true);
	_thread_count = GLOBAL_DEF("thread_pool/thread_count", 4);
	//Todo Add help text, as this will only come into play if threading is disabled, or not available
	_max_work_per_frame_percent = GLOBAL_DEF("thread_pool/max_work_per_frame_percent", 25);

	//Todo this should be recalculated constantly to smooth out performance better
	_max_time_per_frame = Engine::get_singleton()->get_target_fps() * (_max_work_per_frame_percent / 100.0);

	if (!OS::get_singleton()->can_use_threads()) {
		_use_threads = false;
	}
}

ThreadPool::~ThreadPool() {
}

void ThreadPool::_bind_methods() {
	ClassDB::bind_method(D_METHOD("get_use_threads"), &ThreadPool::get_use_threads);
	ClassDB::bind_method(D_METHOD("set_use_threads", "value"), &ThreadPool::set_use_threads);
	ADD_PROPERTY(PropertyInfo(Variant::BOOL, "use_threads"), "set_use_threads", "get_use_threads");

	ClassDB::bind_method(D_METHOD("get_thread_count"), &ThreadPool::get_thread_count);
	ClassDB::bind_method(D_METHOD("set_thread_count", "value"), &ThreadPool::set_thread_count);
	ADD_PROPERTY(PropertyInfo(Variant::INT, "thread_count"), "set_thread_count", "get_thread_count");

	ClassDB::bind_method(D_METHOD("get_max_work_per_frame_percent"), &ThreadPool::get_max_work_per_frame_percent);
	ClassDB::bind_method(D_METHOD("set_max_work_per_frame_percent", "value"), &ThreadPool::set_max_work_per_frame_percent);
	ADD_PROPERTY(PropertyInfo(Variant::REAL, "max_work_per_frame_percent"), "set_max_work_per_frame_percent", "get_max_work_per_frame_percent");

	ClassDB::bind_method(D_METHOD("get_max_time_per_frame"), &ThreadPool::get_max_time_per_frame);
	ClassDB::bind_method(D_METHOD("set_max_time_per_frame", "value"), &ThreadPool::set_max_time_per_frame);
	ADD_PROPERTY(PropertyInfo(Variant::REAL, "max_time_per_frame"), "set_max_time_per_frame", "get_max_time_per_frame");

	MethodInfo mi;
	mi.arguments.push_back(PropertyInfo(Variant::OBJECT, "obj"));
	mi.arguments.push_back(PropertyInfo(Variant::STRING, "method"));

	mi.name = "create_job";
	ClassDB::bind_vararg_method(METHOD_FLAGS_DEFAULT, "create_job", &ThreadPool::_create_job_bind, mi);

	ClassDB::bind_method(D_METHOD("get_job", "object", "method", "create_if_needed"), &ThreadPool::get_job, DEFVAL(true));
	ClassDB::bind_method(D_METHOD("add_job", "job"), &ThreadPool::add_job);
}
