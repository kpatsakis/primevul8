xmlSerializeHexCharRef(unsigned char *out, int val) {
    unsigned char *ptr;

    *out++ = '&';
    *out++ = '#';
    *out++ = 'x';
    if (val < 0x10) ptr = out;
    else if (val < 0x100) ptr = out + 1;
    else if (val < 0x1000) ptr = out + 2;
    else if (val < 0x10000) ptr = out + 3;
    else if (val < 0x100000) ptr = out + 4;
    else ptr = out + 5;
    out = ptr + 1;
    while (val > 0) {
	switch (val & 0xF) {
	    case 0: *ptr-- = '0'; break;
	    case 1: *ptr-- = '1'; break;
	    case 2: *ptr-- = '2'; break;
	    case 3: *ptr-- = '3'; break;
	    case 4: *ptr-- = '4'; break;
	    case 5: *ptr-- = '5'; break;
	    case 6: *ptr-- = '6'; break;
	    case 7: *ptr-- = '7'; break;
	    case 8: *ptr-- = '8'; break;
	    case 9: *ptr-- = '9'; break;
	    case 0xA: *ptr-- = 'A'; break;
	    case 0xB: *ptr-- = 'B'; break;
	    case 0xC: *ptr-- = 'C'; break;
	    case 0xD: *ptr-- = 'D'; break;
	    case 0xE: *ptr-- = 'E'; break;
	    case 0xF: *ptr-- = 'F'; break;
	    default: *ptr-- = '0'; break;
	}
	val >>= 4;
    }
    *out++ = ';';
    *out = 0;
    return(out);
}