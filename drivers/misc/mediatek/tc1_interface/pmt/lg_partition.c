#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/init.h>
#include <linux/printk.h>
#include <linux/kernel.h>       /* printk() */
#include <linux/slab.h>         /* kmalloc() */
#include <linux/fs.h>           /* everything... filp_open */
#include <linux/errno.h>        /* error codes */
#include <linux/types.h>        /* size_t */
#include <linux/proc_fs.h>      /*proc */
#include <linux/fcntl.h>        /* O_ACCMODE */
#include <linux/aio.h>
#include <linux/uaccess.h>        /*set_fs get_fs mm_segment_t */
#include <linux/miscdevice.h>
#include <linux/platform_device.h>
#include <linux/unistd.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/mtd/mtd.h>
#include "lg_partition.h"

#define PART_NAME "/dev/misc2"
#define PRINT	printk
int g_init_write_size = 1;

bool _LGE_GENERIC_WRITE_FUN(unsigned char *buff, unsigned int offset, unsigned int length)
{
	int ret;
	struct file *filp;
	unsigned char *tmp;
	mm_segment_t curr_fs;

	filp = filp_open(PART_NAME, O_RDWR, 0666);
	if (IS_ERR(filp)) {
		ret = PTR_ERR(filp);
		pr_err("Open MISC2 partition fail! errno=%d\n", ret);
		return -1;
	}
	pr_debug("WRITE - Open MISC2 partition OK!\n");
	if (g_init_write_size == 0) {
		struct mtd_info_user info;

		if (filp->f_op->unlocked_ioctl)
			filp->f_op->unlocked_ioctl(filp, MEMGETINFO, (unsigned long) &info);
		else if (filp->f_op->compat_ioctl)
			filp->f_op->compat_ioctl(filp, MEMGETINFO, (unsigned long) &info);

		if (info.writesize != EMMC_BLOCK_SIZE) {
			pr_err("write size error!info.writesize=%d,EMMC_BLOCK_SIZE=%d\n",
				info.writesize, EMMC_BLOCK_SIZE);
			g_init_write_size = 0;
			filp_close(filp, NULL);
			return false;
		}
		g_init_write_size = 1;
	}

	filp->f_op->llseek(filp, offset * EMMC_BLOCK_SIZE, SEEK_SET);
	tmp = kzalloc(EMMC_BLOCK_SIZE, GFP_KERNEL);
	if (!tmp) {
		pr_err("malloc memory fail!\n");
		filp_close(filp, NULL);
		return false;
	}
	memset(tmp, 0x0, EMMC_BLOCK_SIZE);
	curr_fs = get_fs();
	set_fs(KERNEL_DS);
	memcpy(tmp, buff, length);
	ret = filp->f_op->write(filp, tmp, EMMC_BLOCK_SIZE, &(filp->f_pos));
	if (EMMC_BLOCK_SIZE != ret) {
		pr_err("write fail!errno=%d\n", ret);
		filp_close(filp, NULL);
		kfree(tmp);
		set_fs(curr_fs);
		return false;
	}
	set_fs(curr_fs);
	kfree(tmp);
	filp_close(filp, NULL);
	pr_debug("WRITE -  OK!\n");

	return true;
}

bool _LGE_GENERIC_READ_FUN(unsigned char *buff, unsigned int offset, unsigned int length)
{

	int ret;
	struct file *filp;
	unsigned char *tmp;
	mm_segment_t curr_fs;

	filp = filp_open(PART_NAME, O_RDWR, 0666);
	if (IS_ERR(filp)) {
		ret = PTR_ERR(filp);
		pr_err("Open MISC2 partition fail! errno=%d\n", ret);
		return -1;
	}
	pr_debug("READ - Open MISC2 partition OK!\n");
	if (g_init_write_size == 0) {
		struct mtd_info_user info;

		if (filp->f_op->unlocked_ioctl)
			filp->f_op->unlocked_ioctl(filp, MEMGETINFO, (unsigned long) &info);
		else if (filp->f_op->compat_ioctl)
			filp->f_op->compat_ioctl(filp, MEMGETINFO, (unsigned long) &info);

		if (info.writesize != EMMC_BLOCK_SIZE) {
			pr_err("write size error!info.writesize=%d,EMMC_BLOCK_SIZE=%d\n",
				info.writesize, EMMC_BLOCK_SIZE);
			g_init_write_size = 0;
			filp_close(filp, NULL);
			return false;
		}
		g_init_write_size = 1;
	}

	filp->f_op->llseek(filp, offset * EMMC_BLOCK_SIZE, SEEK_SET);
	tmp = kzalloc(EMMC_BLOCK_SIZE, GFP_KERNEL);
	if (!tmp) {
		pr_err("malloc memory fail!\n");
		filp_close(filp, NULL);
		return false;
	}
	memset(tmp, 0x0, EMMC_BLOCK_SIZE);
	curr_fs = get_fs();
	set_fs(KERNEL_DS);

	ret = filp->f_op->read(filp, tmp, EMMC_BLOCK_SIZE, &(filp->f_pos));
	if (EMMC_BLOCK_SIZE != ret) {
		pr_err("read fail!errno=%d\n", ret);
		filp_close(filp, NULL);
		kfree(tmp);
		set_fs(curr_fs);
		return false;
	}
	memcpy(buff, tmp, length);
	set_fs(curr_fs);
	kfree(tmp);
	filp_close(filp, NULL);
	pr_debug("READ -  OK!\n");

	return true;
}

