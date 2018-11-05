/*----------------------------------------------------------------------------------------------------------*/
/*                                                                                               											 		 */
/*           Copyright (C) 2016 Brite Semiconductor Co., Ltd. All rights reserved.                									 */
/*                                                                                                													 */
/*-----------------------------------------------------------------------------------------------------------*/
#include "test_ddr.h"

#if (CONFIG_DDR_TEST == 1)

/*************************************************************************************************************/
// macro definition
/*************************************************************************************************************/
#define BYTE_TO_UINT32(x) ((x) | ((x)<<8) | ((x)<<16) | ((x)<<24))
#define UL_ONEBITS 0xFFFFFFFF
#define ONE 0x00000001L

#define PROGRESSLEN 4
#define PROGRESSOFTEN 250000

#define DDR_CHK_BOARD1 0x55555555
#define DDR_CHK_BOARD2 0xAAAAAAAA
#define UL_LEN 32

union {
    uint8_t bytes[UL_LEN/8];
    uint32_t val;
} mword8;

union {
    uint16_t u16s[UL_LEN/16];
    uint32_t val;
} mword16;

/*************************************************************************************************************/
// global variable definition
/*************************************************************************************************************/
uint32_t g_ddr_revd_addr_st, g_ddr_revd_addr_end;
uint32_t g_ddr_test_size = DDR_TEST_SIZE;

uint8_t g_progress[PROGRESSLEN] = "-\\|/";

/*************************************************************************************************************/
// function prototype
/*************************************************************************************************************/

/*************************************************************************************************************/
// function implementation
/*************************************************************************************************************/
// command process functions
static int cmd_help(int argc, char *argv[])
{
	return cmdline_help_general(&ddr_test_menu[0]);
}

// ddr initialize
static int ddr_init(int argc, char *argv[])
{
	int i;
	E_DDR_CONFIG ddr_cfg;
	E_DDR_SCRAM scram_mode;
	uint32_t ret_val;

	info("%s: ddr init test\n", argv[0]);

	// Check the validity of parameter count
	if (argc < 3) 
	{
		info("wrong cmd_format: %s ddr_cfg scram_mode\n", argv[0]);
		
		info("valid ddr_cfg:\n");
		info("0: LPDDR_CONFIG_100MHZ_CL3_BL8(TBD)\n");
		info("1: LPDDR_CONFIG_133MHZ_CL3_BL4(TBD)\n");
		info("2: LPDDR_CONFIG_133MHZ_CL3_BL8(TBD)\n");
		info("3: LPDDR_CONFIG_200MHZ_CL3_BL4(TBD)\n");
		info("4: LPDDR_CONFIG_200MHZ_CL3_BL8\n");
		info("5: LPDDR2_CONFIG_133MHZ_CL3_BL4(TBD)\n");
		info("6: LPDDR2_CONFIG_133MHZ_CL3_BL8(TBD)\n");
		info("7: LPDDR2_CONFIG_200MHZ_CL3_BL4(TBD)\n");
		info("8: LPDDR2_CONFIG_200MHZ_CL3_BL8(TBD)\n");
		info("9: LPDDR2_CONFIG_400MHZ_CL6_BL4(TBD)\n");
		info("10: LPDDR2_CONFIG_400MHZ_CL6_BL8(TBD)\n");
		info("11: LPDDR2_CONFIG_533MHZ_CL8_BL4(TBD)\n");
		info("12: LPDDR2_CONFIG_533MHZ_CL8_BL8(TBD)\n");	

		info("valid secu_mode:\n");
		info("0: DDR_SCRAM_DIS\n");
		info("1: DDR_SCRAM_EN\n");
		return 1;
	}

	// Check the validity of ddr_cfg
	ddr_cfg = get_arg_ulong(argv[1]);
	if (ddr_cfg >= LPDDR2_CONFIG_CNT)
	{
		info("Please input valid ddr_cfg:\n");
		info("0: LPDDR_CONFIG_100MHZ_CL3_BL8(TBD)\n");
		info("1: LPDDR_CONFIG_133MHZ_CL3_BL4(TBD)\n");
		info("2: LPDDR_CONFIG_133MHZ_CL3_BL8(TBD)\n");
		info("3: LPDDR_CONFIG_200MHZ_CL3_BL4(TBD)\n");
		info("4: LPDDR_CONFIG_200MHZ_CL3_BL8\n");
		info("5: LPDDR2_CONFIG_133MHZ_CL3_BL4(TBD)\n");
		info("6: LPDDR2_CONFIG_133MHZ_CL3_BL8(TBD)\n");
		info("7: LPDDR2_CONFIG_200MHZ_CL3_BL4(TBD)\n");
		info("8: LPDDR2_CONFIG_200MHZ_CL3_BL8(TBD)\n");
		info("9: LPDDR2_CONFIG_400MHZ_CL6_BL4(TBD)\n");
		info("10: LPDDR2_CONFIG_400MHZ_CL6_BL8(TBD)\n");
		info("11: LPDDR2_CONFIG_533MHZ_CL8_BL4(TBD)\n");
		info("12: LPDDR2_CONFIG_533MHZ_CL8_BL8(TBD)\n");
		return 1;
	}

	// Check the validity of security mode
	scram_mode = get_arg_ulong(argv[2]);
	if (scram_mode >= DDR_SCRAM_CNT)
	{
		info("Please input valid secu_mode:\n");
		info("0: DDR_SCRAM_DIS\n");
		info("1: DDR_SCRAM_EN\n");
		return 1;
	}

	ret_val = ddr_init_0(ddr_cfg, scram_mode);
	if (ret_val == 0x0) 
	{
    	debug("ddr init successfully!!!!!!!!!!!!!!!!!!!!!!!\n");
    } 
	else 
	{
    	debug("\nddr init fail,return code: %d!!!!!!!!!!!!!\n",ret_val);
    }

	return 0;
}

static void ddr_move(uint32_t *p_dest, uint32_t *p_src)
{
	*p_dest = *p_src;
}

static bool ddr_post_dataline(uint32_t *p_addr)
{
	uint32_t  *p_tmp_addr = p_addr;
	
	// This is 32 bit wide test patterns.  Note that they reside in ROM
	// (which presumably works) and the tests write them to RAM which may not work.
	// The "otherpattern" is written to drive the data bus to values other
	// than the test pattern.  This is for detecting floating bus lines.
	uint32_t other_pat = 0x01234567;
	uint32_t pat[] = {
		0xaaaaaaaa,
		0xcccccccc,
		0xf0f0f0f0,
		0xff00ff00,
		0xffff0000,
		0xffffffff,
		0x00000000,
		0x0000ffff,
		0x00ff00ff,
		0x0f0f0f0f,
		0x33333333,
		0x55555555
	};

	uint32_t num_pat = sizeof(pat)/sizeof(pat[0]);
	uint32_t i, temp;

	for (i = 0; i < num_pat; i++) 
	{		
		info("p_addr:0x%x, pat:0x%x\n", p_addr, pat[i]);
		
		ddr_move(p_tmp_addr++, &pat[i]);

		// Put a different pattern on the data lines: otherwise they may float long enough to read back what we wrote.
		// Prevent floating buses error 
		ddr_move(p_tmp_addr--, &other_pat);

		ddr_move(&temp, p_tmp_addr);	 

		if (temp != pat[i])
		{
			info("Memory (date line) error at %08x, wrote 0x%08x, read 0x%08x!\n", p_tmp_addr, pat[i], temp);

			return false;
		}
	}

	return true;
}

