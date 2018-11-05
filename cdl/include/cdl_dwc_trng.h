#ifndef __CDL_DWC_TRNG_H__
#define __CDL_DWC_TRNG_H__

// Macro to print messages to user
#define DWC_TRNG_DEBUG(x, ...)
#define DWC_TRNG_PRINT(x, ...)


#define DWC_TRNG_REG_CTRL                       0x00
#define DWC_TRNG_REG_MODE                       0x04
#define DWC_TRNG_REG_SMODE                      0x08
#define DWC_TRNG_REG_STAT                       0x0c
#define DWC_TRNG_REG_IE                      	 0x10
#define DWC_TRNG_REG_ISTAT                      0x14
#define DWC_TRNG_REG_ALARMS                     0x18
#define DWC_TRNG_REG_COREKIT_REL                0x1c
#define DWC_TRNG_REG_FEATURES                   0x20
#define DWC_TRNG_REG_RAND0                      0x24
#define DWC_TRNG_REG_RAND1                      0x28
#define DWC_TRNG_REG_RAND2                      0x2c
#define DWC_TRNG_REG_RAND3                      0x30
#define DWC_TRNG_REG_NPA_DATA0                  0x34
#define DWC_TRNG_REG_NPA_DATA1                  0x38
#define DWC_TRNG_REG_NPA_DATA2                  0x3c
#define DWC_TRNG_REG_NPA_DATA3                  0x40
#define DWC_TRNG_REG_NPA_DATA4                  0x44
#define DWC_TRNG_REG_NPA_DATA5                  0x48
#define DWC_TRNG_REG_NPA_DATA6                  0x4c
#define DWC_TRNG_REG_NPA_DATA7                  0x50
#define DWC_TRNG_REG_NPA_DATA8                  0x54
#define DWC_TRNG_REG_NPA_DATA9                  0x58
#define DWC_TRNG_REG_NPA_DATA10                 0x5c
#define DWC_TRNG_REG_NPA_DATA11                 0x60
#define DWC_TRNG_REG_NPA_DATA12                 0x64
#define DWC_TRNG_REG_NPA_DATA13                 0x68
#define DWC_TRNG_REG_NPA_DATA14                 0x6c
#define DWC_TRNG_REG_NPA_DATA15                 0x70
#define DWC_TRNG_REG_SEED0                      0x74
#define DWC_TRNG_REG_SEED1                      0x78
#define DWC_TRNG_REG_SEED2                      0x7c
#define DWC_TRNG_REG_SEED3                      0x80
#define DWC_TRNG_REG_SEED4                      0x84
#define DWC_TRNG_REG_SEED5                      0x88
#define DWC_TRNG_REG_SEED6                      0x8c
#define DWC_TRNG_REG_SEED7                      0x90
#define DWC_TRNG_REG_SEED8                      0x94
#define DWC_TRNG_REG_SEED9                      0x98
#define DWC_TRNG_REG_SEED10                     0x9c
#define DWC_TRNG_REG_SEED11                     0xa0
#define DWC_TRNG_REG_IA_RDATA                   0xa4
#define DWC_TRNG_REG_IA_WDATA                   0xa8
#define DWC_TRNG_REG_IA_ADDR                    0xac
#define DWC_TRNG_REG_IA_CMD                     0xb0

/* CTRL*/
#define DWC_TRNG_REG_CTRL_CMD_NOP              0
#define DWC_TRNG_REG_CTRL_CMD_GEN_NOISE        1
#define DWC_TRNG_REG_CTRL_CMD_GEN_NONCE        2
#define DWC_TRNG_REG_CTRL_CMD_CREATE_STATE        3
#define DWC_TRNG_REG_CTRL_CMD_RENEW_STATE         4
#define DWC_TRNG_REG_CTRL_CMD_REFRESH_ADDIN    5
#define DWC_TRNG_REG_CTRL_CMD_GEN_RANDOM       6
#define DWC_TRNG_REG_CTRL_CMD_ADVANCE_STATE    7
#define DWC_TRNG_REG_CTRL_CMD_KAT              8
#define DWC_TRNG_REG_CTRL_CMD_ZEROIZE          15

