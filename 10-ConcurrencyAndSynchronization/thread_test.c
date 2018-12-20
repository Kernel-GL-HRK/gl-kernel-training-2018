#include <linux/module.h>
#include <linux/sched.h>
#include <linux/delay.h>
#include <linux/kthread.h>

static int delay_sec = 5;
static struct task_struct *thread_st;
module_param(delay_sec, int, 0);

static int thread_func(void *data)
{
	int total_sleep = 0;

	while (1) {
		if (total_sleep >= delay_sec)
			break;
		pr_info("thread: child process [%d] is running for %d seconds\n",
		current->pid, total_sleep);
		ssleep(1);
		total_sleep += 1;

	}
	pr_info("thread: child process [%d] is completed. total sleep = %d\n",
	current->pid, total_sleep);
	return 0;
}

int test_thread(void)
{
	pr_info("thread: main process [%d] is running\n", current->pid);
	thread_st = kthread_run(thread_func, NULL, "test_thread");
	ssleep(delay_sec + 2);
	pr_info("thread: main process [%d] is completed\n", current->pid);
	return -1;
}

module_init(test_thread);

MODULE_AUTHOR("Oleksandr.Siruk <alexsirook@gmail.com>");
MODULE_DESCRIPTION("Simple Kernel module example");
MODULE_LICENSE("GPL");
MODULE_VERSION("0.1");
