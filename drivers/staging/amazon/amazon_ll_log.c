/*
 * amazon_ll_log.c
 *
 * Store low level software log to /proc/pllk.
 *
 * Copyright (C) 2017 Amazon Technologies Inc. All rights reserved.
 * TODO: Add additional contributor's names.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */

#include <asm/tlbflush.h>
#include <linux/bootmem.h>
#include <linux/console.h>
#include <linux/dma-mapping.h>
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/memblock.h>
#include <linux/module.h>
#include <linux/platform_device.h>
#include <linux/proc_fs.h>
#include <linux/slab.h>
#include <linux/stat.h>
#include <linux/seq_file.h>
#include <linux/uaccess.h>
#include <linux/vmalloc.h>
#include <linux/kthread.h>
#ifdef CONFIG_OF
#include <linux/of.h>
#include <linux/of_device.h>
#include <linux/of_fdt.h>
#include <linux/of_reserved_mem.h>
#endif

#define LOG_MAGIC 0x414d5a4e	/* "AMZN" */

static phys_addr_t amazonlog_base;
static phys_addr_t amazonlog_size;

/* These variables are also protected by logbuf_lock */
static char *amazon_log_buf;
static unsigned int *amazon_log_pos;
static unsigned int amazon_log_size;

void *remap_lowmem(phys_addr_t start, phys_addr_t size);

static int amazon_ll_log_show(struct seq_file *m, void *v)
{
	seq_write(m, amazon_log_buf, *amazon_log_pos);
	return 0;
}

static int amazon_ll_log_file_open(struct inode *inode, struct file *file)
{
	return single_open(file, amazon_ll_log_show, inode->i_private);
}

static const struct file_operations amazon_ll_log_file_ops = {
	.owner = THIS_MODULE,
	.open = amazon_ll_log_file_open,
	.read = seq_read,
	.llseek = seq_lseek,
	.release = single_release,
};

static int insert_logs(void *data)
{
	unsigned int pos, len;


	set_current_state(TASK_UNINTERRUPTIBLE);
	schedule_timeout(30*HZ);
	pos = 0;
	while (pos < *amazon_log_pos) {
		len = strlen(amazon_log_buf + pos);
		if (pos + len < *amazon_log_pos) {
			printk(KERN_WARNING "%s", amazon_log_buf+pos);
			pos += len + 1;
		} else
			break;
	}

	return 0;

}

static int __init amazon_log_buf_init(void)
{
	unsigned int *amazon_log_mag;
	void *vaddr;
	struct proc_dir_entry *entry;
	unsigned int pos;

	if (amazonlog_base == 0 || amazonlog_size == 0) {
		pr_err("LL log: invalid reserved memory.\n");
		return 0;
	}

	vaddr = remap_lowmem(amazonlog_base, amazonlog_size);
	amazon_log_size = amazonlog_size - (sizeof(*amazon_log_pos) + sizeof(*amazon_log_mag));
	amazon_log_buf = vaddr;
	amazon_log_pos = (unsigned int *)(vaddr + amazon_log_size);
	amazon_log_mag = (unsigned int *)(vaddr + amazon_log_size + sizeof(*amazon_log_pos));

	if (*amazon_log_mag != LOG_MAGIC) {
		pr_warning("%s: no old log found\n", __func__);
		return 0;
	}

	pr_warning("%s: *amazon_log_mag:%x *amazon_log_pos:%x "
		"amazon_log_buf:%p amazon_log_size:%u\n",
		__func__, *amazon_log_mag, *amazon_log_pos, amazon_log_buf,
		amazon_log_size);

	/*
	 * Prelader log is simply a very long string, but printk doesn't work this long string.
	 * We break this long string into smaller strings, one string for each line.
	 * Also, we need combine some LK logs into a simgle line.
	 * */

	for (pos = 0; pos < *amazon_log_pos-1; pos++) {
		if (amazon_log_buf[pos] == '\r' && amazon_log_buf[pos+1] == '\n') {
			amazon_log_buf[pos] = '\n';
			amazon_log_buf[pos+1] = '\0';
		}
		if (amazon_log_buf[pos] == ' ' && amazon_log_buf[pos+1] == '\0')
			amazon_log_buf[pos+1] = ' ';

	}

	/* register_log_text_hook(emit_amazon_log, amazon_log_buf, amazon_log_size); */
	entry = proc_create("pllk", 0440, NULL, &amazon_ll_log_file_ops);
	if (!entry) {
		pr_err("ram_console: failed to create proc entry\n");
		return 0;
	}

	kthread_run(insert_logs, NULL, "LL logs");
	return 0;
}

int amazon_ll_log_reserve_memory(struct reserved_mem *rmem)
{
	pr_alert("[memblock]%s: 0x%llx - 0x%llx (0x%llx)\n", "Low level logs reserve memory",
		(unsigned long long)rmem->base,
		(unsigned long long)rmem->base + (unsigned long long)rmem->size,
		(unsigned long long)rmem->size);
	amazonlog_base = rmem->base;
	amazonlog_size = rmem->size;
	return 0;
}

#ifdef CONFIG_OF
RESERVEDMEM_OF_DECLARE(reserved_memory_ll_log, "amazon,ll_log-reserved-memory", amazon_ll_log_reserve_memory);
#endif
late_initcall(amazon_log_buf_init);
