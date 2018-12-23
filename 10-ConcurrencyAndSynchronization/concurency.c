#include <linux/init.h>
#include <linux/module.h>
#include <linux/kthread.h>
#include <linux/delay.h>

#define LOG(str) pr_info(__FILE__": "str"\n");

int th_fun(void *data)
{
	LOG("thread start");
	while( !kthread_should_stop() ) {
		LOG("log");
		msleep(1000);
	}
	LOG("thread end");
	return 0;
}

static struct task_struct *kthread;

static int mymodule_init(void)
{
	kthread = kthread_run(th_fun, NULL, "test thread");
	LOG("module loaded");
	return 0;
}

static void mymodule_exit(void)
{
	LOG("thread force stop");
	kthread_stop(kthread);
	LOG("module exited");
}

module_init(mymodule_init);
module_exit(mymodule_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Sergii Pasechnyi");
MODULE_DESCRIPTION("A simple sysfs kernel module.");
MODULE_VERSION("0.1");
