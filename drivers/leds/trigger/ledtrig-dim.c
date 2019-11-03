/*
 * ledtrig-dim.c
 *
 * Copyright 2017 Amazon Technologies, Inc. All Rights Reserved.
 *
 * The code contained herein is licensed under the GNU General Public
 * License Version 2. You may obtain a copy of the GNU General Public License
 * Version 2 or later at the following locations:
 *
 * http://www.opensource.org/licenses/gpl-license.html
 * http://www.gnu.org/copyleft/gpl.html
 */
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/slab.h>
#include <linux/timer.h>
#include <linux/sched.h>
#include <linux/leds.h>
#include <linux/reboot.h>
#include <linux/of.h>
#include "../leds.h"

#define DIM_DT_NODE "dim-trigger"
#define DIM_THRESHOLD_DT_NODE "dim_threshold"
#define BRIGHT_THRESHOLD_DT_NODE "brightness_threshold"
#define DIM_THRESHOLD_DEFAULT 10
#define BRIGHT_THRESHOLD_DEFAULT 60

struct dim_trig_data_struct {
	unsigned int als_dimming_threshold;
	unsigned int als_brightness_threshold;
} dim_trig_data;

#ifdef CONFIG_LEDS_TRIGGER_DIM_DEBUG
static ssize_t dim_threshold_show(struct device *dev, struct device_attribute *attr,
				 char *buf) {
	return sprintf(buf, "%d\n", dim_trig_data.als_dimming_threshold);
}

static ssize_t dim_threshold_store(struct device *dev, struct device_attribute *attr,
				   const char *buf, size_t count) {
	int ret = kstrtouint(buf, 10, &dim_trig_data.als_dimming_threshold);
	if (ret) {
		pr_err("%s: %u: could not parse dimming threshold: %d\n", __func__, __LINE__, ret);
		return ret;
	}

	return count;
}

static ssize_t brightness_threshold_show(struct device *dev, struct device_attribute *attr,
				 char *buf) {
	return sprintf(buf, "%d\n", dim_trig_data.als_brightness_threshold);
}

static ssize_t brightness_threshold_store(struct device *dev, struct device_attribute *attr,
				   const char *buf, size_t count) {
	int ret = kstrtouint(buf, 10, &dim_trig_data.als_brightness_threshold);
	if (ret) {
		pr_err("%s: %u: could not parse brightness threshold: %d\n", __func__, __LINE__, ret);
		return ret;
	}

	return count;
}

DEVICE_ATTR_RW(dim_threshold);
DEVICE_ATTR_RW(brightness_threshold);
#endif

static void dim_trig_activate(struct led_classdev *led_cdev)
{
#ifdef CONFIG_LEDS_TRIGGER_DIM_DEBUG
	struct device *dev = led_cdev->dev;
	int ret;

	ret = device_create_file(dev, &dev_attr_dim_threshold);
	if (ret) {
		pr_err("%s: %u: could not create dim threshold sys entry: %d\n", __func__, __LINE__, ret);
	}

	ret = device_create_file(dev, &dev_attr_brightness_threshold);
	if (ret) {
		pr_err("%s: %u: could not create brightness threshold sys entry: %d\n", __func__, __LINE__, ret);
	}
#endif
}

static void dim_trig_deactivate(struct led_classdev *led_cdev)
{
#ifdef CONFIG_LEDS_TRIGGER_DIM_DEBUG
	struct device *dev = led_cdev->dev;

	device_remove_file(dev, &dev_attr_dim_threshold);
	device_remove_file(dev, &dev_attr_brightness_threshold);
#endif
}

static struct led_trigger dim_led_trigger = {
	.name     = "dim",
	.activate = dim_trig_activate,
	.deactivate = dim_trig_deactivate,
};


void led_dim_function(unsigned int alsdata)
{

	pr_debug("%s: %u: als value is: %d\n", __func__, __LINE__, alsdata);

	if (alsdata < dim_trig_data.als_dimming_threshold)
		led_trigger_event(&dim_led_trigger, LED_OFF);
	else if (alsdata > dim_trig_data.als_brightness_threshold)
		led_trigger_event(&dim_led_trigger, LED_FULL);

}

void led_dim_trigger_bright(void) {
	led_trigger_event(&dim_led_trigger, LED_FULL);
}

static int parse_dt(void) {
	struct device_node *node = NULL;
	unsigned int dimming_threshold = DIM_THRESHOLD_DEFAULT;
	unsigned int bright_threshold = BRIGHT_THRESHOLD_DEFAULT;
	int ret = 0;

	node = of_find_node_by_name(NULL, DIM_DT_NODE);
	if (NULL != node) {
		ret = of_property_read_u32(node, DIM_THRESHOLD_DT_NODE, &dimming_threshold);
		if (ret) {
			pr_err("%s: %u: could not read dimming threshold: %d\n", __func__, __LINE__, ret);
			dimming_threshold = DIM_THRESHOLD_DEFAULT;
			goto exit;
		}
		pr_debug("%s: %u: dimming threshold from dt: %d\n", __func__, __LINE__, dimming_threshold);

		ret = of_property_read_u32(node, BRIGHT_THRESHOLD_DT_NODE, &bright_threshold);
		if (ret) {
			pr_err("%s: %u: could not read brightness threshold: %d\n", __func__, __LINE__, ret);
			dimming_threshold = BRIGHT_THRESHOLD_DEFAULT;
			goto exit;
		}
		pr_debug("%s: %u: brightness threshold from dt: %d\n", __func__, __LINE__, bright_threshold);
	}

exit:
	dim_trig_data.als_dimming_threshold = dimming_threshold;
	dim_trig_data.als_brightness_threshold = bright_threshold;

	return ret;
}
static int __init dim_trig_init(void)
{
	int ret = 0;

	pr_debug("%s: %u: led dim trigger init\n", __func__, __LINE__);
	ret = parse_dt();
	if (ret) {
		pr_err("%s: %u: error reading device tree: %d\n", __func__, __LINE__, ret);
		goto exit;
	}
	ret = led_trigger_register(&dim_led_trigger);
	if (ret) {
		pr_err("%s: %u: error registering led trigger: %d\n", __func__, __LINE__, ret);
	}

exit:

	return ret;
}

static void __exit dim_trig_exit(void)
{
	led_trigger_unregister(&dim_led_trigger);
}

module_init(dim_trig_init);
module_exit(dim_trig_exit);

MODULE_AUTHOR("Praveen Krishnan <pravkri@amazon.com>");
MODULE_DESCRIPTION("Dim LED trigger");
MODULE_LICENSE("GPL");