static bool ddr_post_addrline(uint32_t *p_addr, uint32_t cnt)
{
	uint32_t *p_target;
	uint32_t readback;
	uint32_t xor;

	for (xor = sizeof(uint32_t); xor > 0; xor <<= 1) 
	{
		// invert the address bit one by one
		p_target = (uint32_t *)((uint32_t)p_addr ^ xor);

		info("p_addr:0x%x, p_target:0x%x\n", p_addr, p_target);

		if (((uint32_t)p_target >= (uint32_t)p_addr) && ((uint32_t)p_target < ((uint32_t)p_addr + cnt))) 
		{
			// The target is from addr with xor, so the both address should be not equal.
			// If the two address are assigned with different value, *target != *addr
			*p_addr = ~*p_target;

			// The address bits invert is invalid, the both address will access the same memory cell
			if(*p_target == *p_addr) 
			{
				info("Memory(address line) error at %08x<->%08x, XOR value %08x !\n", p_addr, p_target, xor);
				
				return false;
			}
		}
	}

	return true;
}

static bool regions_cmp(uint32_t *p_dst, uint32_t *p_src, uint32_t cnt) 
{
    uint32_t i, j = 0;
    uint32_t *p1 = p_dst;
    uint32_t *p2 = p_src;

	info("\n%s, p_dst: 0x%x, p_src: 0x%x, cnt: 0x%x\n", __func__, p_dst, p_src, cnt);
	
    for (i = 0; i < (cnt >> 2); i++, p1++, p2++) 
	{
        if (*p1 != *p2) 
		{
            info("FAILURE: 0x%08lx != 0x%08lx at physical address: 0x%08lx\n", *p1, *p2, p1); 
			
            return false;
        }

		if (!(i % PROGRESSOFTEN))
		{
			info("%c", g_progress[j++ % PROGRESSLEN]);	
		}
    }

	info("\n");
	
    return true;
}

static bool random_value_cmp(uint32_t *p_dst, uint32_t *p_src, uint32_t cnt) 
{
	uint32_t i, j = 0;
	uint32_t *p1 = p_dst;
    uint32_t *p2 = p_src;

	srand(get_ticks());

	info("\nassign value, p_dst: 0x%x, p_src: 0x%x, cnt: 0x%x\n", p_dst, p_src, cnt);
	for (i = 0; i < (cnt >> 2); i++) 
	{
		*p1++ = *p2++ = cb_rand();

		if (!(i % PROGRESSOFTEN))
		{
			info("%c", g_progress[j++ % PROGRESSLEN]);
		}
	}

	return regions_cmp(p_dst, p_src, cnt);
}

static bool xor_cmp(uint32_t *p_dst, uint32_t *p_src, uint32_t cnt) 
{
    uint32_t i, j = 0;
	uint32_t *p1 = p_dst;
    uint32_t *p2 = p_src;
    uint32_t rand;
	
	srand(get_ticks());
	rand = cb_rand();

	info("\%s, p_dst: 0x%x, p_src: 0x%x, cnt: 0x%x\n", __func__, p_dst, p_src, cnt);
    for (i = 0; i < (cnt >> 2); i++) 
    {
    	*p1 = *p2;
		
        *p1++ ^= rand;
        *p2++ ^= rand;

		if (!(i % PROGRESSOFTEN))
		{
			info("%c", g_progress[j++ % PROGRESSLEN]);
		}
    }
	
    return regions_cmp(p_dst, p_src, cnt);
}

static bool sub_cmp(uint32_t *p_dst, uint32_t *p_src, uint32_t cnt) 
{
    uint32_t i, j = 0;
	uint32_t *p1 = p_dst;
    uint32_t *p2 = p_src;
    uint32_t rand;

	srand(get_ticks());
	rand = cb_rand();

	info("\%s, p_dst: 0x%x, p_src: 0x%x, cnt: 0x%x\n", __func__, p_dst, p_src, cnt);
    for (i = 0; i < (cnt >> 2); i++)  
	{
		*p1 = *p2;
		
        *p1++ -= rand;
        *p2++ -= rand;

		if (!(i % PROGRESSOFTEN))
		{
			info("%c", g_progress[j++ % PROGRESSLEN]);
		}		
    }
	
    return regions_cmp(p_dst, p_src, cnt);
}

static bool mul_cmp(uint32_t *p_dst, uint32_t *p_src, uint32_t cnt) 
{
    uint32_t i, j = 0;
	uint32_t *p1 = p_dst;
    uint32_t *p2 = p_src;
    uint32_t rand;

	srand(get_ticks());
	rand = cb_rand();

	info("\%s, p_dst: 0x%x, p_src: 0x%x, cnt: 0x%x\n", __func__, p_dst, p_src, cnt);
    for (i = 0; i < (cnt >> 2); i++) 
	{
		*p1 = *p2;
		
        *p1++ *= rand;
        *p2++ *= rand;

		if (!(i % PROGRESSOFTEN))
		{
			info("%c", g_progress[j++ % PROGRESSLEN]);
		}				
    }
	
    return regions_cmp(p_dst, p_src, cnt);
}

static bool div_cmp(uint32_t *p_dst, uint32_t *p_src, uint32_t cnt) 
{
    uint32_t i, j = 0;
	uint32_t *p1 = p_dst;
    uint32_t *p2 = p_src;
    uint32_t rand;

	srand(get_ticks());
	rand = cb_rand();

	info("\%s, p_dst: 0x%x, p_src: 0x%x, cnt: 0x%x\n", __func__, p_dst, p_src, cnt);
    for (i = 0; i < (cnt >> 2); i++) 
	{
		*p1 = *p2;
		
        if (!rand) 
		{
            rand++;
        }
		
        *p1++ /= rand;
        *p2++ /= rand;

		if (!(i % PROGRESSOFTEN))
		{
			info("%c", g_progress[j++ % PROGRESSLEN]);
		}
    }
	
    return regions_cmp(p_dst, p_src, cnt);
}

static bool or_cmp(uint32_t *p_dst, uint32_t *p_src, uint32_t cnt) 
{    
    uint32_t i, j = 0;
	uint32_t *p1 = p_dst;
    uint32_t *p2 = p_src;
    uint32_t rand;

	srand(get_ticks());
	rand = cb_rand();

	info("\%s, p_dst: 0x%x, p_src: 0x%x, cnt: 0x%x\n", __func__, p_dst, p_src, cnt);
    for (i = 0; i < (cnt >> 2); i++) 
	{
		*p1 = *p2;
		
        *p1++ |= rand;
        *p2++ |= rand;

		if (!(i % PROGRESSOFTEN))
		{
			info("%c", g_progress[j++ % PROGRESSLEN]);
		}
    }
	
    return regions_cmp(p_dst, p_src, cnt);
}

static bool and_cmp(uint32_t *p_dst, uint32_t *p_src, uint32_t cnt) 
{
    uint32_t i, j = 0;
	uint32_t *p1 = p_dst;
    uint32_t *p2 = p_src;
    uint32_t rand;

	srand(get_ticks());
	rand = cb_rand();

	info("\%s, p_dst: 0x%x, p_src: 0x%x, cnt: 0x%x\n", __func__, p_dst, p_src, cnt);
    for (i = 0; i < (cnt >> 2); i++) 
	{
		*p1 = *p2;
		
        *p1++ &= rand;
        *p2++ &= rand;

		if (!(i % PROGRESSOFTEN))
		{
			info("%c", g_progress[j++ % PROGRESSLEN]);
		}
    }
	
    return regions_cmp(p_dst, p_src, cnt);
}

static bool seqinc_cmp(uint32_t *p_dst, uint32_t *p_src, uint32_t cnt) 
{
    uint32_t i, j = 0;
	uint32_t *p1 = p_dst;
    uint32_t *p2 = p_src;
    uint32_t rand;

	srand(get_ticks());
	rand = cb_rand();

	info("\%s, p_dst: 0x%x, p_src: 0x%x, cnt: 0x%x\n", __func__, p_dst, p_src, cnt);
    for (i = 0; i < (cnt >> 2); i++) 
	{
        *p1++ = *p2++ = (i + rand);

		if (!(i % PROGRESSOFTEN))
		{
			info("%c", g_progress[j++ % PROGRESSLEN]);
		}		
    }
	
    return regions_cmp(p_dst, p_src, cnt);
}

