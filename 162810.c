asn1_decode_simple_ber (unsigned int etype, const unsigned char *der,
			unsigned int _der_len, unsigned char **str,
			unsigned int *str_len, unsigned int *ber_len)
{
  int tag_len, len_len;
  const unsigned char *p;
  int der_len = _der_len;
  uint8_t *total = NULL;
  unsigned total_size = 0;
  unsigned char class;
  unsigned long tag;
  unsigned char *out = NULL;
  unsigned out_len;
  long ret;

  if (ber_len) *ber_len = 0;

  if (der == NULL || der_len == 0)
    {
      warn();
      return ASN1_VALUE_NOT_VALID;
    }

  if (ETYPE_OK (etype) == 0)
    {
      warn();
      return ASN1_VALUE_NOT_VALID;
    }

  /* doesn't handle constructed classes */
  if (ETYPE_CLASS (etype) != ASN1_CLASS_UNIVERSAL)
    {
      warn();
      return ASN1_VALUE_NOT_VALID;
    }

  p = der;
  ret = asn1_get_tag_der (p, der_len, &class, &tag_len, &tag);
  if (ret != ASN1_SUCCESS)
    {
      warn();
      return ret;
    }

  if (ber_len) *ber_len += tag_len;

  if (tag != ETYPE_TAG (etype))
    {
      warn();
      return ASN1_DER_ERROR;
    }

  p += tag_len;
  der_len -= tag_len;
  if (der_len <= 0)
    return ASN1_DER_ERROR;

  if (class == ASN1_CLASS_STRUCTURED && ETYPE_IS_STRING(etype))
    {

      len_len = 1;
      if (p[0] != 0x80)
        {
          warn();
          return ASN1_DER_ERROR;
        }

      p += len_len;
      der_len -= len_len;
      if (der_len <= 0)
        return ASN1_DER_ERROR;

      if (ber_len) *ber_len += len_len;

      /* decode the available octet strings */
      do
        {
          unsigned tmp_len;

          ret = asn1_decode_simple_ber(etype, p, der_len, &out, &out_len, &tmp_len);
          if (ret != ASN1_SUCCESS)
            {
              free(total);
              return ret;
            }
          p += tmp_len;
          der_len -= tmp_len;
          if (ber_len) *ber_len += tmp_len;

          if (der_len < 2) /* we need the EOC */
            {
              free(total);
              return ASN1_DER_ERROR;
            }

	  if (out_len > 0)
	    {
              ret = append(&total, &total_size, out, out_len);
              free(out);
              if (ret != ASN1_SUCCESS)
                {
                  free(total);
                  return ret;
                }
	    }

	  if (p[0] == 0 && p[1] == 0) /* EOC */
	    {
              if (ber_len) *ber_len += 2;
  	      break;
  	    }
        }
      while(1);
    }
  else if (class == ETYPE_CLASS(etype))
    {
      if (ber_len)
        {
          ret = asn1_get_length_der (p, der_len, &len_len);
          if (ret < 0)
            {
              warn();
              return ASN1_DER_ERROR;
            }
          *ber_len += ret + len_len;
        }

      /* non-string values are decoded as DER */
      ret = asn1_decode_simple_der(etype, der, _der_len, (const unsigned char**)&out, &out_len);
      if (ret != ASN1_SUCCESS)
        return ret;

      ret = append(&total, &total_size, out, out_len);
      if (ret != ASN1_SUCCESS)
        return ret;
    }
  else
    return ASN1_DER_ERROR;

  *str = total;
  *str_len = total_size;

  return ASN1_SUCCESS;
}