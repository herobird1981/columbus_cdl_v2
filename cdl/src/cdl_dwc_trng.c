#include "cdl_config.h"
#include "cdl_dwc_trng.h"
#include "cdl_sys.h"

/* notes: 
1. directly seeding is porhibited in this driver
2. no interrupt function is support except interrupt path
*/
void dwc_trng_writel(int reg, unsigned long val)
{
   __raw_writel(val, reg);
}

unsigned long dwc_trng_readl(int reg)
{
   return __raw_readl(reg);
}

static int dwc_trng_wait_on_busy(struct dwc_trng_priv *priv)
{
   uint32_t tmp, t;
   
   t = DWC_TRNG_RETRY_MAX;
   do { tmp = dwc_trng_readl((uint32_t)priv->base + DWC_TRNG_REG_STAT); } while ((tmp & DWC_TRNG_REG_STAT_BUSY) && --t);
   if (t) {
      return CRYPTO_OK;
   } else {
      return CRYPTO_TIMEOUT;
   }
}

static int dwc_trng_get_alarms(struct dwc_trng_priv *priv)
{
   uint32_t tmp;
   
   tmp = dwc_trng_readl((uint32_t)priv->base + DWC_TRNG_REG_ISTAT);
   if (tmp & DWC_TRNG_REG_ISTAT_ALARMS) {
      // alarm happened
      tmp = dwc_trng_readl((uint32_t)priv->base + DWC_TRNG_REG_ALARMS);
      DWC_TRNG_DEBUG("Received alarm: %lu\n", (unsigned long)tmp);
      // clear istat
      dwc_trng_writel((uint32_t)priv->base + DWC_TRNG_REG_ISTAT, DWC_TRNG_REG_ISTAT_ALARMS);
      dwc_trng_writel((uint32_t)priv->base + DWC_TRNG_REG_ALARMS, 0x1F);
      priv->status.alarm_code = tmp & 0x1F;
      
      if (priv->status.alarm_code != DWC_TRNG_REG_ALARM_FAILED_TEST_ID_OK) {
         dwc_trng_cmd_zeroize(priv, 0);
      }
   } else {
	   priv->status.alarm_code = 0;
   }
   return priv->status.alarm_code;
}

static int dwc_trng_wait_on_(struct dwc_trng_priv *priv, uint32_t mask)
{
   uint32_t tmp, t;
   
   t = DWC_TRNG_RETRY_MAX;
resume:
   do { tmp = dwc_trng_readl((uint32_t)priv->base + DWC_TRNG_REG_ISTAT); } while (!(tmp & (mask|DWC_TRNG_REG_ISTAT_ALARMS)) && --t);
   if (tmp & DWC_TRNG_REG_ISTAT_ALARMS) {
	   return dwc_trng_get_alarms(priv);
   }
   if (t && !(tmp & mask)) { goto resume; }

   if (t) {
      dwc_trng_writel((uint32_t)priv->base + DWC_TRNG_REG_ISTAT, DWC_TRNG_REG_ISTAT_DONE);
      return CRYPTO_OK;
   } else {
      DWC_TRNG_DEBUG("wait_on_done: failed timeout: %08lx\n", (unsigned long)tmp);
      return CRYPTO_TIMEOUT;
   }
}

static int dwc_trng_wait_on_done(struct dwc_trng_priv *priv)
{
   return dwc_trng_wait_on_(priv, DWC_TRNG_REG_ISTAT_DONE);
}

static int dwc_trng_wait_on_zeroize(struct dwc_trng_priv *priv)
{
   return dwc_trng_wait_on_(priv, DWC_TRNG_REG_ISTAT_ZEROIZE);
}