static bool solidbits_cmp(uint32_t *p_dst, uint32_t *p_src, uint32_t cnt) 
{
    uint32_t i, j, k = 0;
	uint32_t *p1 = p_dst;
    uint32_t *p2 = p_src;
    uint32_t q;

    for (j = 0; j < 2; j++) 
	{
		info("\%s, p_dst: 0x%x, p_src: 0x%x, cnt: 0x%x\n", __func__, p_dst, p_src, cnt);
		info("setting 0x%x\n", j);
		
        q = ((j % 2) == 0) ? UL_ONEBITS : 0;

        p1 = p_dst;
        p2 = p_src;

        for (i = 0; i < (cnt >> 2); i++) 
		{
            *p1++ = *p2++ = ((i % 2) == 0) ? q : ~q;

			if (!(i % PROGRESSOFTEN))
			{
				info("%c", g_progress[k++ % PROGRESSLEN]);
			}
        }
		
        if (!regions_cmp(p_dst, p_src, cnt)) 
		{
            return false;
        }
    }

    return true;
}

static bool blockseq_cmp(uint32_t *p_dst, uint32_t *p_src, uint32_t cnt) 
{
    uint32_t i, j, k = 0;
	uint32_t *p1 = p_dst;
    uint32_t *p2 = p_src;

    for (j = 0; j < 256; j++) 
	{
		info("\%s, p_dst: 0x%x, p_src: 0x%x, cnt: 0x%x\n", __func__, p_dst, p_src, cnt);
		info("setting 0x%x\n", j);

		p1 = p_dst;
		p2 = p_src;

		for (i = 0; i < (cnt >> 2); i++) 
		{
		    *p1++ = *p2++ = BYTE_TO_UINT32(j);

			if (!(i % PROGRESSOFTEN))
			{
				info("%c", g_progress[k++ % PROGRESSLEN]);
			}
		}

		if (!regions_cmp(p_dst, p_src, cnt)) 
		{
		    return false;
		}
    }

    return true;
}

static bool chkboard_cmp(uint32_t *p_dst, uint32_t *p_src, uint32_t cnt)
{
    uint32_t i, j, k = 0;
	uint32_t *p1 = p_dst;
    uint32_t *p2 = p_src;
	uint32_t q;

    for (j = 0; j < 2; j++) 
	{
		info("\%s, p_dst: 0x%x, p_src: 0x%x, cnt: 0x%x\n", __func__, p_dst, p_src, cnt);
		info("setting 0x%x\n", j);
		
        q = ((j % 2) == 0) ? DDR_CHK_BOARD1 : DDR_CHK_BOARD2;
        
        p1 = p_dst;
		p2 = p_src;
		
        for (i = 0; i < (cnt >> 2); i++) 
		{
            *p1++ = *p2++ = ((i % 2) == 0) ? q : ~q;

			if (!(i % PROGRESSOFTEN))
			{
				info("%c", g_progress[k++ % PROGRESSLEN]);
			}
        }
		
        if (!regions_cmp(p_dst, p_src, cnt)) 
		{
		    return false;
		}
    }
	
    return true;
}

static bool bitspread_cmp(uint32_t *p_dst, uint32_t *p_src, uint32_t cnt)
{
    uint32_t i, j, k = 0;
	uint32_t *p1 = p_dst;
    uint32_t *p2 = p_src;
	uint32_t rand;

    for (j = 0; j < UL_LEN; j++) 
	{
		info("\%s, p_dst: 0x%x, p_src: 0x%x, cnt: 0x%x\n", __func__, p_dst, p_src, cnt);
		info("setting 0x%x\n", j);
		
        p1 = p_dst;
		p2 = p_src;
        
        for (i = 0; i < (cnt >> 2); i++) 
		{
            if (j < (UL_LEN/2)) 
			{ 
				// Walk it up.
                *p1++ = *p2++ = (i % 2 == 0) ? ((ONE << j) | (ONE << (j + 2)))
                    							: (UL_ONEBITS ^ ((ONE << j) | (ONE << (j + 2))));
            } 
			else 
			{ 
				// Walk it back down. 
                *p1++ = *p2++ = (i % 2 == 0) ? ((ONE << (UL_LEN - 1 - j)) | (ONE << (UL_LEN + 1 - j)))
                    							: (UL_ONEBITS ^ (ONE << (UL_LEN - 1 - j) | (ONE << (UL_LEN + 1 - j))));
            }

			if (!(i % PROGRESSOFTEN))
			{
				info("%c", g_progress[k++ % PROGRESSLEN]);
			}
        }
		
        if (!regions_cmp(p_dst, p_src, cnt)) 
		{
		    return false;
		}
	}

    return true;
}

static bool bitflip_cmp(uint32_t *p_dst, uint32_t *p_src, uint32_t cnt)
{
    uint32_t i, k, x = 0;
	uint32_t *p1 = p_dst;
    uint32_t *p2 = p_src;
	uint32_t q;

    for (k = 0; k < UL_LEN; k++) 
	{
		q = ONE << k;
		
		info("\%s, p_dst: 0x%x, p_src: 0x%x, cnt: 0x%x\n", __func__, p_dst, p_src, cnt);
		info("setting 0x%x\n", k);

		q = ~q;

		p1 = p_dst;
		p2 = p_src;
		
		for (i = 0; i < (cnt >> 2); i++) 
		{
			*p1++ = *p2++ = (i % 2) == 0 ? q : ~q;

			if (!(i % PROGRESSOFTEN))
			{
				info("%c", g_progress[x++ % PROGRESSLEN]);
			}
		}

		if (!regions_cmp(p_dst, p_src, cnt)) 
		{
			return false;
		}
	}
	
    return true;
}

static bool walkbits0_cmp(uint32_t *p_dst, uint32_t *p_src, uint32_t cnt) 
{
    uint32_t i, j, k = 0;
	uint32_t *p1 = p_dst;
    uint32_t *p2 = p_src;
	
    for (j = 0; j < UL_LEN; j++) 
	{
        info("\%s, p_dst: 0x%x, p_src: 0x%x, cnt: 0x%x\n", __func__, p_dst, p_src, cnt);
		info("setting 0x%x\n", j);
		
        p1 = p_dst;
		p2 = p_src;
        
        for (i = 0; i < (cnt >> 2); i++) 
		{
            if (j < (UL_LEN/2)) 
			{ 
				// Walk it up.
                *p1++ = *p2++ = ONE << j;
            } 
			else 
			{ 
				// Walk it back down.
                *p1++ = *p2++ = ONE << (UL_LEN - 1 - j);
            }

			if (!(i % PROGRESSOFTEN))
			{
				info("%c", g_progress[k++ % PROGRESSLEN]);
			}
        }
		
		if (!regions_cmp(p_dst, p_src, cnt)) 
		{
			return false;
		}
    }
	
    return true;
}

static bool walkbits1_cmp(uint32_t *p_dst, uint32_t *p_src, uint32_t cnt)	
{
    uint32_t i, j, k = 0;
	uint32_t *p1 = p_dst;
    uint32_t *p2 = p_src;

    for (j = 0; j < UL_LEN; j++) 
	{
        info("\%s, p_dst: 0x%x, p_src: 0x%x, cnt: 0x%x\n", __func__, p_dst, p_src, cnt);
		info("setting 0x%x\n", j);

		p1 = p_dst;
		p2 = p_src;
        
        for (i = 0; i < (cnt >> 2); i++)
		{
            if (j < (UL_LEN/2)) 
			{ 
				// Walk it up.
                *p1++ = *p2++ = UL_ONEBITS ^ (ONE << j);
            } 
			else 
			{ 
				// Walk it back down.
                *p1++ = *p2++ = UL_ONEBITS ^ (ONE << (UL_LEN - 1 - j));
            }

			if (!(i % PROGRESSOFTEN))
			{
				info("%c", g_progress[k++ % PROGRESSLEN]);
			}
        }
		
        if (!regions_cmp(p_dst, p_src, cnt)) 
		{
			return false;
		}
    }

	return true;
}
	
