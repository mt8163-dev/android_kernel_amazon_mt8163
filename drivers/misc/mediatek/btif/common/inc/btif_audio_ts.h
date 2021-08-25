#ifndef _BTIF_AUDIO_TS_H_
#define _BTIF_AUDIO_TS_H_

/* Report audio timestamp and GPT value corresponding to it */
void btif_update_audio_ts(unsigned int audio_ts, unsigned int *gpt_val);

/* Driver registration/deregistration */
int btif_ts_init(void);
void btif_ts_exit(void);

#endif /* _BTIF_AUDIO_TS_H_ */