int dwc_trng_init(struct dwc_trng_priv *priv, uint32_t *base, uint32_t int_vector, unsigned max_reads)
{
   uint32_t tmp;
   
   memset(priv, 0, sizeof(struct dwc_trng_priv));
   
   priv->base             = base;
   priv->int_vector		  = int_vector;
   priv->status.max_reads = max_reads;

   //misc_trng_sel(1);

   /* disable each interrupt bit and clear the interrupt status */
   dwc_trng_enable_interrupt(priv, 0x0);   
   dwc_trng_clear_int_status(priv, 0xffffffff);
   
   /* read features */
   tmp = dwc_trng_readl((uint32_t)priv->base + DWC_TRNG_REG_FEATURES);
   priv->config.features.aes_256           = DWC_TRNG_REG_FEATURES_AES_256(tmp);
   priv->config.features.extra_ps_present  = DWC_TRNG_REG_FEATURES_EXTRA_PS_PRESENT(tmp);
   priv->config.features.diag_level_trng3  = DWC_TRNG_REG_FEATURES_DIAG_LEVEL_CLP800(tmp);
   priv->config.features.diag_level_st_hlt = DWC_TRNG_REG_FEATURES_DIAG_LEVEL_ST_HLT(tmp);
   priv->config.features.diag_level_ns     = DWC_TRNG_REG_FEATURES_DIAG_LEVEL_NS(tmp);
   priv->config.features.secure_rst_state  = DWC_TRNG_REG_FEATURES_SECURE_RST_STATE(tmp);
   
   /* read build ID */
   tmp = dwc_trng_readl((uint32_t)priv->base + DWC_TRNG_REG_COREKIT_REL);
   priv->config.build_id.ext_num = DWC_TRNG_REG_COREKIT_REL_EXT_NUM(tmp);
   priv->config.build_id.ext_ver = DWC_TRNG_REG_COREKIT_REL_EXT_VER(tmp);
   priv->config.build_id.rel_num = DWC_TRNG_REG_COREKIT_REL_NUM(tmp);

   /* zero regs */
   dwc_trng_writel((uint32_t)priv->base + DWC_TRNG_REG_ALARMS, dwc_trng_readl((uint32_t)priv->base + DWC_TRNG_REG_ALARMS));
   dwc_trng_writel((uint32_t)priv->base + DWC_TRNG_REG_ISTAT, dwc_trng_readl((uint32_t)priv->base + DWC_TRNG_REG_ISTAT));
   
   // display status
   DWC_TRNG_PRINT("DWC_TRNG: (rel_num.%04x ext_num.%01x, ext_ver.%02x)\n", priv->config.build_id.rel_num, priv->config.build_id.ext_num, \
      , priv->config.build_id.ext_ver);
   DWC_TRNG_PRINT("DWC_TRNG: AES-256=%u, PS=%u\n", priv->config.features.aes_256, priv->config.features.extra_ps_present);
   
   // default mode
   tmp = 0;
   tmp = DWC_TRNG_REG_SMODE_SET_MAX_REJECTS(tmp, 10);
   tmp = DWC_TRNG_REG_SMODE_SET_SECURE_EN(tmp, 1);
   dwc_trng_writel((uint32_t)priv->base + DWC_TRNG_REG_SMODE, tmp);
   dwc_trng_writel((uint32_t)priv->base + DWC_TRNG_REG_MODE, 0);     
   
   /* init lock */
   DWC_TRNG_INIT_LOCK(&priv->lock);
   
   // switch to zeroize mode
   return dwc_trng_cmd_zeroize(priv, 1);
}

int dwc_trng_set_keylen(struct dwc_trng_priv *priv, int aes256, int lock)
{
   unsigned long flags;
   uint32_t tmp;
   int err;

   if (lock) { DWC_TRNG_LOCK(&priv->lock, flags); }

   if (!aes256 || (aes256 && priv->config.features.aes_256)) {
      /* enable AES256 mode */
      tmp = dwc_trng_readl((uint32_t)priv->base + DWC_TRNG_REG_MODE);
      tmp = DWC_TRNG_REG_MODE_SET_SEC_ALG(tmp, aes256);
      dwc_trng_writel((uint32_t)priv->base + DWC_TRNG_REG_MODE, tmp);
      priv->status.keylen = aes256;
      err = 0;
   } else {
      err = CRYPTO_FAILED;
   }

   if (lock) { DWC_TRNG_UNLOCK(&priv->lock, flags); }
   
   return err;
}