static bool wide8bits_random_cmp(uint32_t *p_dst, uint32_t *p_src, uint32_t cnt)	
{
	uint8_t *p1, *t;
	uint32_t *p2;
	uint32_t attempt;
	uint32_t i, b, j = 0;

	srand(get_ticks());

    for (attempt = 0; attempt < 2;  attempt++) 
	{
		info("\%s, p_dst: 0x%x, p_src: 0x%x, cnt: 0x%x\n", __func__, p_dst, p_src, cnt);
		info("setting 0x%x\n", attempt);

        if (attempt & 1) 
		{
            p1 = (uint8_t *) p_dst;
            p2 = p_src;
        } 
		else 
		{
            p1 = (uint8_t *) p_src;
            p2 = p_dst;
        }
		
        for (i = 0; i < (cnt >> 2); i++) 
		{
            t = mword8.bytes;
            *p2++ = mword8.val = cb_rand();
			
            for (b = 0; b < (UL_LEN/8); b++) 
			{
                *p1++ = *t++;
            }
			
            if (!(i % PROGRESSOFTEN))
			{
				info("%c", g_progress[j++ % PROGRESSLEN]);
			}
        }
		
        if (!regions_cmp(p_dst, p_src, cnt)) 
		{
			return false;
		}
    }

	return true;
}

static bool wide16bits_random_cmp(uint32_t *p_dst, uint32_t *p_src, uint32_t cnt)
{
    uint16_t *p1, *t;
    uint32_t *p2;
    uint32_t attempt;
	uint32_t i, b, j = 0;
    
	srand(get_ticks());
	
    for (attempt = 0; attempt < 2; attempt++) 
	{
		info("\%s, p_dst: 0x%x, p_src: 0x%x, cnt: 0x%x\n", __func__, p_dst, p_src, cnt);
		info("setting 0x%x\n", attempt);
		
        if (attempt & 1) 
		{
            p1 = (uint16_t *) p_dst;
            p2 = p_src;
        } 
		else 
		{
            p1 = (uint16_t *) p_src;
            p2 = p_dst;
        }
		
        for (i = 0; i < (cnt >> 2); i++) 
		{
            t = mword16.u16s;
            *p2++ = mword16.val = cb_rand();
			
            for (b = 0; b < (UL_LEN/16); b++) 
			{
                *p1++ = *t++;
            }
			
            if (!(i % PROGRESSOFTEN))
			{
				info("%c", g_progress[j++ % PROGRESSLEN]);
			}
        }
		
        if (!regions_cmp(p_dst, p_src, cnt)) 
		{
			return false;
		}
    }
	
    return true;
}

static int ddr_dataline_probe(int argc, char *argv[])
{
	uint32_t fail = 0;
	uint32_t addr = DDR_BASE;

	info("%s\n", __func__);

	if (!ddr_post_dataline((uint32_t *)addr))
	{
		fail++;
	}

	if (fail)
	{
		info("%s: test fail\n", argv[0]);
	} 
	else 
	{
		info("%s: test pass\n", argv[0]);
	}	

	return 0;
}

static int ddr_addrline_probe(int argc, char *argv[])
{
	uint32_t fail = 0;
	uint32_t addr, cnt, total_cnt;

	info("%s\n", __func__);

	// Check the validity of parameter count
	if (argc < 2) 
	{
		info("wrong cmd_format: %s size\n", argv[0]);
		return 1;
	}

	// Get the parameters	
	total_cnt = get_arg_ulong(argv[1]);
	if ((total_cnt <= (g_ddr_revd_addr_end - DDR_BASE)) || 
		(total_cnt > g_ddr_test_size))
	{
		info("ddr memory size(0x%x) is invalid!\n", total_cnt);
		info("min test size is 0x%x byte\n", (g_ddr_revd_addr_end - DDR_BASE + 1));
		info("max test size is 0x%x byte\n", g_ddr_test_size);
		return 1;
	}

	if (g_ddr_revd_addr_st != g_ddr_revd_addr_end)
	{
		addr = DDR_BASE;
		cnt = g_ddr_revd_addr_st - addr;
		if (!ddr_post_addrline((uint32_t *)addr, cnt))
		{
			fail++;
		}
	}
	
	addr = g_ddr_revd_addr_end;
	cnt = DDR_BASE + total_cnt - addr;
	if (!ddr_post_addrline((uint32_t *)addr, cnt))
	{
		fail++;
	}

	if (fail)
	{
		info("%s: test fail\n", argv[0]);
	} 
	else 
	{
		info("%s: test pass\n", argv[0]);
	}	

	return 0;
}


static int ddr_random_value_test(int argc, char *argv[])
{
	uint32_t fail = 0;
	uint32_t cnt, total_cnt;
	uint32_t *p_src, *p_dst; 

	info("%s\n", __func__);

	// Check the validity of parameter count
	if (argc < 2) 
	{
		info("wrong cmd_format: %s size\n", argv[0]);
		return 1;
	}

	// Get the parameters	
	total_cnt = get_arg_ulong(argv[1]);
	if ((total_cnt <= (g_ddr_revd_addr_end - DDR_BASE)) || 
		(total_cnt > g_ddr_test_size))
	{
		info("ddr memory size(0x%x) is invalid!\n", total_cnt);
		info("min test size is 0x%x byte\n", (g_ddr_revd_addr_end - DDR_BASE + 1));
		info("max test size is 0x%x byte\n", g_ddr_test_size);
		return 1;
	}

	if (g_ddr_revd_addr_st != g_ddr_revd_addr_end)
	{
		p_src = (uint32_t *)DDR_BASE;
		cnt = (g_ddr_revd_addr_st - (uint32_t)p_src)>>1;
		p_dst = (uint32_t *)(p_src + (cnt>>2));
		if (!random_value_cmp(p_dst, p_src, cnt))
		{
			fail++;
			goto end;
		}
	}

	p_src = (uint32_t *)g_ddr_revd_addr_end;
	cnt = (DDR_BASE + total_cnt - (uint32_t)p_src)>>1;
	p_dst = (uint32_t *)(p_src + (cnt>>2));
	if (!random_value_cmp(p_dst, p_src, cnt))
	{
		fail++;
	}

end:
	if (fail)
	{
		info("%s: test fail\n", argv[0]);
	} 
	else 
	{
		info("%s: test pass\n", argv[0]);
	}	

	return 0;
}

static int ddr_xor_cmp_test(int argc, char *argv[])
{
	uint32_t fail = 0;
	uint32_t cnt, total_cnt;
	uint32_t *p_src, *p_dst; 

	info("%s\n", __func__);

	// Check the validity of parameter count
	if (argc < 2) 
	{
		info("wrong cmd_format: %s size\n", argv[0]);
		return 1;
	}

	// Get the parameters	
	total_cnt = get_arg_ulong(argv[1]);
	if ((total_cnt <= (g_ddr_revd_addr_end - DDR_BASE)) || 
		(total_cnt > g_ddr_test_size))
	{
		info("ddr memory size(0x%x) is invalid!\n", total_cnt);
		info("min test size is 0x%x byte\n", (g_ddr_revd_addr_end - DDR_BASE + 1));
		info("max test size is 0x%x byte\n", g_ddr_test_size);
		return 1;
	}

	if (g_ddr_revd_addr_st != g_ddr_revd_addr_end)
	{
		p_src = (uint32_t *)DDR_BASE;
		cnt = (g_ddr_revd_addr_st - (uint32_t)p_src)>>1;
		p_dst = (uint32_t *)(p_src + (cnt>>2));
		if (!xor_cmp(p_dst, p_src, cnt))
		{
			fail++;
			goto end;
		}
	}

	p_src = (uint32_t *)g_ddr_revd_addr_end;
	cnt = (DDR_BASE + total_cnt - (uint32_t)p_src)>>1;
	p_dst = (uint32_t *)(p_src + (cnt>>2));
	if (!xor_cmp(p_dst, p_src, cnt))
	{
		fail++;
	}

end:
	if (fail)
	{
		info("%s: test fail\n", argv[0]);
	} 
	else 
	{
		info("%s: test pass\n", argv[0]);
	}	

	return 0;
}

