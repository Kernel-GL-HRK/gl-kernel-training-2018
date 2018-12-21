#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/kthread.h>
#include <linux/sched.h>
#include <linux/time.h>
#include <linux/timer.h>
#include <linux/delay.h>
#include <linux/spinlock.h>
#include <linux/rtmutex.h>

#define N_THR 4

static struct task_struct *threads[N_THR];

DEFINE_RT_MUTEX(critical_section);

static int thread_fn(void *ptr)
{

	while (!kthread_should_stop()) {
		pr_info("Kthread example: in thread %d\n", (int)ptr);
		rt_mutex_lock(&critical_section);
		msleep_interruptible(200);
		rt_mutex_unlock(&critical_section);
		msleep_interruptible(800);
	}

	return 0;
}

int thread_init(void)
{
	char thread_format[] = "thread %d";
	int i;

	pr_info("Kthread example: in init\n");

	for (i = 0; i < N_THR; i++)
		threads[i] = kthread_run(thread_fn,
				(void *)i, thread_format, i);

	return 0;
}

void thread_cleanup(void)
{
	int ret = 1;
	int i;

	for (i = 0; i < N_THR; i++) {
		if (threads[i]) {

			ret = kthread_stop(threads[i]);

			if (!ret)
				pr_info("Kthread example: thread %d stopped\n", i);

		}
	}
}

module_init(thread_init);
module_exit(thread_cleanup);

MODULE_AUTHOR("Vadym Mishchuk");
MODULE_DESCRIPTION("Kthread example module");
MODULE_LICENSE("GPL");
MODULE_VERSION("0.1");
