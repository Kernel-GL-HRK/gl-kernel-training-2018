// SPDX-License-Identifier: GPL-2.0
#define DEBUG 1
#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/kthread.h>
#include <linux/delay.h>
#include <linux/slab.h>
#include <linux/list.h>
#include <linux/rtmutex.h>

#define MODULE_TAG "threadmodule"

//Different ways to create a thread
#define CREATE_THREAD_EASY

static int Twork = 5;
module_param(Twork, int, 0444);
MODULE_PARM_DESC(Twork, "Delay time. Default 5 sec");

static int threadnum = 3;
module_param(threadnum, int, 0444);
MODULE_PARM_DESC(threadnum, "Number of threads. Default 3");

struct thread_list_node {
	struct task_struct *task;
	struct list_head list;
};
static LIST_HEAD(threads_list);

DEFINE_RT_MUTEX(mutex);

static int thrad_func(void *data)
{
	static char task_name[TASK_COMM_LEN];

	while(!kthread_should_stop()){
		if(rt_mutex_is_locked(&mutex)){
			pr_debug("RT Mutex is locked...wait\n");
		}

		rt_mutex_lock(&mutex);
		get_task_comm(task_name, current);
		pr_debug("Hello! from %s\n", task_name);
		rt_mutex_unlock(&mutex);

		ssleep(1);
	}
	return 0;
}

static int __init tmod_init(void)
{
	int ret = 0;
	char task_name[TASK_COMM_LEN];
	static int tnum = 0;
	struct thread_list_node *tlistnode;
	pr_debug("Loading %s\n", MODULE_TAG);

	for(tnum = 0; tnum < threadnum; tnum++) {
		tlistnode = kzalloc(sizeof(struct thread_list_node), GFP_KERNEL);
		if(!tlistnode){
			ret = -ENOMEM;
			goto err;
		}
		tlistnode->task = kthread_run(thrad_func, NULL, "%d:thread_%d", current->pid, tnum);
		if(IS_ERR(tlistnode->task)){
			ret = PTR_ERR(tlistnode->task);
			kfree(tlistnode);
			goto err;
		}
		list_add_tail(&tlistnode->list, &threads_list);

		get_task_comm(task_name, tlistnode->task);
		pr_debug("Task %s is running\n", task_name);
	}

	ssleep(Twork);

	if (!list_empty(&threads_list)) {
        list_for_each_entry(tlistnode, &threads_list, list) {
          kthread_stop(tlistnode->task);
		  get_task_comm(task_name, tlistnode->task);
		  pr_debug("Task %s is stopped\n", task_name);
        }
	}

	pr_debug("%s loaded\n", MODULE_TAG);
	return 0;
err:
	if (!list_empty(&threads_list)) {
        list_for_each_entry(tlistnode, &threads_list, list) {
          kfree(tlistnode);
        }
	}
	pr_debug("%s exit with error %d\n", MODULE_TAG, ret);
	return ret;
}




static void __exit tmod_exit(void)
{
	struct thread_list_node *tlistnode;

	if (!list_empty(&threads_list)) {
        list_for_each_entry(tlistnode, &threads_list, list) {
          kfree(tlistnode);
        }
	}

	pr_debug("Test module unloaded\n");
}


module_init(tmod_init);
module_exit(tmod_exit);

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("A Thread test module");
MODULE_AUTHOR("Aleksandr Androsov <eelleekk@gmail.com>");