int dwc_trng_set_addin_present(struct dwc_trng_priv *priv, int addin, int lock)
{
   unsigned long flags;
   uint32_t tmp;

   if (lock) { DWC_TRNG_LOCK(&priv->lock, flags); }

   /* enable NONCE mode */
   tmp = dwc_trng_readl((uint32_t)priv->base + DWC_TRNG_REG_MODE);
   tmp = DWC_TRNG_REG_MODE_SET_ADDIN_PRESENT(tmp, addin);
   dwc_trng_writel((uint32_t)priv->base + DWC_TRNG_REG_MODE, tmp);

   if (lock) { DWC_TRNG_UNLOCK(&priv->lock, flags); }
   
   return 0;
}

int dwc_trng_set_pred_resist(struct dwc_trng_priv *priv, int pred_resist, int lock)
{
   unsigned long flags;
   uint32_t tmp;

   if (lock) { DWC_TRNG_LOCK(&priv->lock, flags); }

   /* enable NONCE mode */
   tmp = dwc_trng_readl((uint32_t)priv->base + DWC_TRNG_REG_MODE);
   tmp = DWC_TRNG_REG_MODE_SET_PRED_RESIST(tmp, pred_resist);
   dwc_trng_writel((uint32_t)priv->base + DWC_TRNG_REG_MODE, tmp);

   /* need sync the status of priv */
   priv->config.features.extra_ps_present  = DWC_TRNG_REG_FEATURES_EXTRA_PS_PRESENT(tmp);
   
   if (lock) { DWC_TRNG_UNLOCK(&priv->lock, flags); }
   
   return 0;
}

int dwc_trng_set_kat(struct dwc_trng_priv *priv, int kat_sel, int kat_vec, int lock)
{
	unsigned long flags;
	uint32_t tmp;
	
	if (lock) { DWC_TRNG_LOCK(&priv->lock, flags); }
	
	/* enable NONCE mode */
	tmp = dwc_trng_readl((uint32_t)priv->base + DWC_TRNG_REG_MODE);
	tmp = DWC_TRNG_REG_MODE_SET_KAT_SEL(tmp, kat_sel);
	tmp = DWC_TRNG_REG_MODE_SET_KAT_VEC(tmp, kat_vec);

	dwc_trng_writel((uint32_t)priv->base + DWC_TRNG_REG_MODE, tmp);
	
	if (lock) { DWC_TRNG_UNLOCK(&priv->lock, flags); }

   
   return 0;
}

int dwc_trng_set_nonce(struct dwc_trng_priv *priv, int nonce, int lock)
{
   unsigned long flags;
   uint32_t tmp;

   if (lock) { DWC_TRNG_LOCK(&priv->lock, flags); }

   /* enable NONCE mode */
   tmp = dwc_trng_readl((uint32_t)priv->base + DWC_TRNG_REG_SMODE);
   tmp = DWC_TRNG_REG_SMODE_SET_NONCE(tmp, nonce);
   dwc_trng_writel((uint32_t)priv->base + DWC_TRNG_REG_SMODE, tmp);

   if (lock) { DWC_TRNG_UNLOCK(&priv->lock, flags); }
   
   return 0;
}

int dwc_trng_set_secure(struct dwc_trng_priv *priv, int secure, int lock)
{
   unsigned long flags;
   uint32_t tmp;

   if (lock) { DWC_TRNG_LOCK(&priv->lock, flags); }

   /* enable NONCE mode */
   tmp = dwc_trng_readl((uint32_t)priv->base + DWC_TRNG_REG_SMODE);
   tmp = DWC_TRNG_REG_SMODE_SET_SECURE_EN(tmp, secure);
   dwc_trng_writel((uint32_t)priv->base + DWC_TRNG_REG_SMODE, tmp);

   if (lock) { DWC_TRNG_UNLOCK(&priv->lock, flags); }
   
   return 0;
}