/* MODE */
#define _DWC_TRNG_REG_MODE_SEC_ALG             0
#define _DWC_TRNG_REG_MODE_KAT_VEC             1
#define _DWC_TRNG_REG_MODE_KAT_SEL             2
#define _DWC_TRNG_REG_MODE_PRED_RESIST         3
#define _DWC_TRNG_REG_MODE_ADDIN_PRESENT       4

#define DWC_TRNG_REG_MODE_SEC_ALG              (1UL<<_DWC_TRNG_REG_MODE_ADDIN_PRESENT)
#define DWC_TRNG_REG_MODE_KAT_VEC              (1UL<<_DWC_TRNG_REG_MODE_KAT_VEC)
#define DWC_TRNG_REG_MODE_KAT_SEL              (1UL<<_DWC_TRNG_REG_MODE_KAT_SEL)
#define DWC_TRNG_REG_MODE_PRED_RESIT           (1UL<<_DWC_TRNG_REG_MODE_PRED_RESIST)
#define DWC_TRNG_REG_MODE_ADDIN_PRESENT        (1UL<<_DWC_TRNG_REG_MODE_ADDIN_PRESENT)

/* SMODE */
#define _DWC_TRNG_REG_SMODE_NONCE              0
#define _DWC_TRNG_REG_SMODE_SECURE_EN          1
#define _DWC_TRNG_REG_SMODE_MAX_REJECTS        2

#define DWC_TRNG_REG_SMODE_NONCE               (1UL<<DWC_TRNG_REG_SMODE_NONCE)
#define DWC_TRNG_REG_SMODE_SECURE_EN(x)        ((x)<<DWC_TRNG_REG_SMODE_SECURE_EN)
#define DWC_TRNG_REG_SMODE_MAX_REJECTS(x)         ((x)<<DWC_TRNG_REG_SMODE_MAX_REJECTS)

/* STAT */
#define _DWC_TRNG_REG_STAT_LAST_CMD            0
#define _DWC_TRNG_REG_STAT_SEC_ALG             4
#define _DWC_TRNG_REG_STAT_NONCE_MODE          5
#define _DWC_TRNG_REG_STAT_SECURE              6
#define _DWC_TRNG_REG_STAT_DRBG_STATE          7
#define _DWC_TRNG_REG_STAT_BUSY             31

#define DWC_TRNG_REG_STAT_LAST_CMD(x)          (((x)>>_DWC_TRNG_REG_STAT_LAST_CMD)&0xF)
#define DWC_TRNG_REG_STAT_SEC_ALG              (1UL<<_DWC_TRNG_REG_STAT_SEC_ALG)
#define DWC_TRNG_REG_STAT_NONCE_MODE           (1UL<<_DWC_TRNG_REG_STAT_NONCE_MODE)
#define DWC_TRNG_REG_STAT_SECURE               (1UL<<_DWC_TRNG_REG_STAT_SECURE)
#define DWC_TRNG_REG_STAT_DRBG_STATE           (1UL<<_DWC_TRNG_REG_STAT_DRBG_STATE)
#define DWC_TRNG_REG_STAT_BUSY                 (1UL<<_DWC_TRNG_REG_STAT_BUSY)

/* IE */
#define _DWC_TRNG_REG_IE_GLBL         31
#define _DWC_TRNG_REG_IE_DONE          4
#define _DWC_TRNG_REG_IE_ALARMS        3
#define _DWC_TRNG_REG_IE_NOISE_RDY     2
#define _DWC_TRNG_REG_IE_KAT_COMPLETE  1
#define _DWC_TRNG_REG_IE_ZEROIZE       0