static int ddr_sub_cmp_test(int argc, char *argv[])
{
	uint32_t fail = 0;
	uint32_t cnt, total_cnt;
	uint32_t *p_src, *p_dst; 

	info("%s\n", __func__);

	// Check the validity of parameter count
	if (argc < 2) 
	{
		info("wrong cmd_format: %s size\n", argv[0]);
		return 1;
	}

	// Get the parameters	
	total_cnt = get_arg_ulong(argv[1]);
	if ((total_cnt <= (g_ddr_revd_addr_end - DDR_BASE)) || 
		(total_cnt > g_ddr_test_size))
	{
		info("ddr memory size(0x%x) is invalid!\n", total_cnt);
		info("min test size is 0x%x byte\n", (g_ddr_revd_addr_end - DDR_BASE + 1));
		info("max test size is 0x%x byte\n", g_ddr_test_size);
		return 1;
	}

	if (g_ddr_revd_addr_st != g_ddr_revd_addr_end)
	{
		p_src = (uint32_t *)DDR_BASE;
		cnt = (g_ddr_revd_addr_st - (uint32_t)p_src)>>1;
		p_dst = (uint32_t *)(p_src + (cnt>>2));
		if (!sub_cmp(p_dst, p_src, cnt))
		{
			fail++;
			goto end;
		}
	}
	
	p_src = (uint32_t *)g_ddr_revd_addr_end;
	cnt = (DDR_BASE + total_cnt - (uint32_t)p_src)>>1;
	p_dst = (uint32_t *)(p_src + (cnt>>2));
	if (!sub_cmp(p_dst, p_src, cnt))
	{
		fail++;
	}

end:
	if (fail)
	{
		info("%s: test fail\n", argv[0]);
	} 
	else 
	{
		info("%s: test pass\n", argv[0]);
	}	

	return 0;
}

static int ddr_mul_cmp_test(int argc, char *argv[])
{
	uint32_t fail = 0;
	uint32_t cnt, total_cnt;
	uint32_t *p_src, *p_dst; 

	info("%s\n", __func__);

	// Check the validity of parameter count
	if (argc < 2) 
	{
		info("wrong cmd_format: %s size\n", argv[0]);
		return 1;
	}

	// Get the parameters	
	total_cnt = get_arg_ulong(argv[1]);
	if ((total_cnt <= (g_ddr_revd_addr_end - DDR_BASE)) || 
		(total_cnt > g_ddr_test_size))
	{
		info("ddr memory size(0x%x) is invalid!\n", total_cnt);
		info("min test size is 0x%x byte\n", (g_ddr_revd_addr_end - DDR_BASE + 1));
		info("max test size is 0x%x byte\n", g_ddr_test_size);
		return 1;
	}

	if (g_ddr_revd_addr_st != g_ddr_revd_addr_end)
	{
		p_src = (uint32_t *)DDR_BASE;
		cnt = (g_ddr_revd_addr_st - (uint32_t)p_src)>>1;
		p_dst = (uint32_t *)(p_src + (cnt>>2));
		if (!mul_cmp(p_dst, p_src, cnt))
		{
			fail++;
			goto end;
		}
	}

	p_src = (uint32_t *)g_ddr_revd_addr_end;
	cnt = (DDR_BASE + total_cnt - (uint32_t)p_src)>>1;
	p_dst = (uint32_t *)(p_src + (cnt>>2));
	if (!mul_cmp(p_dst, p_src, cnt))
	{
		fail++;
	}

end:
	if (fail)
	{
		info("%s: test fail\n", argv[0]);
	} 
	else 
	{
		info("%s: test pass\n", argv[0]);
	}	

	return 0;
}

static int ddr_div_cmp_test(int argc, char *argv[])
{
	uint32_t fail = 0;
	uint32_t cnt, total_cnt;
	uint32_t *p_src, *p_dst; 

	info("%s\n", __func__);

	// Check the validity of parameter count
	if (argc < 2) 
	{
		info("wrong cmd_format: %s size\n", argv[0]);
		return 1;
	}

	// Get the parameters	
	total_cnt = get_arg_ulong(argv[1]);
	if ((total_cnt <= (g_ddr_revd_addr_end - DDR_BASE)) || 
		(total_cnt > g_ddr_test_size))
	{
		info("ddr memory size(0x%x) is invalid!\n", total_cnt);
		info("min test size is 0x%x byte\n", (g_ddr_revd_addr_end - DDR_BASE + 1));
		info("max test size is 0x%x byte\n", g_ddr_test_size);
		return 1;
	}

	if (g_ddr_revd_addr_st != g_ddr_revd_addr_end)
	{
		p_src = (uint32_t *)DDR_BASE;
		cnt = (g_ddr_revd_addr_st - (uint32_t)p_src)>>1;
		p_dst = (uint32_t *)(p_src + (cnt>>2));
		if (!div_cmp(p_dst, p_src, cnt))
		{
			fail++;
			goto end;
		}
	}

	p_src = (uint32_t *)g_ddr_revd_addr_end;
	cnt = (DDR_BASE + total_cnt - (uint32_t)p_src)>>1;
	p_dst = (uint32_t *)(p_src + (cnt>>2));
	if (!div_cmp(p_dst, p_src, cnt))
	{
		fail++;
	}

end:
	if (fail)
	{
		info("%s: test fail\n", argv[0]);
	} 
	else 
	{
		info("%s: test pass\n", argv[0]);
	}	

	return 0;
}

static int ddr_or_cmp_test(int argc, char *argv[])
{
	uint32_t fail = 0;
	uint32_t cnt, total_cnt;
	uint32_t *p_src, *p_dst; 

	info("%s\n", __func__);

	// Check the validity of parameter count
	if (argc < 2) 
	{
		info("wrong cmd_format: %s size\n", argv[0]);
		return 1;
	}

	// Get the parameters	
	total_cnt = get_arg_ulong(argv[1]);
	if ((total_cnt <= (g_ddr_revd_addr_end - DDR_BASE)) || 
		(total_cnt > g_ddr_test_size))
	{
		info("ddr memory size(0x%x) is invalid!\n", total_cnt);
		info("min test size is 0x%x byte\n", (g_ddr_revd_addr_end - DDR_BASE + 1));
		info("max test size is 0x%x byte\n", g_ddr_test_size);
		return 1;
	}

	if (g_ddr_revd_addr_st != g_ddr_revd_addr_end)
	{
		p_src = (uint32_t *)DDR_BASE;
		cnt = (g_ddr_revd_addr_st - (uint32_t)p_src)>>1;
		p_dst = (uint32_t *)(p_src + (cnt>>2));
		if (!or_cmp(p_dst, p_src, cnt))
		{
			fail++;
			goto end;
		}
	}

	p_src = (uint32_t *)g_ddr_revd_addr_end;
	cnt = (DDR_BASE + total_cnt - (uint32_t)p_src)>>1;
	p_dst = (uint32_t *)(p_src + (cnt>>2));
	if (!or_cmp(p_dst, p_src, cnt))
	{
		fail++;
	}

end:
	if (fail)
	{
		info("%s: test fail\n", argv[0]);
	} 
	else 
	{
		info("%s: test pass\n", argv[0]);
	}	

	return 0;
}

