// SPDX-License-Identifier: GPL-2.0
#define DEBUG 1
#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/kthread.h>
#include <linux/delay.h>

#define MODULE_TAG "threadmodule"

//Different ways to create a thread
#define CREATE_THREAD_EASY

static int param = 5;
module_param(param, int, 0444);
MODULE_PARM_DESC(param, "Delay time. Default 5 sec");

static int thrad_func(void *data)
{
	ssleep(2);
	return 0;
}

static int __init tmod_init(void)
{
	struct task_struct *task; 
	char task_name[TASK_COMM_LEN];
	static int tnum = 0;
	pr_debug("Loading %s\n", MODULE_TAG);

#ifdef CREATE_THREAD_EASY
	task = kthread_run(thrad_func, NULL, "%d:thread_%d", current->pid, tnum++);	
#else
	task = kthread_create(thrad_func, NULL, "%d:thread_%d", current->pid, tnum++);
	if(!IS_ERR(task)){
		wake_up_process(task);
	}
#endif

	if(IS_ERR(task)){
		return PTR_ERR(task);
	}

	get_task_comm(task_name, task);
	pr_debug("Task %s is running\n", task_name);

	ssleep(3);

	pr_debug("%s loaded\n", MODULE_TAG);
	return 0;
}




static void __exit tmod_exit(void)
{
	pr_debug("Test module unloaded\n");
}


module_init(tmod_init);
module_exit(tmod_exit);

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("A Thread test module");
MODULE_AUTHOR("Aleksandr Androsov <eelleekk@gmail.com>");
