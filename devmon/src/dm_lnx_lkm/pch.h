#pragma once

#include <linux/version.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/device.h>
#include <linux/kernel.h>
#include <linux/uaccess.h>
#include <linux/fs.h>
#include <linux/uio.h>
#include <linux/ioctl.h>
#include <linux/poll.h>
#include <linux/cdev.h>
#include <linux/slab.h>
#include <linux/wait.h>
#include <linux/sched.h>
#include <linux/ctype.h>
#include <asm/uaccess.h>
#include <asm/ioctls.h>

#ifdef CONFIG_ARM
#	include <asm/cacheflush.h>
#endif

#define ASSERT(condition) BUG_ON(!(condition))

#if (LINUX_VERSION_CODE < KERNEL_VERSION(3, 9, 0))
#	define f_inode f_dentry->d_inode
#endif

#if (LINUX_VERSION_CODE < KERNEL_VERSION(3, 3, 0))
#	define devnode_mode_t mode_t
#else
#	define devnode_mode_t umode_t
#endif

#if (LINUX_VERSION_CODE >= KERNEL_VERSION(3, 16, 0))
#	define _DM_IOV_ITER 1
#endif