static int ddr_and_cmp_test(int argc, char *argv[])
{
	uint32_t fail = 0;
	uint32_t cnt, total_cnt;
	uint32_t *p_src, *p_dst; 

	info("%s\n", __func__);

	// Check the validity of parameter count
	if (argc < 2) 
	{
		info("wrong cmd_format: %s size\n", argv[0]);
		return 1;
	}

	// Get the parameters	
	total_cnt = get_arg_ulong(argv[1]);
	if ((total_cnt <= (g_ddr_revd_addr_end - DDR_BASE)) || 
		(total_cnt > g_ddr_test_size))
	{
		info("ddr memory size(0x%x) is invalid!\n", total_cnt);
		info("min test size is 0x%x byte\n", (g_ddr_revd_addr_end - DDR_BASE + 1));
		info("max test size is 0x%x byte\n", g_ddr_test_size);
		return 1;
	}

	if (g_ddr_revd_addr_st != g_ddr_revd_addr_end)
	{
		p_src = (uint32_t *)DDR_BASE;
		cnt = (g_ddr_revd_addr_st - (uint32_t)p_src)>>1;
		p_dst = (uint32_t *)(p_src + (cnt>>2));
		if (!and_cmp(p_dst, p_src, cnt))
		{
			fail++;
			goto end;
		}
	}

	p_src = (uint32_t *)g_ddr_revd_addr_end;
	cnt = (DDR_BASE + total_cnt - (uint32_t)p_src)>>1;
	p_dst = (uint32_t *)(p_src + (cnt>>2));
	if (!and_cmp(p_dst, p_src, cnt))
	{
		fail++;
	}

end:
	if (fail)
	{
		info("%s: test fail\n", argv[0]);
	} 
	else 
	{
		info("%s: test pass\n", argv[0]);
	}	

	return 0;
}

static int ddr_seqinc_cmp_test(int argc, char *argv[])
{
	uint32_t fail = 0;
	uint32_t cnt, total_cnt;
	uint32_t *p_src, *p_dst; 

	info("%s\n", __func__);

	// Check the validity of parameter count
	if (argc < 2) 
	{
		info("wrong cmd_format: %s size\n", argv[0]);
		return 1;
	}

	// Get the parameters	
	total_cnt = get_arg_ulong(argv[1]);
	if ((total_cnt <= (g_ddr_revd_addr_end - DDR_BASE)) || 
		(total_cnt > g_ddr_test_size))
	{
		info("ddr memory size(0x%x) is invalid!\n", total_cnt);
		info("min test size is 0x%x byte\n", (g_ddr_revd_addr_end - DDR_BASE + 1));
		info("max test size is 0x%x byte\n", g_ddr_test_size);
		return 1;
	}

	if (g_ddr_revd_addr_st != g_ddr_revd_addr_end)
	{
		p_src = (uint32_t *)DDR_BASE;
		cnt = (g_ddr_revd_addr_st - (uint32_t)p_src)>>1;
		p_dst = (uint32_t *)(p_src + (cnt>>2));
		if (!seqinc_cmp(p_dst, p_src, cnt))
		{
			fail++;
			goto end;
		}
	}

	p_src = (uint32_t *)g_ddr_revd_addr_end;
	cnt = (DDR_BASE + total_cnt - (uint32_t)p_src)>>1;
	p_dst = (uint32_t *)(p_src + (cnt>>2));
	if (!seqinc_cmp(p_dst, p_src, cnt))
	{
		fail++;
	}

end:
	if (fail)
	{
		info("%s: test fail\n", argv[0]);
	} 
	else 
	{
		info("%s: test pass\n", argv[0]);
	}	

	return 0;
}

static int ddr_solidbits_cmp_test(int argc, char *argv[])
{
	uint32_t fail = 0;
	uint32_t cnt, total_cnt;
	uint32_t *p_src, *p_dst; 

	info("%s\n", __func__);

	// Check the validity of parameter count
	if (argc < 2) 
	{
		info("wrong cmd_format: %s size\n", argv[0]);
		return 1;
	}

	// Get the parameters	
	total_cnt = get_arg_ulong(argv[1]);
	if ((total_cnt <= (g_ddr_revd_addr_end - DDR_BASE)) || 
		(total_cnt > g_ddr_test_size))
	{
		info("ddr memory size(0x%x) is invalid!\n", total_cnt);
		info("min test size is 0x%x byte\n", (g_ddr_revd_addr_end - DDR_BASE + 1));
		info("max test size is 0x%x byte\n", g_ddr_test_size);
		return 1;
	}

	if (g_ddr_revd_addr_st != g_ddr_revd_addr_end)
	{
		p_src = (uint32_t *)DDR_BASE;
		cnt = (g_ddr_revd_addr_st - (uint32_t)p_src)>>1;
		p_dst = (uint32_t *)(p_src + (cnt>>2));
		if (!solidbits_cmp(p_dst, p_src, cnt))
		{
			fail++;
			goto end;
		}
	}

	p_src = (uint32_t *)g_ddr_revd_addr_end;
	cnt = (DDR_BASE + total_cnt - (uint32_t)p_src)>>1;
	p_dst = (uint32_t *)(p_src + (cnt>>2));
	if (!solidbits_cmp(p_dst, p_src, cnt))
	{
		fail++;
	}

end:
	if (fail)
	{
		info("%s: test fail\n", argv[0]);
	} 
	else 
	{
		info("%s: test pass\n", argv[0]);
	}	

	return 0;
}

static int ddr_blockseq_cmp_test(int argc, char *argv[])
{
	uint32_t fail = 0;
	uint32_t cnt, total_cnt;
	uint32_t *p_src, *p_dst; 

	info("%s\n", __func__);

	// Check the validity of parameter count
	if (argc < 2) 
	{
		info("wrong cmd_format: %s size\n", argv[0]);
		return 1;
	}

	// Get the parameters	
	total_cnt = get_arg_ulong(argv[1]);
	if ((total_cnt <= (g_ddr_revd_addr_end - DDR_BASE)) || 
		(total_cnt > g_ddr_test_size))
	{
		info("ddr memory size(0x%x) is invalid!\n", total_cnt);
		info("min test size is 0x%x byte\n", (g_ddr_revd_addr_end - DDR_BASE + 1));
		info("max test size is 0x%x byte\n", g_ddr_test_size);
		return 1;
	}

	if (g_ddr_revd_addr_st != g_ddr_revd_addr_end)
	{
		p_src = (uint32_t *)DDR_BASE;
		cnt = (g_ddr_revd_addr_st - (uint32_t)p_src)>>1;
		p_dst = (uint32_t *)(p_src + (cnt>>2));
		if (!blockseq_cmp(p_dst, p_src, cnt))
		{
			fail++;
			goto end;
		}
	}

	p_src = (uint32_t *)g_ddr_revd_addr_end;
	cnt = (DDR_BASE + total_cnt - (uint32_t)p_src)>>1;
	p_dst = (uint32_t *)(p_src + (cnt>>2));
	if (!blockseq_cmp(p_dst, p_src, cnt))
	{
		fail++;
	}

end:
	if (fail)
	{
		info("%s: test fail\n", argv[0]);
	} 
	else 
	{
		info("%s: test pass\n", argv[0]);
	}	

	return 0;
}