int dwc_trng_cmd_zeroize(struct dwc_trng_priv *priv, int lock)
{
   unsigned long flags;
   int err;
   
   DWC_TRNG_DEBUG("cmd: zeroize\n");
   
   err = CRYPTO_FAILED;
   
   if (lock) { DWC_TRNG_LOCK(&priv->lock, flags); }
   
   if (dwc_trng_get_alarms(priv)) { err = CRYPTO_FATAL; goto ERR; }

   // wait for core to not be busy
   if (dwc_trng_wait_on_busy(priv)) { goto ERR; }

   // issue zeroize command
   dwc_trng_writel((uint32_t)priv->base + DWC_TRNG_REG_CTRL, DWC_TRNG_REG_CTRL_CMD_ZEROIZE);

   // wait for done
   if (dwc_trng_wait_on_zeroize(priv)) { goto ERR; }

   err = 0;
   priv->status.current_state = DWC_TRNG_STATE_ZEROIZE;
ERR:
   if (lock) { DWC_TRNG_UNLOCK(&priv->lock, flags); }
   return err;
}

int dwc_trng_cmd_seed(struct dwc_trng_priv *priv, uint32_t *seed, int lock) // seed=NULL means to perform a noise reseed
{
   unsigned long flags;
   int err;
   
   err = CRYPTO_FAILED;
   
   DWC_TRNG_DEBUG("cmd: seed\n");

   if (lock) { DWC_TRNG_LOCK(&priv->lock, flags); }
   
   // valid state?
   if (priv->status.current_state != DWC_TRNG_STATE_ZEROIZE && priv->status.current_state != DWC_TRNG_STATE_ADVANCE_STATE) {
      DWC_TRNG_DEBUG("cmd_seed: Invalid state\n");
      err = CRYPTO_INVALID_SEQUENCE;
      goto ERR;
   }
   
   if (dwc_trng_get_alarms(priv)) { err = CRYPTO_FATAL; goto ERR; }
   if (dwc_trng_wait_on_busy(priv)) { goto ERR; }
   
   if (seed == NULL) {
      // noise reseed
      dwc_trng_set_nonce(priv, 0, 0);
      dwc_trng_writel((uint32_t)priv->base + DWC_TRNG_REG_CTRL, DWC_TRNG_REG_CTRL_CMD_GEN_NOISE);
      // wait on done
      if (dwc_trng_wait_on_done(priv)) { goto ERR; }
   } else {
      uint32_t x;
      
      // nonce reseed
      dwc_trng_set_nonce(priv, 1, 0);
      // write seed
      for (x = 0; x < 12; x++) {
         dwc_trng_writel((uint32_t)priv->base + DWC_TRNG_REG_SEED0 + (x<<2), seed[x]);
      }
   }
   
   err = 0;
   priv->status.current_state = DWC_TRNG_STATE_SEEDING;
   priv->status.reads_left    = priv->status.max_reads;
ERR:
   if (lock) { DWC_TRNG_UNLOCK(&priv->lock, flags); }
   return err;
}


int dwc_trng_cmd_create_state(struct dwc_trng_priv *priv, uint32_t *ps, int lock)
{
   unsigned long flags;
   uint32_t zero_ps[12] = { 0 };
   int err;
   
   err = CRYPTO_FAILED;

   DWC_TRNG_DEBUG("cmd: create_state\n");
   
   if (lock) { DWC_TRNG_LOCK(&priv->lock, flags); }
   
   // valid state?
   if (priv->status.current_state != DWC_TRNG_STATE_SEEDING) {
      DWC_TRNG_DEBUG("cmd_create_state: Invalid state\n");
      err = CRYPTO_INVALID_SEQUENCE;
      goto ERR;
   }
   
   if (dwc_trng_get_alarms(priv)) { err = CRYPTO_FATAL; goto ERR; }
   if (dwc_trng_wait_on_busy(priv)) { goto ERR; }
   
   // write PS if necessary   
   if (priv->config.features.extra_ps_present) {
      uint32_t x, y;
      if (!ps) {
         ps = &zero_ps[0];
      }
      y = priv->status.keylen ? 12 : 8;
      for (x = 0; x < y; x++) {
         dwc_trng_writel((uint32_t)priv->base + DWC_TRNG_REG_NPA_DATA0 + (x<<2), ps[x]);
      }
   }
   
   dwc_trng_writel((uint32_t)priv->base + DWC_TRNG_REG_CTRL, DWC_TRNG_REG_CTRL_CMD_CREATE_STATE);
   // wait on done
   if (dwc_trng_wait_on_done(priv)) { goto ERR; }
   
   err = 0;
   priv->status.current_state = DWC_TRNG_STATE_CREATE_STATE;
ERR:
   if (lock) { DWC_TRNG_UNLOCK(&priv->lock, flags); }
   return err;
}