#define DWC_TRNG_REG_IE_GLBL          (1UL<<_DWC_TRNG_REG_IE_GLBL)
#define DWC_TRNG_REG_IE_DONE          (1UL<<_DWC_TRNG_REG_IE_DONE)
#define DWC_TRNG_REG_IE_ALARMS        (1UL<<_DWC_TRNG_REG_IE_ALARMS)
#define DWC_TRNG_REG_IE_NOISE_RDY     (1UL<<_DWC_TRNG_REG_IE_NOISE_RDY)
#define DWC_TRNG_REG_IE_KAT_COMPLETE  (1UL<<_DWC_TRNG_REG_IE_KAT_COMPLETE)
#define DWC_TRNG_REG_IE_ZEROIZE       (1UL<<_DWC_TRNG_REG_IE_ZEROIZE)


/* ISTAT */
#define _DWC_TRNG_REG_ISTAT_DONE                  4
#define _DWC_TRNG_REG_ISTAT_ALARMS                3
#define _DWC_TRNG_REG_ISTAT_NOISE_RDY             2
#define _DWC_TRNG_REG_ISTAT_KAT_COMPLETE          1
#define _DWC_TRNG_REG_ISTAT_ZEROIZE               0

#define DWC_TRNG_REG_ISTAT_DONE                   (1UL<<_DWC_TRNG_REG_ISTAT_DONE)
#define DWC_TRNG_REG_ISTAT_ALARMS              (1UL<<_DWC_TRNG_REG_ISTAT_ALARMS)
#define DWC_TRNG_REG_ISTAT_NOISE_RDY              (1UL<<_DWC_TRNG_REG_ISTAT_NOISE_RDY)
#define DWC_TRNG_REG_ISTAT_KAT_COMPLETE           (1UL<<_DWC_TRNG_REG_ISTAT_KAT_COMPLETE)
#define DWC_TRNG_REG_ISTAT_ZEROIZE                (1UL<<_DWC_TRNG_REG_ISTAT_ZEROIZE)

/* ALARMS */
#define DWC_TRNG_REG_ALARM_ILLEGAL_CMD_SEQ                      (1UL<<4)
#define DWC_TRNG_REG_ALARM_FAILED_TEST_ID_OK                    0
#define DWC_TRNG_REG_ALARM_FAILED_TEST_ID_KAT_STAT              1
#define DWC_TRNG_REG_ALARM_FAILED_TEST_ID_KAT                   2
#define DWC_TRNG_REG_ALARM_FAILED_TEST_ID_MONOBIT               3
#define DWC_TRNG_REG_ALARM_FAILED_TEST_ID_RUN                   4
#define DWC_TRNG_REG_ALARM_FAILED_TEST_ID_LONGRUN               5
#define DWC_TRNG_REG_ALARM_FAILED_TEST_ID_AUTOCORRELATION       6
#define DWC_TRNG_REG_ALARM_FAILED_TEST_ID_POKER                 7
#define DWC_TRNG_REG_ALARM_FAILED_TEST_ID_REPETITION_COUNT      8
#define DWC_TRNG_REG_ALARM_FAILED_TEST_ID_ADAPATIVE_PROPORTION  9

/* BUILD_ID */
#define DWC_TRNG_REG_COREKIT_REL_EXT_NUM(x)                (((x)>>28)&0xF)
#define DWC_TRNG_REG_COREKIT_REL_EXT_VER(x)             (((x)>>16)&0xFF)
#define DWC_TRNG_REG_COREKIT_REL_NUM(x)                    ((x)&0xFFFF)

/* FEATURES */
#define DWC_TRNG_REG_FEATURES_AES_256(x)                   (((x)>>9)&1)
#define DWC_TRNG_REG_FEATURES_EXTRA_PS_PRESENT(x)          (((x)>>8)&1)
#define DWC_TRNG_REG_FEATURES_DIAG_LEVEL_NS(x)             (((x)>>7)&1)
#define DWC_TRNG_REG_FEATURES_DIAG_LEVEL_CLP800(x)         (((x)>>4)&7)
#define DWC_TRNG_REG_FEATURES_DIAG_LEVEL_ST_HLT(x)         (((x)>>1)&7)
#define DWC_TRNG_REG_FEATURES_SECURE_RST_STATE(x)          ((x)&1)

