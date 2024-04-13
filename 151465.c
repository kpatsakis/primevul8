int mbedtls_asn1_write_int( unsigned char **p, unsigned char *start, int val )
{
    int ret;
    size_t len = 0;

    // TODO negative values and values larger than 128
    // DER format assumes 2s complement for numbers, so the leftmost bit
    // should be 0 for positive numbers and 1 for negative numbers.
    //
    if( *p - start < 1 )
        return( MBEDTLS_ERR_ASN1_BUF_TOO_SMALL );

    len += 1;
    *--(*p) = val;

    if( val > 0 && **p & 0x80 )
    {
        if( *p - start < 1 )
            return( MBEDTLS_ERR_ASN1_BUF_TOO_SMALL );

        *--(*p) = 0x00;
        len += 1;
    }

    MBEDTLS_ASN1_CHK_ADD( len, mbedtls_asn1_write_len( p, start, len ) );
    MBEDTLS_ASN1_CHK_ADD( len, mbedtls_asn1_write_tag( p, start, MBEDTLS_ASN1_INTEGER ) );

    return( (int) len );
}