int dwc_trng_cmd_renew_state(struct dwc_trng_priv *priv, int lock)
{
   unsigned long flags;
   int err;
   
   err = CRYPTO_FAILED;
   
   DWC_TRNG_DEBUG("cmd: renew_state\n");
   
   if (lock) { DWC_TRNG_LOCK(&priv->lock, flags); }
   
   // valid state?
   if (priv->status.current_state != DWC_TRNG_STATE_SEEDING) {
      DWC_TRNG_DEBUG("cmd_renew_state: Invalid state\n");
      err = CRYPTO_INVALID_SEQUENCE;
      goto ERR;
   }
   
   if (dwc_trng_get_alarms(priv)) { err = CRYPTO_FATAL; goto ERR; }
   if (dwc_trng_wait_on_busy(priv)) { goto ERR; }
   dwc_trng_writel((uint32_t)priv->base + DWC_TRNG_REG_CTRL, DWC_TRNG_REG_CTRL_CMD_RENEW_STATE);
   if (dwc_trng_wait_on_done(priv)) { goto ERR; }
   
   err = 0;
   priv->status.current_state = DWC_TRNG_STATE_RENEW_STATE;
ERR:
   if (lock) { DWC_TRNG_UNLOCK(&priv->lock, flags); }
   return err;
}

int dwc_trng_cmd_gen_random(struct dwc_trng_priv *priv, unsigned num_reads, uint32_t *rand, int lock)
{
   unsigned long flags;
   int err;
   uint32_t x=0, y=0;
   
   err = CRYPTO_FAILED;
   
   DWC_TRNG_DEBUG("cmd: gen_Random\n");

   if (lock) { DWC_TRNG_LOCK(&priv->lock, flags); }
   
   // valid state?
   if ((priv->status.current_state != DWC_TRNG_STATE_RENEW_STATE) && (priv->status.current_state != DWC_TRNG_STATE_CREATE_STATE) &&
	   (priv->status.current_state != DWC_TRNG_STATE_GEN_RANDOM) && (priv->status.current_state != DWC_TRNG_STATE_ADVANCE_STATE) &&
	   (priv->status.current_state != DWC_TRNG_STATE_REFRESH_ADDIN)) {
      DWC_TRNG_DEBUG("cmd_gen_random: Invalid state\n");
      err = CRYPTO_INVALID_SEQUENCE;
      goto ERR;
   }
   
   if (dwc_trng_get_alarms(priv)) { err = CRYPTO_FATAL; goto ERR; }
   if (dwc_trng_wait_on_busy(priv)) { goto ERR; }

   priv->status.current_state = DWC_TRNG_STATE_GEN_RANDOM;
   for (x = 0; x < num_reads; x++) {
      // issue gen_random 
      dwc_trng_writel((uint32_t)priv->base + DWC_TRNG_REG_CTRL, DWC_TRNG_REG_CTRL_CMD_GEN_RANDOM);
      if (dwc_trng_wait_on_done(priv)) { goto ERR; }
	  rand[y++] = dwc_trng_readl((uint32_t)priv->base + DWC_TRNG_REG_RAND0);
	  rand[y++] = dwc_trng_readl((uint32_t)priv->base + DWC_TRNG_REG_RAND1);
	  rand[y++] = dwc_trng_readl((uint32_t)priv->base + DWC_TRNG_REG_RAND2);
	  rand[y++] = dwc_trng_readl((uint32_t)priv->base + DWC_TRNG_REG_RAND3);
	  
      if (priv->status.max_reads && --(priv->status.reads_left) == 0) {
         // reseed the device based on the noise source
         if ((err = dwc_trng_cmd_advance_state(priv, 0))) { goto ERR; }
         if ((err = dwc_trng_cmd_seed(priv, NULL, 0)))    { goto ERR; }
         if ((err = dwc_trng_cmd_renew_state(priv, 0)))   { goto ERR; }
         // reset us back into GEN_RANDOM state
         priv->status.current_state = DWC_TRNG_STATE_GEN_RANDOM;
         priv->status.reads_left    = priv->status.max_reads;
      }
   }
   err = 0;
ERR:
   if (lock) { DWC_TRNG_UNLOCK(&priv->lock, flags); }
   return err;   
}
   