static int ddr_chkboard_cmp_test(int argc, char *argv[])
{
	uint32_t fail = 0;
	uint32_t cnt, total_cnt;
	uint32_t *p_src, *p_dst; 

	info("%s\n", __func__);

	// Check the validity of parameter count
	if (argc < 2) 
	{
		info("wrong cmd_format: %s size\n", argv[0]);
		return 1;
	}

	// Get the parameters	
	total_cnt = get_arg_ulong(argv[1]);
	if ((total_cnt <= (g_ddr_revd_addr_end - DDR_BASE)) || 
		(total_cnt > g_ddr_test_size))
	{
		info("ddr memory size(0x%x) is invalid!\n", total_cnt);
		info("min test size is 0x%x byte\n", (g_ddr_revd_addr_end - DDR_BASE + 1));
		info("max test size is 0x%x byte\n", g_ddr_test_size);
		return 1;
	}

	if (g_ddr_revd_addr_st != g_ddr_revd_addr_end)
	{
		p_src = (uint32_t *)DDR_BASE;
		cnt = (g_ddr_revd_addr_st - (uint32_t)p_src)>>1;
		p_dst = (uint32_t *)(p_src + (cnt>>2));
		if (!chkboard_cmp(p_dst, p_src, cnt))
		{
			fail++;
			goto end;
		}
	}

	p_src = (uint32_t *)g_ddr_revd_addr_end;
	cnt = (DDR_BASE + total_cnt - (uint32_t)p_src)>>1;
	p_dst = (uint32_t *)(p_src + (cnt>>2));
	if (!chkboard_cmp(p_dst, p_src, cnt))
	{
		fail++;
	}

end:
	if (fail)
	{
		info("%s: test fail\n", argv[0]);
	} 
	else 
	{
		info("%s: test pass\n", argv[0]);
	}	

	return 0;
}

static int ddr_bitspread_cmp_test(int argc, char *argv[])
{
	uint32_t fail = 0;
	uint32_t cnt, total_cnt;
	uint32_t *p_src, *p_dst; 

	info("%s\n", __func__);

	// Check the validity of parameter count
	if (argc < 2) 
	{
		info("wrong cmd_format: %s size\n", argv[0]);
		return 1;
	}

	// Get the parameters	
	total_cnt = get_arg_ulong(argv[1]);
	if ((total_cnt <= (g_ddr_revd_addr_end - DDR_BASE)) || 
		(total_cnt > g_ddr_test_size))
	{
		info("ddr memory size(0x%x) is invalid!\n", total_cnt);
		info("min test size is 0x%x byte\n", (g_ddr_revd_addr_end - DDR_BASE + 1));
		info("max test size is 0x%x byte\n", g_ddr_test_size);
		return 1;
	}

	if (g_ddr_revd_addr_st != g_ddr_revd_addr_end)
	{
		p_src = (uint32_t *)DDR_BASE;
		cnt = (g_ddr_revd_addr_st - (uint32_t)p_src)>>1;
		p_dst = (uint32_t *)(p_src + (cnt>>2));
		if (!bitspread_cmp(p_dst, p_src, cnt))
		{
			fail++;
			goto end;
		}
	}

	p_src = (uint32_t *)g_ddr_revd_addr_end;
	cnt = (DDR_BASE + total_cnt - (uint32_t)p_src)>>1;
	p_dst = (uint32_t *)(p_src + (cnt>>2));
	if (!bitspread_cmp(p_dst, p_src, cnt))
	{
		fail++;
	}

end:
	if (fail)
	{
		info("%s: test fail\n", argv[0]);
	} 
	else 
	{
		info("%s: test pass\n", argv[0]);
	}	

	return 0;
}

static int ddr_bitflip_cmp_test(int argc, char *argv[])
{
	uint32_t fail = 0;
	uint32_t cnt, total_cnt;
	uint32_t *p_src, *p_dst; 

	info("%s\n", __func__);

	// Check the validity of parameter count
	if (argc < 2) 
	{
		info("wrong cmd_format: %s size\n", argv[0]);
		return 1;
	}

	// Get the parameters	
	total_cnt = get_arg_ulong(argv[1]);
	if ((total_cnt <= (g_ddr_revd_addr_end - DDR_BASE)) || 
		(total_cnt > g_ddr_test_size))
	{
		info("ddr memory size(0x%x) is invalid!\n", total_cnt);
		info("min test size is 0x%x byte\n", (g_ddr_revd_addr_end - DDR_BASE + 1));
		info("max test size is 0x%x byte\n", g_ddr_test_size);
		return 1;
	}

	if (g_ddr_revd_addr_st != g_ddr_revd_addr_end)
	{
		p_src = (uint32_t *)DDR_BASE;
		cnt = (g_ddr_revd_addr_st - (uint32_t)p_src)>>1;
		p_dst = (uint32_t *)(p_src + (cnt>>2));
		if (!bitflip_cmp(p_dst, p_src, cnt))
		{
			fail++;
			goto end;
		}
	}

	p_src = (uint32_t *)g_ddr_revd_addr_end;
	cnt = (DDR_BASE + total_cnt - (uint32_t)p_src)>>1;
	p_dst = (uint32_t *)(p_src + (cnt>>2));
	if (!bitflip_cmp(p_dst, p_src, cnt))
	{
		fail++;
	}

end:
	if (fail)
	{
		info("%s: test fail\n", argv[0]);
	} 
	else 
	{
		info("%s: test pass\n", argv[0]);
	}	

	return 0;
}

static int ddr_walkbits0_cmp_test(int argc, char *argv[])
{
	uint32_t fail = 0;
	uint32_t cnt, total_cnt;
	uint32_t *p_src, *p_dst; 

	info("%s\n", __func__);

	// Check the validity of parameter count
	if (argc < 2) 
	{
		info("wrong cmd_format: %s size\n", argv[0]);
		return 1;
	}

	// Get the parameters	
	total_cnt = get_arg_ulong(argv[1]);
	if ((total_cnt <= (g_ddr_revd_addr_end - DDR_BASE)) || 
		(total_cnt > g_ddr_test_size))
	{
		info("ddr memory size(0x%x) is invalid!\n", total_cnt);
		info("min test size is 0x%x byte\n", (g_ddr_revd_addr_end - DDR_BASE + 1));
		info("max test size is 0x%x byte\n", g_ddr_test_size);
		return 1;
	}

	if (g_ddr_revd_addr_st != g_ddr_revd_addr_end)
	{
		p_src = (uint32_t *)DDR_BASE;
		cnt = (g_ddr_revd_addr_st - (uint32_t)p_src)>>1;
		p_dst = (uint32_t *)(p_src + (cnt>>2));
		if (!walkbits0_cmp(p_dst, p_src, cnt))
		{
			fail++;
			goto end;
		}
	}

	p_src = (uint32_t *)g_ddr_revd_addr_end;
	cnt = (DDR_BASE + total_cnt - (uint32_t)p_src)>>1;
	p_dst = (uint32_t *)(p_src + (cnt>>2));
	if (!walkbits0_cmp(p_dst, p_src, cnt))
	{
		fail++;
	}

end:
	if (fail)
	{
		info("%s: test fail\n", argv[0]);
	} 
	else 
	{
		info("%s: test pass\n", argv[0]);
	}	

	return 0;
}

static int ddr_walkbits1_cmp_test(int argc, char *argv[])
{
	uint32_t fail = 0;
	uint32_t cnt, total_cnt;
	uint32_t *p_src, *p_dst; 

	info("%s\n", __func__);

	// Check the validity of parameter count
	if (argc < 2) 
	{
		info("wrong cmd_format: %s size\n", argv[0]);
		return 1;
	}

	// Get the parameters	
	total_cnt = get_arg_ulong(argv[1]);
	if ((total_cnt <= (g_ddr_revd_addr_end - DDR_BASE)) || 
		(total_cnt > g_ddr_test_size))
	{
		info("ddr memory size(0x%x) is invalid!\n", total_cnt);
		info("min test size is 0x%x byte\n", (g_ddr_revd_addr_end - DDR_BASE + 1));
		info("max test size is 0x%x byte\n", g_ddr_test_size);
		return 1;
	}

	if (g_ddr_revd_addr_st != g_ddr_revd_addr_end)
	{
		p_src = (uint32_t *)DDR_BASE;
		cnt = (g_ddr_revd_addr_st - (uint32_t)p_src)>>1;
		p_dst = (uint32_t *)(p_src + (cnt>>2));
		if (!walkbits1_cmp(p_dst, p_src, cnt))
		{
			fail++;
			goto end;
		}
	}
	
	p_src = (uint32_t *)g_ddr_revd_addr_end;
	cnt = (DDR_BASE + total_cnt - (uint32_t)p_src)>>1;
	p_dst = (uint32_t *)(p_src + (cnt>>2));
	if (!walkbits1_cmp(p_dst, p_src, cnt))
	{
		fail++;
	}

end:
	if (fail)
	{
		info("%s: test fail\n", argv[0]);
	} 
	else 
	{
		info("%s: test pass\n", argv[0]);
	}	

	return 0;
}

