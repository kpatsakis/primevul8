static void ssl_write_signature_algorithms_ext( ssl_context *ssl,
                                                unsigned char *buf,
                                                size_t *olen )
{
    unsigned char *p = buf;
    const unsigned char *end = ssl->out_msg + SSL_MAX_CONTENT_LEN;
    size_t sig_alg_len = 0;
#if defined(POLARSSL_RSA_C) || defined(POLARSSL_ECDSA_C)
    unsigned char *sig_alg_list = buf + 6;
#endif

    *olen = 0;

    if( ssl->max_minor_ver != SSL_MINOR_VERSION_3 )
        return;

    SSL_DEBUG_MSG( 3, ( "client hello, adding signature_algorithms extension" ) );

#if defined(POLARSSL_RSA_C)
#if defined(POLARSSL_SHA512_C)
    /* SHA512 + RSA signature, SHA384 + RSA signature */
    sig_alg_len += 4;
#endif
#if defined(POLARSSL_SHA256_C)
    /* SHA256 + RSA signature, SHA224 + RSA signature */
    sig_alg_len += 4;
#endif
#if defined(POLARSSL_SHA1_C)
    /* SHA1 + RSA signature */
    sig_alg_len += 2;
#endif
#if defined(POLARSSL_MD5_C)
    /* MD5 + RSA signature */
    sig_alg_len += 2;
#endif
#endif /* POLARSSL_RSA_C */
#if defined(POLARSSL_ECDSA_C)
#if defined(POLARSSL_SHA512_C)
    /* SHA512 + ECDSA signature, SHA384 + ECDSA signature */
    sig_alg_len += 4;
#endif
#if defined(POLARSSL_SHA256_C)
    /* SHA256 + ECDSA signature, SHA224 + ECDSA signature */
    sig_alg_len += 4;
#endif
#if defined(POLARSSL_SHA1_C)
    /* SHA1 + ECDSA signature */
    sig_alg_len += 2;
#endif
#if defined(POLARSSL_MD5_C)
    /* MD5 + ECDSA signature */
    sig_alg_len += 2;
#endif
#endif /* POLARSSL_ECDSA_C */

    if( end < p || (size_t)( end - p ) < sig_alg_len + 6 )
    {
        SSL_DEBUG_MSG( 1, ( "buffer too small" ) );
        return;
    }

    /*
     * Prepare signature_algorithms extension (TLS 1.2)
     */
    sig_alg_len = 0;

#if defined(POLARSSL_RSA_C)
#if defined(POLARSSL_SHA512_C)
    sig_alg_list[sig_alg_len++] = SSL_HASH_SHA512;
    sig_alg_list[sig_alg_len++] = SSL_SIG_RSA;
    sig_alg_list[sig_alg_len++] = SSL_HASH_SHA384;
    sig_alg_list[sig_alg_len++] = SSL_SIG_RSA;
#endif
#if defined(POLARSSL_SHA256_C)
    sig_alg_list[sig_alg_len++] = SSL_HASH_SHA256;
    sig_alg_list[sig_alg_len++] = SSL_SIG_RSA;
    sig_alg_list[sig_alg_len++] = SSL_HASH_SHA224;
    sig_alg_list[sig_alg_len++] = SSL_SIG_RSA;
#endif
#if defined(POLARSSL_SHA1_C)
    sig_alg_list[sig_alg_len++] = SSL_HASH_SHA1;
    sig_alg_list[sig_alg_len++] = SSL_SIG_RSA;
#endif
#if defined(POLARSSL_MD5_C)
    sig_alg_list[sig_alg_len++] = SSL_HASH_MD5;
    sig_alg_list[sig_alg_len++] = SSL_SIG_RSA;
#endif
#endif /* POLARSSL_RSA_C */
#if defined(POLARSSL_ECDSA_C)
#if defined(POLARSSL_SHA512_C)
    sig_alg_list[sig_alg_len++] = SSL_HASH_SHA512;
    sig_alg_list[sig_alg_len++] = SSL_SIG_ECDSA;
    sig_alg_list[sig_alg_len++] = SSL_HASH_SHA384;
    sig_alg_list[sig_alg_len++] = SSL_SIG_ECDSA;
#endif
#if defined(POLARSSL_SHA256_C)
    sig_alg_list[sig_alg_len++] = SSL_HASH_SHA256;
    sig_alg_list[sig_alg_len++] = SSL_SIG_ECDSA;
    sig_alg_list[sig_alg_len++] = SSL_HASH_SHA224;
    sig_alg_list[sig_alg_len++] = SSL_SIG_ECDSA;
#endif
#if defined(POLARSSL_SHA1_C)
    sig_alg_list[sig_alg_len++] = SSL_HASH_SHA1;
    sig_alg_list[sig_alg_len++] = SSL_SIG_ECDSA;
#endif
#if defined(POLARSSL_MD5_C)
    sig_alg_list[sig_alg_len++] = SSL_HASH_MD5;
    sig_alg_list[sig_alg_len++] = SSL_SIG_ECDSA;
#endif
#endif /* POLARSSL_ECDSA_C */

    /*
     * enum {
     *     none(0), md5(1), sha1(2), sha224(3), sha256(4), sha384(5),
     *     sha512(6), (255)
     * } HashAlgorithm;
     *
     * enum { anonymous(0), rsa(1), dsa(2), ecdsa(3), (255) }
     *   SignatureAlgorithm;
     *
     * struct {
     *     HashAlgorithm hash;
     *     SignatureAlgorithm signature;
     * } SignatureAndHashAlgorithm;
     *
     * SignatureAndHashAlgorithm
     *   supported_signature_algorithms<2..2^16-2>;
     */
    *p++ = (unsigned char)( ( TLS_EXT_SIG_ALG >> 8 ) & 0xFF );
    *p++ = (unsigned char)( ( TLS_EXT_SIG_ALG      ) & 0xFF );

    *p++ = (unsigned char)( ( ( sig_alg_len + 2 ) >> 8 ) & 0xFF );
    *p++ = (unsigned char)( ( ( sig_alg_len + 2 )      ) & 0xFF );

    *p++ = (unsigned char)( ( sig_alg_len >> 8 ) & 0xFF );
    *p++ = (unsigned char)( ( sig_alg_len      ) & 0xFF );

    *olen = 6 + sig_alg_len;
}