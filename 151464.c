int mbedtls_asn1_write_len( unsigned char **p, unsigned char *start, size_t len )
{
    if( len < 0x80 )
    {
        if( *p - start < 1 )
            return( MBEDTLS_ERR_ASN1_BUF_TOO_SMALL );

        *--(*p) = (unsigned char) len;
        return( 1 );
    }

    if( len <= 0xFF )
    {
        if( *p - start < 2 )
            return( MBEDTLS_ERR_ASN1_BUF_TOO_SMALL );

        *--(*p) = (unsigned char) len;
        *--(*p) = 0x81;
        return( 2 );
    }

    if( *p - start < 3 )
        return( MBEDTLS_ERR_ASN1_BUF_TOO_SMALL );

    // We assume we never have lengths larger than 65535 bytes
    //
    *--(*p) = len % 256;
    *--(*p) = ( len / 256 ) % 256;
    *--(*p) = 0x82;

    return( 3 );
}