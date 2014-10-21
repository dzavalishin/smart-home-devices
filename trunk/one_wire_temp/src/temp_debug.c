
#if DS_DEBUG
void uart_put_temp(const uint8_t subzero, uint8_t cel,
                   uint8_t cel_frac_bits)
{
    uint8_t buffer[sizeof(int)*8+1];
    uint16_t decicelsius;
    uint8_t i, j;

    uart_putc((subzero)?'-':'+');
    uart_puti((int)cel);
    debug_puts(".");

    itoa((cel_frac_bits*DS18X20_FRACCONV),(char*)buffer,10);
    j = 4 - strlen((char*)buffer);

    for (i=0;i<j;i++) debug_puts("0");

    debug_puts((char*)buffer);
    debug_puts("-C [");
    // "rounding"
    uart_putc((subzero)?'-':'+');
    decicelsius = DS18X20_temp_to_decicel(subzero, cel, cel_frac_bits);
    uart_puti( (int)(decicelsius/10) );
    debug_puts(".");
    uart_putc( (decicelsius%10) + '0');
    debug_puts("-C]");
}
#endif

