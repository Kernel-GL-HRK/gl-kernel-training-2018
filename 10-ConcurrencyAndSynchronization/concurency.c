#include <linux/init.h>
#include <linux/module.h>
#include <linux/kthread.h>
#include <linux/delay.h>
#include <linux/vmalloc.h>
#include <linux/timer.h>
#include <linux/rtmutex.h>

#define LOG(str) pr_info(__FILE__": "str"\n");

static int N = 1;
module_param(N, int, 0000);
MODULE_PARM_DESC(N, "Threads count to be created");

static int T = 10;
module_param(T, int, 0000);
MODULE_PARM_DESC(T, "Thread lifetime in seconds");

struct timer_list timer;
static struct task_struct **kthreads;
static int stop_flag = 0;

int th_fun(void *data)
{
	static DEFINE_RT_MUTEX(mutex);

	LOG("thread start");
	while( !kthread_should_stop() && !stop_flag ) {

		rt_mutex_lock(&mutex);
		LOG("log");
		rt_mutex_unlock(&mutex);

		msleep(1000);
	}
	LOG("thread end");
	return 0;
}

static void start_threads(void)
{
	int i = 0;

	kthreads = vmalloc(sizeof(struct task_struct*) * N);
	for (i = 0; i < N; i++){
		kthreads[i]= kthread_run(th_fun, NULL, "test_thread_%d", i);
	}
}

static void stop_threads(void)
{
	int i = 0;

	if (!stop_flag)
		for (i = 0; i < N; i++){
			LOG("thread force stop");
			kthread_stop(kthreads[i]);
		}
}

void timer_cb(unsigned long data)
{
	stop_flag = 1;
}

static int mymodule_init(void)
{
	start_threads();

	setup_timer(&timer, timer_cb, T * 1000);
	mod_timer(&timer, jiffies + msecs_to_jiffies(T * 1000));

	LOG("module loaded");
	return 0;
}

static void mymodule_exit(void)
{
	stop_threads();
	vfree(kthreads);
	del_timer(&timer);
	LOG("module exited");
}

module_init(mymodule_init);
module_exit(mymodule_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Sergii Pasechnyi");
MODULE_DESCRIPTION("A simple sysfs kernel module.");
MODULE_VERSION("0.1");
