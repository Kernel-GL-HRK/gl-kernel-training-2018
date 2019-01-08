#include <linux/init.h>
#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/kernel.h>
#include <linux/err.h>
#include <linux/delay.h>
#include <linux/kthread.h>
#include <linux/list.h>
#include <linux/slab.h>

//default init
static int T = 3;
static int N = 1;

module_param(T, int, 0000);
module_param(N, int, 0000);


struct threadsList{
	struct list_head	list;
	struct task_struct	*thread;
};

static struct threadsList threads;

static int stopThreads(struct threadsList *const pthreads)
{	
	struct list_head	*ptr	= NULL;
	struct list_head	*next	= NULL;
	struct threadsList	*entry	= NULL;
	int isEmpty			= 0;
	if (NULL == pthreads)
		return -EINVAL;

	isEmpty = list_empty(&pthreads->list);
	if (isEmpty)
		return 0;

	list_for_each_safe(ptr, next, &pthreads->list){
		entry = list_entry(ptr, struct threadsList, list);
		kthread_stop(entry->thread);
		list_del(&entry->list);
		kfree(entry);
	}

	return 0;
}
 
static int threadfunc( void * data )
{
	
	while (!kthread_should_stop())
	{
		pr_info("thread: child process [%d] is running\n", current->pid );
		msleep(1000);
	}
	pr_info("thread: child process [%d] is completed\n", current->pid );
	return 0;
}


int startThreads(int numThreads, struct threadsList *const _threads)
{
	int cntThreads = 0;
	pr_info("thread: main process [%d] is running\n", current->pid );

	if (numThreads <= 0) {
		pr_err("Task10_module: error creating threads.\n"
			"N  is not valid. Set value greater than 0\n");
		return -EINVAL;
	}

	if (NULL == _threads)
		return -EINVAL;

	do {
		struct threadsList *newThreadNode = (struct threadsList *)kmalloc(sizeof(struct threadsList),
												GFP_KERNEL);

		if (!newThreadNode)
			return -ENOMEM;
		
		newThreadNode->thread = kthread_create( threadfunc, NULL, "Thread_%d", cntThreads);
		if (newThreadNode->thread == ERR_PTR (-ENOMEM))
			return -ENOMEM;

		wake_up_process(newThreadNode->thread);
		list_add_tail(&newThreadNode->list, &_threads->list);
		pr_info("thread_[%d] is running\n", cntThreads );
		++cntThreads;
	} while(cntThreads < numThreads);

	return 0;
}


static int __init glModule_init(void)
{
	int ret = 0;
	pr_info("Hello, world!\n");
	
	if (N <= 0) {
		pr_err("Task10_module: error creating threads.\n"
			"N  is not valid. Set value greater than 0\n");
		return -EINVAL;
	}


	if (T <= 0) {
		pr_err("Task10_module: error creating threads.\n"
			"T is not valid. Set value greater than 0\n");
		return -EINVAL;
	}

	INIT_LIST_HEAD(&threads.list);

	ret = startThreads(N, &threads);
	if (ret) return ret;

	msleep(T * 1000);
	pr_info("thread: main process [%d] is completed\n", current->pid );
	
	stopThreads(&threads);

	return 0;
}

static void __exit glModule_exit(void)
{
	pr_info("Goodbye, world!\n");
}

module_init(glModule_init);
module_exit(glModule_exit);


MODULE_LICENSE("GPL");
MODULE_AUTHOR("Roman Nikishyn");
MODULE_DESCRIPTION("A simple Linux driver.");
MODULE_VERSION("0.1");
