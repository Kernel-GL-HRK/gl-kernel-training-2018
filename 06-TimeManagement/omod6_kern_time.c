// SPDX-License-Identifier: AFL-3.0
/*
 * Copyright (C) 2018
 * Author: Oleg Khokhlov <oleg.khokhlov.ua@gmail.com>
 *
 * SysFS kernel module for test kernel timing functions
 *
 * doc links:
 *
 */

#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/device.h>
#include <linux/err.h>
#include <linux/uaccess.h>
#include <linux/string.h>

#include <linux/jiffies.h>
#include <linux/ktime.h>
#include <linux/timer.h>
#include <linux/rtc.h>
#include <linux/delay.h>



//Internal Kernel API (Time Management):

//1. Implement kernel module with API in sysfs or procfs, which is able to:
// - return time (in seconds) passed since previous read;
// - return absolute time of previous reading;
// - periodically print some message to log, make period adjustable;

// --------------- MODULE PARAMETERS DESCRIPTION -------------------
//int iParam;
//module_param(iParam, int, 0);
//MODULE_PARM_DESC(iParam, "iParam: ReturnCode");

//char *sParam = "Default";
//module_param(sParam, charp, 0);
//MODULE_PARM_DESC(sParam, "sParam: string parameter");

//------------ GLOBAL MODULE DATA ---------------------
static struct class *attr_class;

#define SysFS_DIR_NAME   "omod6"
#define SysFS_ATTR_NAME  "ktime"

uint32_t ModCallCount;
uint32_t LastCallTimeSec;
uint32_t LastCall_dT;
uint32_t LastCallTicks;

uint32_t TimerInterval;
uint32_t TimerCounter;
uint32_t TimerStartTick;

char LogMessage[256];

uint32_t GetTimeStamp(void)
{
	return (uint32_t) (ktime_to_ms(ktime_get_real()) / 1000);
}

uint32_t GetTickCount(void)
{
	return (uint32_t) (get_jiffies_64() * 1000 / HZ);
}


//--------------- SysFS read/write functions ----------------------

static ssize_t ktime_show(struct class *cl,
			struct class_attribute *attr,
			char *buf)
{
	int L;
	uint32_t TS, dT, T;
	struct rtc_time tm;

	T = GetTickCount();
	TS = GetTimeStamp();
	dT = TS - LastCallTimeSec;

	if (dT == 0)
		dT = LastCall_dT;
	else
		LastCall_dT = dT;

	LastCallTicks = T;

	ModCallCount++;

	sprintf(buf, "ktime (%u ticks, %u HZ):\n", (uint32_t)get_jiffies_64(), HZ);
	sprintf(strchr(buf, 0), "%u module call\n", ModCallCount);
	sprintf(strchr(buf, 0), "%u sec from last call\n", dT);
	rtc_time64_to_tm(TS, &tm);
	//https://ru.wikipedia.org/wiki/Time.h
	sprintf(strchr(buf, 0), "%u sec curr abs time from Epoch (%d.%02d.%02d, %02d:%02d:%02d)\n", TS,
		tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec);
	sprintf(strchr(buf, 0), "%u sec prev call abs time from Epoch\n", LastCallTimeSec);
	sprintf(strchr(buf, 0), "%u ms - Timer Interval\n", TimerInterval);
	sprintf(strchr(buf, 0), "%u times timer called\n", TimerCounter);

	LastCallTimeSec = TS;

	L = strlen(buf);
	return L;
}

// Kernel Timer object
struct timer_list MyTimer;

//
// Kernel Timer Function. Run in interrupt context! Called only once after start/mod
//
void KernelTimerFunc(struct timer_list *t)
{
	uint32_t T, dT;

	TimerCounter++;
	T = GetTickCount();
	dT = T - TimerStartTick;
	pr_info("omod6 timer[%u]: %s (delay = %u ms)\n", TimerCounter, LogMessage, dT);
}


static ssize_t ktime_store(struct class *cl,
			struct class_attribute *attr,
			const char *buf, size_t count)
{
	int interval = 0;
	char *cp;
	int n;

	if (sscanf(buf, "%d", &interval) == 1) {
		cp = strchr(buf, ' ');
		if (cp) {
			while (*cp == ' ')
				cp++;
			for (n = 0; n < sizeof(LogMessage) - 1; n++) {
				if (*cp < ' ')
					break;
				LogMessage[n] = *cp++;
			}
			LogMessage[n] = 0;
		}
	}

	pr_info("omod6 store (%d, \'%s\')\n", interval, LogMessage);

	TimerInterval = interval;

	// start, stop or reconfig timer
	if (interval == 0) {
		if (timer_pending(&MyTimer)) {
			del_timer(&MyTimer);
			pr_info("omod6 Timer disabled\n");
		}
	} else {
		unsigned long expired = get_jiffies_64() + interval * HZ / 1000;
		mod_timer(&MyTimer, expired);
		TimerStartTick = GetTickCount();
		pr_info("omod6 Timer started (interval=%u)\n", interval);
	}
	return count;
}

static struct class_attribute class_attr_ktime = {
	.attr = { .name = SysFS_ATTR_NAME, .mode = 0666 },
	.show = ktime_show,
	.store = ktime_store
};


static int __init omod_init(void)
{
	int ret;

	attr_class = class_create(THIS_MODULE, SysFS_DIR_NAME);
	if (attr_class == NULL) {
		pr_err("omod6: error creating sysfs class\n");
		return -EEXIST;
	}

	ret = class_create_file(attr_class, &class_attr_ktime);
	if (ret) {
		pr_err("omod6: error creating sysfs class attribute\n");
		class_destroy(attr_class);
		return ret;
	}

	timer_setup(&MyTimer, KernelTimerFunc, 0);

	pr_info("omod6 Kernel time function test module started.\n");
	LastCallTimeSec = GetTimeStamp();
	return 0;
}

static void __exit omod_exit(void)
{
	class_remove_file(attr_class, &class_attr_ktime);
	class_destroy(attr_class);

	pr_info("omod6 module normal shutdown.\n");
}

module_init(omod_init);
module_exit(omod_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Oleg Khokhlov");
MODULE_DESCRIPTION("OlegH Lesson06 module: test kernel time funcs");
MODULE_VERSION("0.2");

