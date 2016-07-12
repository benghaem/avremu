#include "memory.h"
#include "IOREG.h"
#include <stdio.h>
#include <string.h>


/*  ---------------------------------  */
/*  AVR DATA MEMORY (REG, IO, SRAM)    */
/*  ---------------------------------  */

/*
 * Initializes a datamem struct with the proper values
 * This is important to ensure the data memory is zeroed out
 * and that the memory is in the expected state at start
 */
void* datamem_init(struct datamem* d){
    void* ret = memset(&(d->mem[0]),0,sizeof(d->mem));
    /* Set OCR1C to all 1's initial. See page 92 of datasheet */
    datamem_write_io(d, OCR1C, 0xFF);
    /* Set SPH / SPL to last data memory location */
    datamem_write_io_SP(d, DATAMEM_SIZE - 1);
    return ret;
}

/*
 * Reads 8bit value from address in data memory
 * The offset argument allows the address to
 * be located within a specific subspace of memory.
 */
uint8_t datamem_read_addr(struct datamem* d, int offset, int addr){
    int target = offset + addr;
    if (target >= 0 && target < DATAMEM_SIZE){
        return d->mem[offset+addr];
    }
    return 0;
}

/*
 * Reads 16bit value from two addresses in data memory
 * where the resulting 16bit value is the result of
 * logically combining bits from the high address and
 * the low address
 */
uint16_t datamem_read_addr16(struct datamem* d, int offset, int addr_low, int addr_high){
    uint16_t L = datamem_read_addr(d, offset, addr_low);
    uint16_t H = datamem_read_addr(d, offset, addr_high);
    H = H << 8;
    return H | L;
}

/*
 * Writes to any address in data memory
 * we can provide a offset to select
 * a specific subset of memory
 *
 * Returns:
 * -1 on error
 * or
 * the absolute address minus the offset on success
 */
int datamem_write_addr(struct datamem* d, int offset, int addr, uint8_t data){
    int target = offset + addr;
    /*  safety check on ranges */
    if (target >= 0 && target < DATAMEM_SIZE){
        d->mem[target] = data;
        return target - offset;
    } else {
        return -1;
    }
}

/*
 * Writes a 16 bit value across two addresses
 * the high eight bits are placed at the high address
 * and the low eight bits are placed at the low address
 *
 * Returns:
 * -1 on error (occurs if either of the values are outside
 *  the range or if the addresses are equal)
 *  or
 *  the absolute address of the low value minus the offset
 *  on success
 */
int datamem_write_addr16(struct datamem* d, int offset, int addr_low, int addr_high, uint16_t data){
    /* mask to select upper 8 bits from data */
    uint8_t H = (data & 0xFF00) >> 8;
    /* mask to select */
    uint8_t L = data & 0xFF;

    int high_check = 0;
    int low_check = 0;
    int eq_check = 0;

    if (addr_high >= 0 && addr_high < DATAMEM_SIZE){
        high_check = 1;
    }

    if (addr_low >= 0 && addr_low < DATAMEM_SIZE){
        low_check = 1;
    }

    if (addr_low != addr_high){
        eq_check = 1;
    }

    if (high_check && low_check && eq_check){
        datamem_write_addr(d, offset, addr_high, H);
        datamem_write_addr(d, offset, addr_low, L);
        return addr_low - offset;
    }
    return -1;
}

/*  ---------------------------------  */
/*  AVR GENERAL REGISTERS              */
/*  ---------------------------------  */

/*
 * These functions are simply wrappers for the functions
 * above with offsets and extended checks
 */

uint8_t datamem_read_reg(struct datamem* d, int addr){
    if (addr >= 0 && addr < RFILE_SIZE){
        return datamem_read_addr(d, RFILE_OFFSET, addr);
    }
    return 0;
}

int datamem_write_reg(struct datamem* d, int addr, uint8_t data){
    if (addr >= 0 && addr < RFILE_SIZE){
        return datamem_write_addr(d, RFILE_OFFSET, addr, data);
    }
    return -1;
}

uint16_t datamem_read_reg16(struct datamem* d, int addr_low, int addr_high){
    return datamem_read_addr16(d, RFILE_OFFSET, addr_low, addr_high);
}

int datamem_write_reg16(struct datamem* d, int addr_low, int addr_high, uint16_t data){
    return datamem_write_addr16(d, RFILE_OFFSET, addr_low, addr_high, data);
}

uint16_t datamem_read_reg_X(struct datamem* d){
    return datamem_read_reg16(d, REG_XL, REG_XH);
}