/* IA_CMD */
#define DWC_TRNG_REG_IA_CMD_GO (1UL<<31)
#define DWC_TRNG_REG_IA_CMD_WR (1)


#define _DWC_TRNG_REG_SMODE_MAX_REJECTS_MASK      255UL
#define _DWC_TRNG_REG_SMODE_SECURE_EN_MASK        1UL
#define _DWC_TRNG_REG_SMODE_NONCE_MASK            1UL
#define _DWC_TRNG_REG_MODE_SEC_ALG_MASK           1UL
#define _DWC_TRNG_REG_MODE_ADDIN_PRESENT_MASK     1UL
#define _DWC_TRNG_REG_MODE_PRED_RESIST_MASK       1UL
#define _DWC_TRNG_REG_MODE_KAT_SEL_MASK     1UL
#define _DWC_TRNG_REG_MODE_KAT_VEC_MASK       1UL


#define DWC_TRNG_REG_SMODE_SET_MAX_REJECTS(y, x)  (((y) & ~(_DWC_TRNG_REG_SMODE_MAX_REJECTS_MASK << _DWC_TRNG_REG_SMODE_MAX_REJECTS)) | ((x) << _DWC_TRNG_REG_SMODE_MAX_REJECTS))
#define DWC_TRNG_REG_SMODE_SET_SECURE_EN(y, x)    (((y) & ~(_DWC_TRNG_REG_SMODE_SECURE_EN_MASK   << _DWC_TRNG_REG_SMODE_SECURE_EN))   | ((x) << _DWC_TRNG_REG_SMODE_SECURE_EN))
#define DWC_TRNG_REG_SMODE_SET_NONCE(y, x)        (((y) & ~(_DWC_TRNG_REG_SMODE_NONCE_MASK       << _DWC_TRNG_REG_SMODE_NONCE))       | ((x) << _DWC_TRNG_REG_SMODE_NONCE))
#define DWC_TRNG_REG_SMODE_GET_MAX_REJECTS(x)     (((x) >> _DWC_TRNG_REG_SMODE_MAX_REJECTS) & _DWC_TRNG_REG_SMODE_MAX_REJECTS_MASK)
#define DWC_TRNG_REG_SMODE_GET_SECURE_EN(x)       (((x) >> _DWC_TRNG_REG_SMODE_SECURE_EN)   & _DWC_TRNG_REG_SMODE_SECURE_EN_MASK)
#define DWC_TRNG_REG_SMODE_GET_NONCE(x)           (((x) >> _DWC_TRNG_REG_SMODE_NONCE)       & _DWC_TRNG_REG_SMODE_NONCE_MASK)

#define DWC_TRNG_REG_MODE_SET_SEC_ALG(y, x)       (((y) & ~(_DWC_TRNG_REG_MODE_SEC_ALG_MASK       << _DWC_TRNG_REG_MODE_SEC_ALG))        | ((x) << _DWC_TRNG_REG_MODE_SEC_ALG))
#define DWC_TRNG_REG_MODE_SET_PRED_RESIST(y, x)   (((y) & ~(_DWC_TRNG_REG_MODE_PRED_RESIST_MASK   << _DWC_TRNG_REG_MODE_PRED_RESIST))    | ((x) << _DWC_TRNG_REG_MODE_PRED_RESIST))
#define DWC_TRNG_REG_MODE_SET_ADDIN_PRESENT(y, x) (((y) & ~(_DWC_TRNG_REG_MODE_ADDIN_PRESENT_MASK << _DWC_TRNG_REG_MODE_ADDIN_PRESENT))  | ((x) << _DWC_TRNG_REG_MODE_ADDIN_PRESENT))
#define DWC_TRNG_REG_MODE_SET_KAT_SEL(y, x)       (((y) & ~(_DWC_TRNG_REG_MODE_KAT_SEL_MASK << _DWC_TRNG_REG_MODE_KAT_SEL))  | ((x) << _DWC_TRNG_REG_MODE_KAT_SEL))
#define DWC_TRNG_REG_MODE_SET_KAT_VEC(y, x)       (((y) & ~(_DWC_TRNG_REG_MODE_KAT_VEC_MASK << _DWC_TRNG_REG_MODE_KAT_VEC))  | ((x) << _DWC_TRNG_REG_MODE_KAT_VEC))
#define DWC_TRNG_REG_MODE_GET_SEC_ALG(x)          (((x) >> _DWC_TRNG_REG_MODE_SEC_ALG)       & _DWC_TRNG_REG_MODE_SEC_ALG_MASK)
#define DWC_TRNG_REG_MODE_GET_PRED_RESIST(x)      (((x) >> _DWC_TRNG_REG_MODE_PRED_RESIST)   & _DWC_TRNG_REG_MODE_PRED_RESIST_MASK)
#define DWC_TRNG_REG_MODE_GET_ADDIN_PRESENT(x)    (((x) >> _DWC_TRNG_REG_MODE_ADDIN_PRESENT) & _DWC_TRNG_REG_MODE_ADDIN_PRESENT_MASK)

