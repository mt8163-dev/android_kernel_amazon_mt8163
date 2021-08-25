#include <linux/types.h>
#include <linux/kernel.h>

#if defined(CONFIG_MTK_BQ24297_SUPPORT) || defined(CONFIG_MTK_BQ24296_SUPPORT)
#include "bq24297.h"
#endif

/************* ATTENTATION ***************/
/* IF ANY NEW CHARGER IC SUPPORT IN THIS FILE, */
/* REMEMBER TO NOTIFY USB OWNER TO MODIFY OTG RELATED FILES!! */

void tbl_charger_otg_vbus(int mode)
{
	pr_debug("[%s] mode = %d\n", __func__, mode);

	if (mode & 0xFF) {
#if defined(CONFIG_MTK_BQ24297_SUPPORT) || defined(CONFIG_MTK_BQ24296_SUPPORT)
		bq24297_set_otg_config(0x1);	/* OTG */
		bq24297_set_boost_lim(0x1);	/* 1.5A on VBUS */
		bq24297_set_en_hiz(0x0);
#endif
	} else {
#if defined(CONFIG_MTK_BQ24297_SUPPORT) || defined(CONFIG_MTK_BQ24296_SUPPORT)
		bq24297_set_otg_config(0x0);	/* OTG & Charge disabled */
#endif
	}
};