uint16_t datamem_read_reg_Y(struct datamem* d){
    return datamem_read_reg16(d, REG_YL, REG_YH);
}

uint16_t datamem_read_reg_Z(struct datamem* d){
    return datamem_read_reg16(d, REG_ZL, REG_ZH);
}

int datamem_write_reg_X(struct datamem* d, uint16_t x){
    return datamem_write_reg16(d, REG_XL, REG_XH, x);
}

int datamem_write_reg_Y(struct datamem* d, uint16_t y){
    return datamem_write_reg16(d, REG_YL, REG_YH, y);
}

int datamem_write_reg_Z(struct datamem* d, uint16_t Z){
    return datamem_write_reg16(d, REG_ZL, REG_ZH, Z);
}

/*  ---------------------------------  */
/*  AVR IO REGISTERS                   */
/*  ---------------------------------  */

uint8_t datamem_read_io(struct datamem* d, int addr){
    if (addr >= 0 && addr < IOFILE_SIZE){
        return datamem_read_addr(d, IOFILE_OFFSET, addr);
    }
    return 0;
}

int datamem_read_io_bit(struct datamem* d, int addr, int bit){
    uint8_t data = datamem_read_io(d, addr);
    if (bit >=0 && bit < 8){
        return (data >> bit) & 0x1;
    }
    return -1;
}

uint16_t datamem_read_io_SP(struct datamem* d){
    return datamem_read_addr16(d, IOFILE_OFFSET, SPL, SPH);
}

int datamem_write_io(struct datamem* d, int addr, uint8_t data){
    if (addr >= 0 && addr < IOFILE_SIZE){
        return datamem_write_addr(d,IOFILE_OFFSET, addr, data);
    }
    return -1;
}

/*
 * Writes a specific bit to a register within the io space
 * This should be used when updating complex io registers
 * such as SREG that contain multiple flags
 *
 * Follows the same return convention as above
 * where -1 represents error
 */
int datamem_write_io_bit(struct datamem* d, int addr, int bit, int data){
    uint8_t current_reg = datamem_read_io(d, addr);
    uint8_t isolated_bit;
    uint8_t mask;
    uint8_t updated_reg;

    if (bit >=0 && bit < 8){
        if (data == 0 || data == 1){
            mask = 0x1 << bit;
            isolated_bit = data << bit;
            /* the idea here is to recognize the bit that needs to be changed */
            /* and whether or not we should swap it from the current value */
            updated_reg = current_reg ^ ((current_reg ^ isolated_bit) & mask);
            return datamem_write_io(d, addr, updated_reg);
        }
    }
    return -1;
}

/* Stack pointer */
int datamem_write_io_SP(struct datamem* d, uint16_t sp){
    return datamem_write_addr16(d, IOFILE_OFFSET, SPL, SPH, sp);
}

/*  ---------------------------------  */
/*  AVR SRAM                           */
/*  ---------------------------------  */

uint8_t datamem_read_sram(struct datamem* d, int addr){
    if (addr >= 0 && addr < SRAM_SIZE){
        return datamem_read_addr(d, addr, SRAM_OFFSET);
    }
    return 0;
}

int datamem_write_sram(struct datamem* d, int addr, uint8_t data){
    if (addr >= 0 && addr < SRAM_SIZE){
        return datamem_write_addr(d, addr, SRAM_OFFSET, data);
    }
    return -1;
}

/*  ---------------------------------  */
/*  datamem DEBUG EXTRAS               */
/*  ---------------------------------  */

/*
 * Displays a region of datamem for use when debugging
 */
void datamem_print_region(struct datamem* d, int startAddr, int stopAddr){
    uint8_t value;
    for (; startAddr < stopAddr; startAddr++){
        value = d->mem[startAddr];
        printf("%i : %X\n", startAddr, value);
    }
}


/*  ---------------------------------  */
/*  AVR PROGRAM MEMORY                 */
/*  ---------------------------------  */

/*
 * Initializes the program memory by setting all
 * values to zero
 */
void* progmem_init(struct progmem* p){
    return memset(&(p->mem[0]),0,sizeof(p->mem));
}

uint16_t progmem_read_addr(struct progmem* p, int addr){
    if (addr >= 0 && addr < PROGMEM_SIZE){
         return p->mem[addr];
    }
    return 0;
}

int progmem_write_addr(struct progmem* p, int addr, uint16_t data){
    if (addr >= 0 && addr < PROGMEM_SIZE){
        p->mem[addr] = data;
        return addr;
    }
    return -1;
}
