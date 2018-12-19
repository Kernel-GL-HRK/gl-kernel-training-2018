#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/kthread.h>
#include <linux/sched.h>
#include <linux/time.h>
#include <linux/timer.h>
#include <linux/delay.h>

#define KTHREAD_RUN 0

static struct task_struct *thread1;

static int thread_fn(void *ptr)
{
	pr_info("Kthread example: in thread1\n");

	while (!kthread_should_stop()) {
		schedule();
		set_current_state(TASK_INTERRUPTIBLE);
	}

	return 0;
}

int thread_init(void)
{
	char our_thread[8] = "thread1";

	pr_info("Kthread example: in init\n");
#if KTHREAD_RUN
	thread1 = kthread_run(thread_fn, NULL, our_thread);
#else
	thread1 = kthread_create(thread_fn, NULL, our_thread);
	if ((thread1))
		wake_up_process(thread1);
#endif
	if ((thread1))
		pr_info("Kthread example: kthread created\n");

	return 0;
}

void thread_cleanup(void)
{
	int ret = 1;

	ret = kthread_stop(thread1);

	if (!ret)
		pr_info("Kthread example: thread stopped\n");
}

module_init(thread_init);
module_exit(thread_cleanup);

MODULE_AUTHOR("Vadym Mishchuk");
MODULE_DESCRIPTION("Kthread example module");
MODULE_LICENSE("GPL");
MODULE_VERSION("0.1");
