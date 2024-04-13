int mbedtls_asn1_write_printable_string( unsigned char **p, unsigned char *start,
                                 const char *text, size_t text_len )
{
    int ret;
    size_t len = 0;

    MBEDTLS_ASN1_CHK_ADD( len, mbedtls_asn1_write_raw_buffer( p, start,
                  (const unsigned char *) text, text_len ) );

    MBEDTLS_ASN1_CHK_ADD( len, mbedtls_asn1_write_len( p, start, len ) );
    MBEDTLS_ASN1_CHK_ADD( len, mbedtls_asn1_write_tag( p, start, MBEDTLS_ASN1_PRINTABLE_STRING ) );

    return( (int) len );
}