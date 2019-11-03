#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/slab.h>
#include <linux/rtc.h>
#include <linux/spinlock.h>
#include <linux/interrupt.h>
#include <linux/platform_device.h>
#include <linux/delay.h>

#include "lg_partition.h"

static int __init test_init(void)
{
	msleep(3000);
	pr_debug("start test kernel LG API ....\n");
	LGE_API_test();
	pr_debug("end test kernel LG API...\n");

	return 1;
}

/* should never be called */
static void __exit test_exit(void)
{

}

late_initcall(test_init);
module_exit(test_exit);

MODULE_AUTHOR("kai.zhu@mediatek.com");
MODULE_DESCRIPTION("test module");
MODULE_LICENSE("GPL");
