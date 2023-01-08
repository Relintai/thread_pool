#include "register_types.h"

/*

Copyright (c) 2020-2022 Péter Magyar

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

#include "core/version.h"

#include "core/object/class_db.h"

#include "core/config/engine.h"

#include "thread_pool.h"
#include "thread_pool_execute_job.h"
#include "thread_pool_job.h"

static ThreadPool *thread_pool = NULL;

void initialize_thread_pool_module(ModuleInitializationLevel p_level) {
	if (p_level == MODULE_INITIALIZATION_LEVEL_SCENE) {
		GDREGISTER_CLASS(ThreadPoolJob);
		GDREGISTER_CLASS(ThreadPoolExecuteJob);
		GDREGISTER_CLASS(ThreadPool);

		thread_pool = memnew(ThreadPool);
		Engine::get_singleton()->add_singleton(Engine::Singleton("ThreadPool", ThreadPool::get_singleton()));
	}
}

void uninitialize_thread_pool_module(ModuleInitializationLevel p_level) {
	if (p_level == MODULE_INITIALIZATION_LEVEL_SCENE) {
		if (thread_pool) {
			memdelete(thread_pool);
		}
	}
}