/*
 * Common error definitions.  Be sure to update pdu_error_code when changing
 * anything in this list.
 */
#define CRYPTO_OK                      (   0)
#define CRYPTO_FAILED                  (  -1)
#define CRYPTO_INPROGRESS              (  -2)
#define CRYPTO_INVALID_HANDLE          (  -3)
#define CRYPTO_INVALID_CONTEXT         (  -4)
#define CRYPTO_INVALID_SIZE            (  -5)
#define CRYPTO_NOT_INITIALIZED         (  -6)
#define CRYPTO_NO_MEM                  (  -7)
#define CRYPTO_INVALID_ALG             (  -8)
#define CRYPTO_INVALID_KEY_SIZE        (  -9)
#define CRYPTO_INVALID_ARGUMENT        ( -10)
#define CRYPTO_MODULE_DISABLED         ( -11)
#define CRYPTO_NOT_IMPLEMENTED         ( -12)
#define CRYPTO_INVALID_BLOCK_ALIGNMENT ( -13)
#define CRYPTO_INVALID_MODE            ( -14)
#define CRYPTO_INVALID_KEY             ( -15)
#define CRYPTO_AUTHENTICATION_FAILED   ( -16)
#define CRYPTO_INVALID_IV_SIZE         ( -17)
#define CRYPTO_MEMORY_ERROR            ( -18)
#define CRYPTO_LAST_ERROR              ( -19)
#define CRYPTO_HALTED                  ( -20)
#define CRYPTO_TIMEOUT                 ( -21)
#define CRYPTO_SRM_FAILED              ( -22)
#define CRYPTO_COMMON_ERROR_MAX        (-100)
#define CRYPTO_INVALID_ICV_KEY_SIZE    (-100)
#define CRYPTO_INVALID_PARAMETER_SIZE  (-101)
#define CRYPTO_SEQUENCE_OVERFLOW       (-102)
#define CRYPTO_DISABLED                (-103)
#define CRYPTO_INVALID_VERSION         (-104)
#define CRYPTO_FATAL                   (-105)
#define CRYPTO_INVALID_PAD             (-106)
#define CRYPTO_FIFO_FULL               (-107)
#define CRYPTO_INVALID_SEQUENCE        (-108)
#define CRYPTO_INVALID_FIRMWARE        (-109)
#define CRYPTO_NOT_FOUND               (-110)
#define CRYPTO_CMD_FIFO_INACTIVE       (-111)

// macro to yield CPU (when in a busy loop)
// this is a placeholder which does not actually yield, user must update
// this macro
#define CPU_YIELD() 1

// DMAable address type, usually can be some equivalent of uint32_t but in Linux it must be dma_addr_t
// since on 64-bit and PAE boxes the pointers used by coherant allocation are 64-bits (even though only the lower 32 bits are used)
#define DWC_TRNG_DMA_ADDR_T               uint32_t