bool LGE_FacWriteWifiMacAddr(unsigned char *wifiMacAddr, bool needFlashProgram)
{
	return _LGE_GENERIC_WRITE_FUN(wifiMacAddr, LGE_FAC_WIFI_MAC_ADDR_OFFSET, LGE_FAC_WIFI_MAC_ADDR_LEN);
}
EXPORT_SYMBOL(LGE_FacWriteWifiMacAddr);

bool LGE_FacReadWifiMacAddr(unsigned char *wifiMacAddr)
{
	return _LGE_GENERIC_READ_FUN(wifiMacAddr, LGE_FAC_WIFI_MAC_ADDR_OFFSET, LGE_FAC_WIFI_MAC_ADDR_LEN);
}
EXPORT_SYMBOL(LGE_FacReadWifiMacAddr);

bool LGE_FacWriteBtAddr(unsigned char *btAddr, bool needFlashProgram)
{
	return _LGE_GENERIC_WRITE_FUN(btAddr, LGE_FAC_BT_ADDR_OFFSET, LGE_FAC_BT_ADDR_LEN);
}
EXPORT_SYMBOL(LGE_FacWriteBtAddr);

bool LGE_FacReadBtAddr(unsigned char *btAddr)
{
	return _LGE_GENERIC_READ_FUN(btAddr, LGE_FAC_BT_ADDR_OFFSET, LGE_FAC_BT_ADDR_LEN);
}
EXPORT_SYMBOL(LGE_FacReadBtAddr);

const unsigned int imei_mapping_table[4] = {LGE_FAC_IMEI_1_OFFSET,
										LGE_FAC_IMEI_0_OFFSET,
										LGE_FAC_IMEI_2_OFFSET,
										LGE_FAC_IMEI_3_OFFSET};

bool LGE_FacWriteImei(unsigned char imei_type, unsigned char *imei, bool needFlashProgram)
{
	if (imei_mapping_table[imei_type] == LGE_FAC_IMEI_ENDMARK)
		return false;

	return _LGE_GENERIC_WRITE_FUN(imei, imei_mapping_table[imei_type], LGE_FAC_IMEI_LEN);
}
EXPORT_SYMBOL(LGE_FacWriteImei);

bool LGE_FacReadImei(unsigned char imei_type, unsigned char *imei)
{
	if (imei_mapping_table[imei_type] == LGE_FAC_IMEI_ENDMARK)
		return false;

	return _LGE_GENERIC_READ_FUN(imei, imei_mapping_table[imei_type], LGE_FAC_IMEI_LEN);
}
EXPORT_SYMBOL(LGE_FacReadImei);

bool LGE_FacWriteSimLockType(unsigned char simLockType, bool needFlashProgram)
{
	return _LGE_GENERIC_WRITE_FUN(&simLockType, LGE_FAC_SIM_LOCK_TYPE_OFFSET, LGE_FAC_SIM_LOCK_TYPE_LEN);
}
EXPORT_SYMBOL(LGE_FacWriteSimLockType);

bool LGE_FacReadSimLockType(unsigned char *simLockType)
{
	return _LGE_GENERIC_READ_FUN(simLockType, LGE_FAC_SIM_LOCK_TYPE_OFFSET, LGE_FAC_SIM_LOCK_TYPE_LEN);
}
EXPORT_SYMBOL(LGE_FacReadSimLockType);

