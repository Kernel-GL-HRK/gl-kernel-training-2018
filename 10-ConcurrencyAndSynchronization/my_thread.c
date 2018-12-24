#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/kthread.h>
#include <linux/sched.h>
#include <linux/time.h>
#include <linux/timer.h>
#include <linux/delay.h>
#include <linux/spinlock.h>
#include <linux/rtmutex.h>
#include <linux/semaphore.h>

#define MAX_THREADS 4

static struct task_struct *threads[MAX_THREADS];

//Please use one single RT_MUTEX or SPINLOCK or SEMAPHORE
static DEFINE_RT_MUTEX(mutexname);
// static DEFINE_SPINLOCK(spinlockname);
// static DEFINE_SEMAPHORE(semaphorename); // no in hometask, I just wished to try

 static int thread_func(void *data)
{
 	while (!kthread_should_stop()) {
        pr_info(__FILE__": pid [%d] is running in thread %d\n", current->pid, (int)data);

        // please see DEFENITION to un/comment RT_MUTEX or SPINLOCK or SEMAPHORE methods below
		rt_mutex_lock(&mutexname);
        // spin_lock(&spinlockname);
        // down(&semaphorename);

		msleep_interruptible(500);
  
        // please see DEFENITION to un/comment RT_MUTEX or SPINLOCK or SEMAPHORE methods below
		rt_mutex_unlock(&mutexname);
        // spin_unlock(&spinlockname);
        // up(&semaphorename);

		msleep_interruptible(500);
	}
 	return 0;
}

 int mymodule_init(void)
{
	int i;

 	pr_info(__FILE__": Initialization\n");
 	for (i = 0; i < MAX_THREADS; i++) {
		threads[i] = kthread_run(thread_func, (void*)i, "my_thread_%d", i);
        pr_info(__FILE__": thread %d was run\n", i);
     }
 	return 0;
}

 void mymodule_exit(void)
{
	int ret, i;

 	for (i = 0; i < MAX_THREADS; i++) {
        ret = kthread_stop(threads[i]);
        if (!ret)
            pr_info(__FILE__": thread %d stopped\n", i);
	}
}

module_init(mymodule_init);
module_exit(mymodule_exit);
 
MODULE_AUTHOR("Eugene.Ovdiyuk <ovdiyuk@gmail.com>");
MODULE_DESCRIPTION("Simple sysfs module");
MODULE_LICENSE("GPL");
MODULE_VERSION("0.1");
