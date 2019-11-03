#ifndef __LG_PARTITION_H
#define __LG_PARTITION_H
/* DATA layout*/
#define LGE_FAC_WIFI_MAC_ADDR_OFFSET                    (1)
#define LGE_FAC_BT_ADDR_OFFSET                          (2)
#define LGE_FAC_IMEI_MASTER_OFFSET                      (3)
#define LGE_FAC_IMEI_NOT_MASTER_OFFSET                  (4)
#define LGE_FAC_SIM_LOCK_TYPE_OFFSET                    (5)
#define LGE_FAC_NETWORK_CODE_LIST_NUM_OFFSET            (6)
#define LGE_FAC_UNLOCK_CODE_VERIFY_FAIL_COUNT_OFFSET    (7)
#define LGE_FAC_UNLOCK_FAIL_COUNT_OFFSET                (8)
#define LGE_FAC_UNLOCK_CODE_OFFSET                      (9)
#define LGE_FAC_VERIFY_UNLOCK_CODE_OFFSET               (10)
#define LGE_FAC_UNLOCK_CODE_VALIDNESS_OFFSET            (11)
#define LGE_FAC_NETWORK_CODE_OFFSET                     (12)
#define LGE_FAC_NETWORK_CODE_VALIDNESS_OFFSET           (13)
#define LGE_FAC_INIT_SIM_LOCK_DATA_OFFSET               (14)
#define LGE_FAC_FUSG_FLAG_OFFSET                        (15)
#define LGE_FAC_DATA_VERSION_OFFSET                     (16)
#define LGE_FAC_PID_OFFSET                              (17)
#define LGE_FAC_SOFTWARE_VERSION_OFFSET                 (18)
#define LGE_FAC_IMEI_TRIPLE_OFFSET                      (19)
#define LGE_FAC_IMEI_QUADRUPLE_OFFSET                   (20)

#define LGE_FAC_IMEI_ENDMARK    (0xFFFFFFFF)

#define LGE_FAC_IMEI_0_OFFSET   LGE_FAC_IMEI_MASTER_OFFSET
#define LGE_FAC_IMEI_1_OFFSET   LGE_FAC_IMEI_NOT_MASTER_OFFSET
#define LGE_FAC_IMEI_2_OFFSET   LGE_FAC_IMEI_TRIPLE_OFFSET
#define LGE_FAC_IMEI_3_OFFSET   LGE_FAC_IMEI_QUADRUPLE_OFFSET

/*data length*/
#define LGE_FAC_PID_PART_1_LEN 22
#define LGE_FAC_PID_PART_2_LEN 10
#define LGE_FAC_PID_LEN (LGE_FAC_PID_PART_1_LEN + LGE_FAC_PID_PART_2_LEN) /* decimal(22) + ASCII(10) */
#define LGE_FAC_DATA_VERSION_LEN 4
#define LGE_FAC_FUSG_FLAG	1
#define LGE_FAC_UNLOCK_FAIL_COUNT_LEN	1
#define LGE_FAC_UNLOCK_CODE_VERIFY_FAIL_COUNT_LEN	1
#define LGE_FAC_VERIFY_UNLOCK_CODE_LEN	1
#define LGE_FAC_NETWORK_CODE_LIST_NUM_LEN	2
#define LGE_FAC_SIM_LOCK_TYPE_LEN	1
#define LGE_FAC_BT_ADDR_LEN (0x6)   /* hexadecimal */
#define LGE_FAC_IMEI_LEN (15)   /* decimal */
#define LGE_FAC_WIFI_MAC_ADDR_LEN (6)
#define LGE_FAC_SUFFIX_STR_LEN (15)
#define LGE_FAC_NC_MCC_LEN 3
#define LGE_FAC_NC_MNC_LEN 3
#define LGE_FAC_NC_GID1_LEN 8
#define LGE_FAC_NC_GID2_LEN 8
#define LGE_FAC_NC_SUBSET_LEN 2
#define LGE_FAC_NETWORK_CODE_LEN (LGE_FAC_NC_MCC_LEN + LGE_FAC_NC_MNC_LEN + \
				LGE_FAC_NC_GID1_LEN + LGE_FAC_NC_GID2_LEN + LGE_FAC_NC_SUBSET_LEN)
#define LGE_FAC_SV_LEN	60
#define LGE_FAC_FUSG_FLAG_LEN 1
#define LGE_FAC_MAX_NETWORK_CODE_LIST_NUM (40)  /* This number may be increased in the future */
#define LGE_FAC_UNLOCK_CODE_LEN (16)
#define LGE_FAC_SLTYPE_VALID_MASK 0x1F
#define LGE_FAC_SLTYPE_MASK_NETWORK 0x01
#define LGE_FAC_SLTYPE_MASK_SERVICE_PROVIDER 0x02
#define LGE_FAC_SLTYPE_MASK_NETWORK_SUBSET 0x04
#define LGE_FAC_SLTYPE_MASK_COOPERATE 0x08
#define LGE_FAC_SLTYPE_MASK_LOCK_TO_SIM 0x10
#define LGE_FAC_SLTYPE_MASK_HARDLOCK 0x20
#define LGE_FAC_SLTYPE_MASK_RESERVED_1 0x40 /* T.B.D */
#define LGE_FAC_SLTYPE_MASK_RESERVED_2 0x80 /* T.B.D */
#define LGE_FAC_MAX_UNLOCK_CODE_VERIFY_FAIL_COUNT 3