// these are for IRQ contexts (can block IRQs)
// USER MUST SUPPLY THESE, the following definitions are an EXAMPLE only
#define DWC_TRNG_LOCK_TYPE                volatile int
#define DWC_TRNG_INIT_LOCK(lock)          *lock = 0
#define DWC_TRNG_LOCK(lock, flags)        do { while (*lock) { CPU_YIELD(); }; *lock = 1; } while (0)
#define DWC_TRNG_UNLOCK(lock, flags)      *lock = 0

// these are for bottom half BH contexts (cannot block IRQs)
// USER MUST SUPPLY THESE, the following definitions are an EXAMPLE only
#define DWC_TRNG_LOCK_TYPE_BH             volatile int
#define DWC_TRNG_INIT_LOCK_BH(lock)       *lock = 0
#define DWC_TRNG_LOCK_BH(lock)            do { while (*lock) { CPU_YIELD(); }; *lock = 1; } while (0)
#define DWC_TRNG_UNLOCK_BH(lock)          *lock = 0

#define DWC_TRNG_RETRY_MAX 5000000UL

struct dwc_trng_priv{
   uint32_t *base;   
   uint32_t int_vector;
   uint32_t seed_mode;   
   struct {
      struct { 
         unsigned aes_256,
                  extra_ps_present,
                  diag_level_trng3,
                  diag_level_st_hlt,
                  diag_level_ns,
                  secure_rst_state;
      } features;

      struct {
         unsigned ext_num,
                  ext_ver,
                  rel_num;
      } build_id;
   } config;
   
   struct {
      unsigned alarm_code;
      unsigned 
         max_reads,
		 reads_left,
         seed_mode,
         keylen;
      int current_state;
   } status;
   
   DWC_TRNG_LOCK_TYPE lock;
};

enum {
   DWC_TRNG_STATE_KAT=0,
   DWC_TRNG_STATE_ZEROIZE,
   DWC_TRNG_STATE_SEEDING,
   DWC_TRNG_STATE_CREATE_STATE,
   DWC_TRNG_STATE_RENEW_STATE,
   DWC_TRNG_STATE_REFRESH_ADDIN,
   DWC_TRNG_STATE_GEN_RANDOM,
   DWC_TRNG_STATE_ADVANCE_STATE
};

#define dwc_trng_zero_status(x) memset(&(x->status), 0, sizeof (x->status))

int dwc_trng_init(struct dwc_trng_priv *priv, uint32_t *base, uint32_t int_vector, unsigned max_reads);
int dwc_trng_set_keylen(struct dwc_trng_priv *priv, int aes256, int lock);
int dwc_trng_set_secure(struct dwc_trng_priv *priv, int secure, int lock);
int dwc_trng_set_nonce(struct dwc_trng_priv *priv, int nonce, int lock);
int dwc_trng_set_addin_present(struct dwc_trng_priv *priv, int addin, int lock);
int dwc_trng_set_pred_resist(struct dwc_trng_priv *priv, int pred_resist, int lock);
int dwc_trng_set_kat(struct dwc_trng_priv *priv, int kat_sel, int kat_vec, int lock);
int dwc_trng_cmd_zeroize(struct dwc_trng_priv *priv, int lock);
int dwc_trng_cmd_seed(struct dwc_trng_priv *priv, uint32_t *seed, int lock); // seed=NULL means to perform a noise reseed
int dwc_trng_cmd_create_state(struct dwc_trng_priv *priv, uint32_t *ps, int lock);
int dwc_trng_cmd_renew_state(struct dwc_trng_priv *priv, int lock);
int dwc_trng_cmd_gen_random(struct dwc_trng_priv *priv, unsigned num_reads, uint32_t *rand, int lock);
int dwc_trng_cmd_advance_state(struct dwc_trng_priv *priv, int lock);
int dwc_trng_cmd_kat(struct dwc_trng_priv *priv, int kat_sel, int kat_vec, uint32_t *alarms, int lock);
void dwc_trng_enable_interrupt(struct dwc_trng_priv *priv, uint32_t en);
uint32_t dwc_trng_get_interrupt_status(struct dwc_trng_priv *priv);
void dwc_trng_clear_int_status(struct dwc_trng_priv *priv, uint32_t clr);
#endif