bool LGE_FacWriteNetworkCodeListNum(unsigned short *networkCodeListNum, bool needFlashProgram)
{
	return _LGE_GENERIC_WRITE_FUN((unsigned char *)&networkCodeListNum,
				LGE_FAC_NETWORK_CODE_LIST_NUM_OFFSET, LGE_FAC_NETWORK_CODE_LIST_NUM_LEN);
}
EXPORT_SYMBOL(LGE_FacWriteNetworkCodeListNum);

bool LGE_FacReadNetworkCodeListNum(unsigned short *networkCodeListNum)
{
	return _LGE_GENERIC_READ_FUN((unsigned char *)networkCodeListNum,
				LGE_FAC_NETWORK_CODE_LIST_NUM_OFFSET, LGE_FAC_NETWORK_CODE_LIST_NUM_LEN);
}
EXPORT_SYMBOL(LGE_FacReadNetworkCodeListNum);

bool LGE_FacWriteUnlockCodeVerifyFailCount(unsigned char failCount, bool needFlashProgram)
{
	return _LGE_GENERIC_WRITE_FUN(&failCount,
			LGE_FAC_UNLOCK_CODE_VERIFY_FAIL_COUNT_OFFSET, LGE_FAC_UNLOCK_CODE_VERIFY_FAIL_COUNT_LEN);
}
EXPORT_SYMBOL(LGE_FacWriteUnlockCodeVerifyFailCount);

bool LGE_FacReadUnlockCodeVerifyFailCount(unsigned char *failCount)
{
	return _LGE_GENERIC_READ_FUN(failCount,
			LGE_FAC_UNLOCK_CODE_VERIFY_FAIL_COUNT_OFFSET, LGE_FAC_UNLOCK_CODE_VERIFY_FAIL_COUNT_LEN);
}
EXPORT_SYMBOL(LGE_FacReadUnlockCodeVerifyFailCount);

bool LGE_FacWriteUnlockFailCount(unsigned char simLockType, unsigned char failCount, bool needFlashProgram)
{
	return _LGE_GENERIC_WRITE_FUN(&failCount,
				LGE_FAC_UNLOCK_FAIL_COUNT_OFFSET, LGE_FAC_UNLOCK_FAIL_COUNT_LEN);
}
EXPORT_SYMBOL(LGE_FacWriteUnlockFailCount);

bool LGE_FacReadUnlockFailCount(unsigned char simLockType, unsigned char *failCount)
{
	return _LGE_GENERIC_READ_FUN(failCount,
				LGE_FAC_UNLOCK_FAIL_COUNT_OFFSET, LGE_FAC_UNLOCK_FAIL_COUNT_LEN);
}
EXPORT_SYMBOL(LGE_FacReadUnlockFailCount);

bool LGE_FacWriteUnlockCode(FactoryUnlockCode *unlockCode, bool needFlashProgram)
{
	return _LGE_GENERIC_WRITE_FUN((unsigned char *)unlockCode,
				LGE_FAC_UNLOCK_CODE_OFFSET, LGE_FAC_UNLOCK_CODE_LEN);
}
EXPORT_SYMBOL(LGE_FacWriteUnlockCode);

bool LGE_FacVerifyUnlockCode(unsigned char simLockType, unsigned char *unlockCode, bool *isOk)
{
	*isOk = true;
	return true;
}
EXPORT_SYMBOL(LGE_FacVerifyUnlockCode);

bool LGE_FacCheckUnlockCodeValidness(bool *isValid)
{
	*isValid = true;
	return true;
}
EXPORT_SYMBOL(LGE_FacCheckUnlockCodeValidness);

bool LGE_FacWriteNetworkCode(FactoryNetworkCode *networkCode,
				unsigned short networkCodeListNum, bool needFlashProgram)
{
	return _LGE_GENERIC_WRITE_FUN((unsigned char *)networkCode,
				LGE_FAC_NETWORK_CODE_OFFSET, LGE_FAC_NETWORK_CODE_LEN);
}
EXPORT_SYMBOL(LGE_FacWriteNetworkCode);

bool LGE_FacReadNetworkCode(FactoryNetworkCode *networkCode, unsigned short networkCodeListNum)
{
	return _LGE_GENERIC_READ_FUN((unsigned char *)networkCode,
				LGE_FAC_NETWORK_CODE_OFFSET, LGE_FAC_NETWORK_CODE_LEN);
}
EXPORT_SYMBOL(LGE_FacReadNetworkCode);