int dwc_trng_cmd_advance_state(struct dwc_trng_priv *priv, int lock)
{
   unsigned long flags;
   int err;
   
   err = CRYPTO_FAILED;
   DWC_TRNG_DEBUG("cmd: advance_state\n");
   
   if (lock) { DWC_TRNG_LOCK(&priv->lock, flags); }
   
   // valid state?
   if (priv->status.current_state != DWC_TRNG_STATE_GEN_RANDOM) {
      DWC_TRNG_DEBUG("cmd_advance_state: Invalid state\n");
      err = CRYPTO_INVALID_SEQUENCE;
      goto ERR;
   }
   
   if (dwc_trng_get_alarms(priv)) { err = CRYPTO_FATAL; goto ERR; }
   if (dwc_trng_wait_on_busy(priv)) { goto ERR; }
   dwc_trng_writel((uint32_t)priv->base + DWC_TRNG_REG_CTRL, DWC_TRNG_REG_CTRL_CMD_ADVANCE_STATE);
   if (dwc_trng_wait_on_done(priv)) { goto ERR; }
   
   err = 0;
   priv->status.current_state = DWC_TRNG_STATE_ADVANCE_STATE;
ERR:
   if (lock) { DWC_TRNG_UNLOCK(&priv->lock, flags); }
   return err;
}

int dwc_trng_cmd_kat(struct dwc_trng_priv *priv, int kat_sel, int kat_vec, uint32_t *alarms, int lock)
{
   unsigned long flags = 0;
   uint32_t tmp = 0;   
   int err = CRYPTO_FAILED;
   
   DWC_TRNG_DEBUG("cmd: kat\n");
   
   if (lock) { DWC_TRNG_LOCK(&priv->lock, flags); }
   
   if (dwc_trng_get_alarms(priv)) { err = CRYPTO_FATAL; goto ERR; }

   // wait for core to not be busy
   if (dwc_trng_wait_on_busy(priv)) { goto ERR; }

   /* enable NONCE mode */
   tmp = dwc_trng_readl((uint32_t)priv->base + DWC_TRNG_REG_MODE);
   tmp = DWC_TRNG_REG_MODE_SET_KAT_SEL(tmp, kat_sel);
   tmp = DWC_TRNG_REG_MODE_SET_KAT_VEC(tmp, kat_sel);
   dwc_trng_writel((uint32_t)priv->base + DWC_TRNG_REG_MODE, tmp);

   // issue kat command
   dwc_trng_writel((uint32_t)priv->base + DWC_TRNG_REG_CTRL, DWC_TRNG_REG_CTRL_CMD_KAT);

   // wait for done
   if (dwc_trng_wait_on_(priv, DWC_TRNG_REG_ISTAT_KAT_COMPLETE)) { goto ERR; }

   //alarms = dwc_trng_get_alarms(priv);
   dwc_trng_get_alarms(priv);
   priv->status.current_state = DWC_TRNG_STATE_KAT;
ERR:
   if (lock) { DWC_TRNG_UNLOCK(&priv->lock, flags); }
   return err;
}

void dwc_trng_enable_interrupt(struct dwc_trng_priv *priv, uint32_t en)
{
	dwc_trng_writel((uint32_t)priv->base + DWC_TRNG_REG_IE, DWC_TRNG_REG_IE_GLBL | en);
}

uint32_t dwc_trng_get_interrupt_status(struct dwc_trng_priv *priv)
{
	dwc_trng_readl((uint32_t)priv->base + DWC_TRNG_REG_ISTAT)& 0x1f;
}

void dwc_trng_clear_int_status(struct dwc_trng_priv *priv, uint32_t clr)
{
	dwc_trng_writel((uint32_t)priv->base + DWC_TRNG_REG_ISTAT, clr);
}
