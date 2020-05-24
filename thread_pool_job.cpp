/*
Copyright (c) 2019-2020 Péter Magyar

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

#include "thread_pool_job.h"

bool ThreadPoolJob::get_complete() const {
	return _complete;
}
void ThreadPoolJob::set_complete(const bool value) {
	_complete = value;
}

bool ThreadPoolJob::get_limit_execution_time() const {
	return _limit_execution_time;
}
void ThreadPoolJob::set_limit_execution_time(const bool value) {
	_limit_execution_time = value;
}

float ThreadPoolJob::get_max_allocated_time() const {
	return _max_allocated_time;
}
void ThreadPoolJob::set_max_allocated_time(const float value) {
	_max_allocated_time = value;
}

int ThreadPoolJob::get_start_time() const {
	return _start_time;
}
void ThreadPoolJob::set_start_time(const int value) {
	_start_time = value;
}

int ThreadPoolJob::get_current_run_stage() const {
	return _current_run_stage;
}
void ThreadPoolJob::set_current_run_stage(const int value) {
	_current_run_stage = value;
}

int ThreadPoolJob::get_stage() const {
	return _stage;
}
void ThreadPoolJob::set_stage(const int value) {
	_stage = value;
}

float ThreadPoolJob::get_current_execution_time() {
	return 0;
}

bool ThreadPoolJob::should_skip() {
	if (_current_run_stage < _stage) {
		++_current_run_stage;
		return true;
	}

	++_current_run_stage;
	++_stage;

	return false;
}
bool ThreadPoolJob::should_continue() {
	if (!_limit_execution_time)
		return true;

	return true;
}

void ThreadPoolJob::execute() {
	ERR_FAIL_COND(!_object);
	ERR_FAIL_COND(!_object->has_method(_method));

	_object->call(_method, argptr[0], argptr[1], argptr[2], argptr[3], argptr[4]);
}
void ThreadPoolJob::setup(const Variant &obj, const StringName &p_method, VARIANT_ARG_DECLARE) {
	_complete = false;
	_object = obj;
	_method = p_method;

	argptr[0] = &p_arg1;
	argptr[1] = &p_arg2;
	argptr[2] = &p_arg3;
	argptr[3] = &p_arg4;
	argptr[4] = &p_arg5;

	if (!_object || !_object->has_method(p_method)) {
		_complete = true;

		ERR_FAIL_COND(!_object);
		ERR_FAIL_COND(!_object->has_method(p_method));
	}
}

#if VERSION_MAJOR < 4
Variant ThreadPoolJob::_setup_bind(const Variant **p_args, int p_argcount, Variant::CallError &r_error) {
#else
Variant ThreadPoolJob::_setup_bind(const Variant **p_args, int p_argcount, Callable::CallError &r_error) {
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

	_complete = false;
	_object = *argptr[0];
	_method = *argptr[1];

	if (p_argcount > 2)
		argptr[0] = argptr[2];

	if (p_argcount > 3)
		argptr[1] = argptr[3];

	if (p_argcount > 4)
		argptr[2] = argptr[4];

	if (p_argcount > 5)
		argptr[3] = argptr[5];

	if (p_argcount > 6)
		argptr[4] = argptr[6];

	if (!_object || !_object->has_method(_method)) {
		_complete = true;
	}

#if VERSION_MAJOR < 4
	r_error.error = Variant::CallError::CALL_OK;
#else
	r_error.error = Callable::CallError::CALL_OK;
#endif

	return Variant();
}

ThreadPoolJob::ThreadPoolJob() {
	_complete = true;

	_limit_execution_time = false;
	_max_allocated_time = 0;
	_start_time = 0;

	_current_run_stage = 0;
	_stage = 0;

	_object = NULL;
}
ThreadPoolJob::~ThreadPoolJob() {
}

void ThreadPoolJob::_bind_methods() {
	ClassDB::bind_method(D_METHOD("get_complete"), &ThreadPoolJob::get_complete);
	ClassDB::bind_method(D_METHOD("set_complete", "value"), &ThreadPoolJob::set_complete);
	ADD_PROPERTY(PropertyInfo(Variant::BOOL, "complete"), "set_complete", "get_complete");

	ClassDB::bind_method(D_METHOD("get_limit_execution_time"), &ThreadPoolJob::get_limit_execution_time);
	ClassDB::bind_method(D_METHOD("set_limit_execution_time", "value"), &ThreadPoolJob::set_limit_execution_time);
	ADD_PROPERTY(PropertyInfo(Variant::BOOL, "limit_execution_time"), "set_limit_execution_time", "get_limit_execution_time");

	ClassDB::bind_method(D_METHOD("get_start_time"), &ThreadPoolJob::get_start_time);
	ClassDB::bind_method(D_METHOD("set_start_time", "value"), &ThreadPoolJob::set_start_time);
	ADD_PROPERTY(PropertyInfo(Variant::INT, "start_time"), "set_start_time", "get_start_time");

	ClassDB::bind_method(D_METHOD("get_current_run_stage"), &ThreadPoolJob::get_current_run_stage);
	ClassDB::bind_method(D_METHOD("set_current_run_stage", "value"), &ThreadPoolJob::set_current_run_stage);
	ADD_PROPERTY(PropertyInfo(Variant::INT, "current_run_stage"), "set_current_run_stage", "get_current_run_stage");

	ClassDB::bind_method(D_METHOD("get_stage"), &ThreadPoolJob::get_stage);
	ClassDB::bind_method(D_METHOD("set_stage", "value"), &ThreadPoolJob::set_stage);
	ADD_PROPERTY(PropertyInfo(Variant::INT, "stage"), "set_stage", "get_stage");

	ClassDB::bind_method(D_METHOD("get_current_execution_time"), &ThreadPoolJob::get_current_execution_time);

	ClassDB::bind_method(D_METHOD("should_skip"), &ThreadPoolJob::should_skip);
	ClassDB::bind_method(D_METHOD("should_continue"), &ThreadPoolJob::should_continue);

	ClassDB::bind_method(D_METHOD("execute"), &ThreadPoolJob::execute);

	MethodInfo mi;
	mi.arguments.push_back(PropertyInfo(Variant::OBJECT, "obj"));
	mi.arguments.push_back(PropertyInfo(Variant::STRING, "method"));

	mi.name = "setup";
	ClassDB::bind_vararg_method(METHOD_FLAGS_DEFAULT, "setup", &ThreadPoolJob::_setup_bind, mi);
}