bool LGE_FacCheckNetworkCodeValidness(unsigned char simLockType, bool *isValid)
{
	*isValid = true;
	return true;
}
EXPORT_SYMBOL(LGE_FacCheckNetworkCodeValidness);

bool LGE_FacInitSimLockData(void)
{
	return true;
}
EXPORT_SYMBOL(LGE_FacInitSimLockData);

bool LGE_FacReadFusgFlag(unsigned char *fusgFlag)
{
	return _LGE_GENERIC_READ_FUN(fusgFlag, LGE_FAC_FUSG_FLAG_OFFSET, LGE_FAC_FUSG_FLAG_LEN);
}
EXPORT_SYMBOL(LGE_FacReadFusgFlag);

bool LGE_FacWriteFusgFlag(unsigned char fusgFlag, bool needFlashProgram)
{
	return _LGE_GENERIC_WRITE_FUN(&fusgFlag, LGE_FAC_FUSG_FLAG_OFFSET, LGE_FAC_FUSG_FLAG_LEN);
}
EXPORT_SYMBOL(LGE_FacWriteFusgFlag);

bool LGE_FacReadDataVersion(unsigned char *dataVersion)
{
	return _LGE_GENERIC_READ_FUN(dataVersion, LGE_FAC_DATA_VERSION_OFFSET, LGE_FAC_DATA_VERSION_LEN);
}
EXPORT_SYMBOL(LGE_FacReadDataVersion);

bool LGE_FacWriteDataVersion(unsigned char *dataVersion, bool needFlashProgram)
{
	return _LGE_GENERIC_WRITE_FUN(dataVersion, LGE_FAC_DATA_VERSION_OFFSET, LGE_FAC_DATA_VERSION_LEN);
}
EXPORT_SYMBOL(LGE_FacWriteDataVersion);

bool LGE_FacReadPid(unsigned char *pid)
{
	return _LGE_GENERIC_READ_FUN(pid, LGE_FAC_PID_OFFSET, LGE_FAC_PID_LEN);
}
EXPORT_SYMBOL(LGE_FacReadPid);

bool LGE_FacWritePid(unsigned char *pid, bool needFlashProgram)
{
	return _LGE_GENERIC_WRITE_FUN(pid, LGE_FAC_PID_OFFSET, LGE_FAC_PID_LEN);
}
EXPORT_SYMBOL(LGE_FacWritePid);

void LGE_FacGetSoftwareversion(bool isOriginalVersion, unsigned char *pVersion)
{
	int index = 0;

	for (index = 0; index < LGE_FAC_SV_LEN; index++)
		pVersion[index] = index;
}
EXPORT_SYMBOL(LGE_FacGetSoftwareversion);

int LGE_API_test(void)
{
	unsigned char buff[EMMC_BLOCK_SIZE];
	int index = 0;

	memset(buff, 0xa, EMMC_BLOCK_SIZE);
	for (index = 0; index < LGE_FAC_WIFI_MAC_ADDR_LEN; index++)
		buff[index] = LGE_FAC_WIFI_MAC_ADDR_OFFSET+index;

	LGE_FacWriteWifiMacAddr(buff, true);
	memset(buff, 0xb, EMMC_BLOCK_SIZE);
	LGE_FacReadWifiMacAddr(buff);
	pr_debug("kernel wifi data:");
	for (index = 0; index < 100; index++)
		pr_debug(" 0x%x", buff[index]);

	pr_debug("\n");

	memset(buff, 0xc, EMMC_BLOCK_SIZE);
	for (index = 0; index < LGE_FAC_BT_ADDR_LEN; index++)
		buff[index] = LGE_FAC_BT_ADDR_OFFSET+0xc;

	LGE_FacWriteBtAddr(buff, true);
	memset(buff, 0xd, EMMC_BLOCK_SIZE);
	LGE_FacReadBtAddr(buff);
	pr_debug("kernel BT data:");
	for (index = 0; index < 100; index++)
		pr_debug(" 0x%x", buff[index]);

	pr_debug("\n");
	return 0;
}
EXPORT_SYMBOL(LGE_API_test);

MODULE_AUTHOR("Kai Zhu@mediatek.com");
MODULE_DESCRIPTION("access partition API");
MODULE_LICENSE("GPL");
