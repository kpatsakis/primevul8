der_to_ldap_BitString (struct berval *berValue,
                                   struct berval *ldapValue)
{
	ber_len_t bitPadding=0;
	ber_len_t bits, maxBits;
	char *tmpStr;
	unsigned char byte;
	ber_len_t bitLength;
	ber_len_t valLen;
	unsigned char* valPtr;

	ldapValue->bv_len=0;
	ldapValue->bv_val=NULL;

	/* Gets padding and points to binary data */
	valLen=berValue->bv_len;
	valPtr=(unsigned char*)berValue->bv_val;
	if (valLen) {
		bitPadding=(ber_len_t)(valPtr[0]);
		valLen--;
		valPtr++;
	}
	/* If Block is non DER encoding fixes to DER encoding */
	if (bitPadding >= BITS_PER_BYTE) {
		if (valLen*BITS_PER_BYTE > bitPadding ) {
			valLen-=(bitPadding/BITS_PER_BYTE);
			bitPadding%=BITS_PER_BYTE;
		} else {
			valLen=0;
			bitPadding=0;
		}
	}
	/* Just in case bad encoding */
	if (valLen*BITS_PER_BYTE < bitPadding ) {
		bitPadding=0;
		valLen=0;
	}

	/* Gets buffer to hold RFC4517 Bit String format */
	bitLength=valLen*BITS_PER_BYTE-bitPadding;
	tmpStr=LDAP_MALLOC(bitLength + STR_OVERHEAD + 1);

	if (!tmpStr)
		return LDAP_NO_MEMORY;

	ldapValue->bv_val=tmpStr;
	ldapValue->bv_len=bitLength + STR_OVERHEAD;

	/* Formatting in '*binary-digit'B format */
	maxBits=BITS_PER_BYTE;
	*tmpStr++ ='\'';
	while(valLen) {
		byte=*valPtr;
		if (valLen==1)
			maxBits-=bitPadding;
		for (bits=0; bits<maxBits; bits++) {
			if (0x80 & byte)
				*tmpStr='1';
			else
				*tmpStr='0';
			tmpStr++;
			byte<<=1;
		}
		valPtr++;
		valLen--;
	}
	*tmpStr++ ='\'';
	*tmpStr++ ='B';
	*tmpStr=0;

	return LDAP_SUCCESS;
}