#define EMMC_BLOCK_SIZE	512

#ifndef bool
#define bool	unsigned char
#define true	1
#define false	0
#endif

typedef struct FactoryNetworkCodeTag {
	unsigned char Mcc[LGE_FAC_NC_MCC_LEN];  /* Ex) { 2, 4, 5 } */
	unsigned char Mnc[LGE_FAC_NC_MNC_LEN];  /* Ex) { 4, 3, 0xF } */
	unsigned char Gid1[LGE_FAC_NC_GID1_LEN];    /* Ex) { 0xB, 2, 0xF, 0xF, 0xF, 0xF, 0xF, 0xF } */
	unsigned char Gid2[LGE_FAC_NC_GID2_LEN];    /* Ex) { 8, 0xA, 0xF, 0xF, 0xF, 0xF, 0xF, 0xF } */
	unsigned char Subset[LGE_FAC_NC_SUBSET_LEN];    /* Ex) { 6, 2 } */
	unsigned char dummy[8];
} FactoryNetworkCode;

typedef struct FactoryUnlockCodeTag {
	unsigned char network[LGE_FAC_UNLOCK_CODE_LEN];
	unsigned char serviceProvider[LGE_FAC_UNLOCK_CODE_LEN];
	unsigned char networkSubset[LGE_FAC_UNLOCK_CODE_LEN];
	unsigned char cooperate[LGE_FAC_UNLOCK_CODE_LEN];
	unsigned char lockToSim[LGE_FAC_UNLOCK_CODE_LEN];
	unsigned char hardlock[LGE_FAC_UNLOCK_CODE_LEN];
	unsigned char reserved_1[LGE_FAC_UNLOCK_CODE_LEN];
	unsigned char reserved_2[LGE_FAC_UNLOCK_CODE_LEN];
} FactoryUnlockCode;

bool LGE_FacWriteWifiMacAddr(unsigned char *wifiMacAddr, bool needFlashProgram);
bool LGE_FacReadWifiMacAddr(unsigned char *wifiMacAddr);
bool LGE_FacWriteBtAddr(unsigned char *btAddr, bool needFlashProgram);
bool LGE_FacReadBtAddr(unsigned char *btAddr);
bool LGE_FacWriteImei(unsigned char imei_type, unsigned char *imei, bool needFlashProgram);
bool LGE_FacReadImei(unsigned char imei_type, unsigned char *imei);
bool LGE_FacWriteSimLockType(unsigned char simLockType, bool needFlashProgram);
bool LGE_FacReadSimLockType(unsigned char *simLockType);
bool LGE_FacWriteUnlockCodeVerifyFailCount(unsigned char failCount, bool needFlashProgram);
bool LGE_FacReadUnlockCodeVerifyFailCount(unsigned char *failCount);
bool LGE_FacWriteUnlockFailCount(unsigned char simLockType, unsigned char failCount, bool needFlashProgram);
bool LGE_FacReadUnlockFailCount(unsigned char simLockType, unsigned char *failCount);
bool LGE_FacWriteUnlockCode(FactoryUnlockCode *unlockCode, bool needFlashProgram);
bool LGE_FacVerifyUnlockCode(unsigned char simLockType, unsigned char *unlockCode, bool *isOk);
bool LGE_FacCheckUnlockCodeValidness(bool *isValid);
bool LGE_FacWriteNetworkCode(FactoryNetworkCode *networkCode, unsigned short networkCodeListNum, bool needFlashProgram);
bool LGE_FacReadNetworkCode(FactoryNetworkCode *networkCode, unsigned short networkCodeListNum);
bool LGE_FacWriteNetworkCodeListNum(unsigned short *networkCodeListNum, bool needFlashProgram);
bool LGE_FacReadNetworkCodeListNum(unsigned short *networkCodeListNum);
bool LGE_FacCheckNetworkCodeValidness(unsigned char simLockType, bool *isValid);
bool LGE_FacInitSimLockData(void);
bool LGE_FacReadFusgFlag(unsigned char *fusgFlag);
bool LGE_FacWriteFusgFlag(unsigned char fusgFlag, bool needFlashProgram);
bool LGE_FacReadDataVersion(unsigned char *dataVersion);
bool LGE_FacWriteDataVersion(unsigned char *dataVersion, bool needFlashProgram);
bool LGE_FacReadPid(unsigned char *pid);
bool LGE_FacWritePid(unsigned char *pid, bool needFlashProgram);
void LGE_FacGetSoftwareversion(bool isOriginalVersion, unsigned char *pVersion);
int LGE_API_test(void);

/*#define TC1_GET_NAME(fname)	"LGE_"#fname
#define TC1_FAC_NAME(fname)	LGE_##fname
#define TC1_FAC_IMEI_LEN		LGE_FAC_IMEI_LEN
#define TC1_FAC_NETWORK_CODE_LEN	LGE_FAC_NETWORK_CODE_LEN
*/
#endif
