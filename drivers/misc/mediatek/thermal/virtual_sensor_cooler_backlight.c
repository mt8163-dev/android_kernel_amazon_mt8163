#ifdef pr_fmt
#undef pr_fmt
#endif
#define pr_fmt(fmt) KBUILD_MODNAME ": " fmt


#include <linux/kernel.h>
#include <linux/delay.h>
#include <linux/module.h>
#include <linux/printk.h>
#include <linux/types.h>
#include <linux/kobject.h>

#include "mt-plat/mtk_thermal_monitor.h"
#include <linux/platform_data/mtk_thermal.h>

#define virtual_sensor_cooler_backlight_dprintk(fmt, args...) pr_debug("thermal/cooler/backlight " fmt, ##args)

#define MAX_BRIGHTNESS		255
static struct mutex bl_update_lock;
static struct mtk_cooler_platform_data cool_dev = {
	.type = "lcd-backlight",
	.state = 0,
	.max_state = THERMAL_MAX_TRIPS,
	.level = MAX_BRIGHTNESS,
	.levels = {
		175, 175, 175,
		175, 175, 175,
		175, 175, 175,
		175, 175, 175
	},
};

static int virtual_sensor_get_max_state(struct thermal_cooling_device *cdev,
			   unsigned long *state)
{
	*state = cool_dev.max_state;
	return 0;
}

static int virtual_sensor_get_cur_state(struct thermal_cooling_device *cdev,
			   unsigned long *state)
{
	*state = cool_dev.state;
	return 0;
}

static int virtual_sensor_set_cur_state(struct thermal_cooling_device *cdev,
			   unsigned long state)
{
	int level;
	unsigned long max_state;

	if (!cool_dev.cdev)
		return 0;

	mutex_lock(&(bl_update_lock));

	if (cool_dev.state == state)
		goto out;

	max_state = cool_dev.max_state;
	cool_dev.state = (state > max_state) ? max_state : state;

	if (!cool_dev.state)
		level = MAX_BRIGHTNESS;
	else
		level = cool_dev.levels[cool_dev.state - 1];

	if (level == cool_dev.level)
		goto out;

	cool_dev.level = level;
	setMaxbrightness(level, 1);

out:
	mutex_unlock(&(bl_update_lock));

	return 0;
}

static ssize_t levels_show(struct device *dev, struct device_attribute *attr, char *buf)
{
	int i;
	int offset = 0;

	if (!cool_dev.cdev)
		return -EINVAL;
	for (i = 0; i < THERMAL_MAX_TRIPS; i++)
		offset += sprintf(buf + offset, "%d %d\n", i+1, cool_dev.levels[i]);
	return offset;
}

static ssize_t levels_store(struct device *dev, struct device_attribute *attr, const char *buf, size_t count)
{
	int level, state;

	if (!cool_dev.cdev)
		return -EINVAL;
	if (sscanf(buf, "%d %d\n", &state, &level) != 2)
		return -EINVAL;
	if (state >= THERMAL_MAX_TRIPS)
		return -EINVAL;
	cool_dev.levels[state] = level;
	return count;
}

static struct thermal_cooling_device_ops cooling_ops = {
	.get_max_state = virtual_sensor_get_max_state,
	.get_cur_state = virtual_sensor_get_cur_state,
	.set_cur_state = virtual_sensor_set_cur_state,
};

static DEVICE_ATTR(levels, S_IRUGO | S_IWUSR, levels_show, levels_store);

static int virtual_sensor_cooler_backlight_register_ltf(void)
{
	virtual_sensor_cooler_backlight_dprintk("register ltf\n");

	cool_dev.cdev = thermal_cooling_device_register(cool_dev.type, &cool_dev,
						  &cooling_ops);
	if (!cool_dev.cdev)
		return -EINVAL;
	device_create_file(&cool_dev.cdev->device, &dev_attr_levels);
	mutex_init(&(bl_update_lock));

	return 0;
}

static void virtual_sensor_cooler_backlight_unregister_ltf(void)
{
	virtual_sensor_cooler_backlight_dprintk("unregister ltf\n");

	thermal_cooling_device_unregister(cool_dev.cdev);
	mutex_destroy(&(bl_update_lock));
}


static int __init virtual_sensor_cooler_backlight_init(void)
{
	int err = 0;

	virtual_sensor_cooler_backlight_dprintk("init\n");

	err = virtual_sensor_cooler_backlight_register_ltf();
	if (err)
		goto err_unreg;

	return 0;

 err_unreg:
	virtual_sensor_cooler_backlight_unregister_ltf();
	return err;
}

static void __exit virtual_sensor_cooler_backlight_exit(void)
{
	virtual_sensor_cooler_backlight_dprintk("exit\n");

	virtual_sensor_cooler_backlight_unregister_ltf();
}
module_init(virtual_sensor_cooler_backlight_init);
module_exit(virtual_sensor_cooler_backlight_exit);