static int ddr_wide8bits_cmp_test(int argc, char *argv[])
{
	uint32_t fail = 0;
	uint32_t cnt, total_cnt;
	uint32_t *p_src, *p_dst; 

	info("%s\n", __func__);

	// Check the validity of parameter count
	if (argc < 2) 
	{
		info("wrong cmd_format: %s size\n", argv[0]);
		return 1;
	}

	// Get the parameters	
	total_cnt = get_arg_ulong(argv[1]);
	if ((total_cnt <= (g_ddr_revd_addr_end - DDR_BASE)) || 
		(total_cnt > g_ddr_test_size))
	{
		info("ddr memory size(0x%x) is invalid!\n", total_cnt);
		info("min test size is 0x%x byte\n", (g_ddr_revd_addr_end - DDR_BASE + 1));
		info("max test size is 0x%x byte\n", g_ddr_test_size);
		return 1;
	}

	if (g_ddr_revd_addr_st != g_ddr_revd_addr_end)
	{
		p_src = (uint32_t *)DDR_BASE;
		cnt = (g_ddr_revd_addr_st - (uint32_t)p_src)>>1;
		p_dst = (uint32_t *)(p_src + (cnt>>2));
		if (!wide8bits_random_cmp(p_dst, p_src, cnt))
		{
			fail++;
			goto end;
		}
	}

	p_src = (uint32_t *)g_ddr_revd_addr_end;
	cnt = (DDR_BASE + total_cnt - (uint32_t)p_src)>>1;
	p_dst = (uint32_t *)(p_src + (cnt>>2));
	if (!wide8bits_random_cmp(p_dst, p_src, cnt))
	{
		fail++;
	}

end:
	if (fail)
	{
		info("%s: test fail\n", argv[0]);
	} 
	else 
	{
		info("%s: test pass\n", argv[0]);
	}	

	return 0;
}

static int ddr_wide16bits_cmp_test(int argc, char *argv[])
{
	uint32_t fail = 0;
	uint32_t cnt, total_cnt;
	uint32_t *p_src, *p_dst; 

	info("%s\n", __func__);

	// Check the validity of parameter count
	if (argc < 2) 
	{
		info("wrong cmd_format: %s size\n", argv[0]);
		return 1;
	}

	// Get the parameters	
	total_cnt = get_arg_ulong(argv[1]);
	if ((total_cnt <= (g_ddr_revd_addr_end - DDR_BASE)) || 
		(total_cnt > g_ddr_test_size))
	{
		info("ddr memory size(0x%x) is invalid!\n", total_cnt);
		info("min test size is 0x%x byte\n", (g_ddr_revd_addr_end - DDR_BASE + 1));
		info("max test size is 0x%x byte\n", g_ddr_test_size);
		return 1;
	}

	if (g_ddr_revd_addr_st != g_ddr_revd_addr_end)
	{
		p_src = (uint32_t *)DDR_BASE;
		cnt = (g_ddr_revd_addr_st - (uint32_t)p_src)>>1;
		p_dst = (uint32_t *)(p_src + (cnt>>2));
		if (!wide16bits_random_cmp(p_dst, p_src, cnt))
		{
			fail++;
			goto end;
		}
	}

	p_src = (uint32_t *)g_ddr_revd_addr_end;
	cnt = (DDR_BASE + total_cnt - (uint32_t)p_src)>>1;
	p_dst = (uint32_t *)(p_src + (cnt>>2));
	if (!wide16bits_random_cmp(p_dst, p_src, cnt))
	{
		fail++;
	}

end:
	if (fail)
	{
		info("%s: test fail\n", argv[0]);
	} 
	else 
	{
		info("%s: test pass\n", argv[0]);
	}	

	return 0;
}

//*****************************************************************************
//
// This is the table that holds the command names, implementing functions,
// and brief description.
//
//*****************************************************************************
cmdline_entry ddr_test_menu[] = {
	{"help",					cmd_help,					"                    : Display list of commands"},
	{"h",						cmd_help,					"                       : alias for help"},
	{"?",						cmd_help,					"                       : alias for help"},
	//{"ddr_init",					ddr_init,	" 					 : ddr initialize"},
	{"ddr_dataline_probe",		ddr_dataline_probe,			"      : ddr dataline probe"},
	{"ddr_addrline_probe",		ddr_addrline_probe,			"      : ddr addrline probe"},
	{"ddr_random_value_test",	ddr_random_value_test,		"   : ddr random value"},
	{"ddr_xor_cmp_test",		ddr_xor_cmp_test,			"        : ddr xor comparison"},
	{"ddr_sub_cmp_test", 		ddr_sub_cmp_test, 			"        : ddr sub comparison"},
	{"ddr_mul_cmp_test", 		ddr_mul_cmp_test, 			"        : ddr mul comparison"},
	{"ddr_div_cmp_test",		ddr_div_cmp_test,			"        : ddr div comparison"},
	{"ddr_or_cmp_test",			ddr_or_cmp_test,			"         : ddr or comparison"},
	{"ddr_and_cmp_test",		ddr_and_cmp_test,			"        : ddr and comparison"},
	{"ddr_seqinc_cmp_test",		ddr_seqinc_cmp_test,		"     : ddr sequencial comparison"},
	{"ddr_solidbits_cmp_test", 	ddr_solidbits_cmp_test,		"  : ddr solidbits comparison"},
	{"ddr_blockseq_cmp_test", 	ddr_blockseq_cmp_test, 		"   : ddr blockseq comparison"},
	{"ddr_chkboard_cmp_test", 	ddr_chkboard_cmp_test, 		"   : ddr checkerboard comparison"},
    {"ddr_bitspread_cmp_test", 	ddr_bitspread_cmp_test, 	"  : ddr bitspread comparison"},
    {"ddr_bitflip_cmp_test", 	ddr_bitflip_cmp_test, 		"    : ddr bitflip comparison"},    
    {"ddr_walkbits0_cmp_test", 	ddr_walkbits0_cmp_test, 	"  : ddr walkbits0 comparison"},
    {"ddr_walkbits1_cmp_test", 	ddr_walkbits1_cmp_test, 	"  : ddr walkbits1 comparison"},
    {"ddr_wide8bits_cmp_test", 	ddr_wide8bits_cmp_test, 	"  : ddr 8bit wide random"},
    {"ddr_wide16bits_cmp_test", ddr_wide16bits_cmp_test, 	" : ddr 16bit wide random"},
	{"q",						NULL,	  					"                       : quit ddr test"},
	{0,							0,		  					0}
};

extern uint32_t _text;
extern uint32_t _ebss;
extern uint32_t _ediscard;

int ddr_test_prepare(int sel)
{
	g_ddr_revd_addr_st = (uint32_t)&_text; 
	g_ddr_revd_addr_end = (uint32_t)&_ediscard;
	g_ddr_test_size = DDR_TEST_SIZE;
	
	info("cdl code area reserved [0x%x, 0x%x], this area should not be tested!\n", g_ddr_revd_addr_st, g_ddr_revd_addr_end);
	info("g_ddr_revd_addr_st:0x%x, 0x%x\n", g_ddr_revd_addr_st, g_ddr_revd_addr_end);
	
	return 0;
}

#endif

