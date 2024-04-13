asn1_write_value (asn1_node node_root, const char *name,
		  const void *ivalue, int len)
{
  asn1_node node, p, p2;
  unsigned char *temp, *value_temp = NULL, *default_temp = NULL;
  int len2, k, k2, negative;
  size_t i;
  const unsigned char *value = ivalue;
  unsigned int type;

  node = asn1_find_node (node_root, name);
  if (node == NULL)
    return ASN1_ELEMENT_NOT_FOUND;

  if ((node->type & CONST_OPTION) && (value == NULL) && (len == 0))
    {
      asn1_delete_structure (&node);
      return ASN1_SUCCESS;
    }

  type = type_field (node->type);

  if ((type == ASN1_ETYPE_SEQUENCE_OF || type == ASN1_ETYPE_SET_OF) && (value == NULL) && (len == 0))
    {
      p = node->down;
      while ((type_field (p->type) == ASN1_ETYPE_TAG)
	     || (type_field (p->type) == ASN1_ETYPE_SIZE))
	p = p->right;

      while (p->right)
	asn1_delete_structure (&p->right);

      return ASN1_SUCCESS;
    }

  /* Don't allow element deletion for other types */
  if (value == NULL)
    {
      return ASN1_VALUE_NOT_VALID;
    }

  switch (type)
    {
    case ASN1_ETYPE_BOOLEAN:
      if (!_asn1_strcmp (value, "TRUE"))
	{
	  if (node->type & CONST_DEFAULT)
	    {
	      p = node->down;
	      while (type_field (p->type) != ASN1_ETYPE_DEFAULT)
		p = p->right;
	      if (p->type & CONST_TRUE)
		_asn1_set_value (node, NULL, 0);
	      else
		_asn1_set_value (node, "T", 1);
	    }
	  else
	    _asn1_set_value (node, "T", 1);
	}
      else if (!_asn1_strcmp (value, "FALSE"))
	{
	  if (node->type & CONST_DEFAULT)
	    {
	      p = node->down;
	      while (type_field (p->type) != ASN1_ETYPE_DEFAULT)
		p = p->right;
	      if (p->type & CONST_FALSE)
		_asn1_set_value (node, NULL, 0);
	      else
		_asn1_set_value (node, "F", 1);
	    }
	  else
	    _asn1_set_value (node, "F", 1);
	}
      else
	return ASN1_VALUE_NOT_VALID;
      break;
    case ASN1_ETYPE_INTEGER:
    case ASN1_ETYPE_ENUMERATED:
      if (len == 0)
	{
	  if ((isdigit (value[0])) || (value[0] == '-'))
	    {
	      value_temp = malloc (SIZEOF_UNSIGNED_LONG_INT);
	      if (value_temp == NULL)
		return ASN1_MEM_ALLOC_ERROR;

	      _asn1_convert_integer (value, value_temp,
				     SIZEOF_UNSIGNED_LONG_INT, &len);
	    }
	  else
	    {			/* is an identifier like v1 */
	      if (!(node->type & CONST_LIST))
		return ASN1_VALUE_NOT_VALID;
	      p = node->down;
	      while (p)
		{
		  if (type_field (p->type) == ASN1_ETYPE_CONSTANT)
		    {
		      if (!_asn1_strcmp (p->name, value))
			{
			  value_temp = malloc (SIZEOF_UNSIGNED_LONG_INT);
			  if (value_temp == NULL)
			    return ASN1_MEM_ALLOC_ERROR;

			  _asn1_convert_integer (p->value,
						 value_temp,
						 SIZEOF_UNSIGNED_LONG_INT,
						 &len);
			  break;
			}
		    }
		  p = p->right;
		}
	      if (p == NULL)
		return ASN1_VALUE_NOT_VALID;
	    }
	}
      else
	{			/* len != 0 */
	  value_temp = malloc (len);
	  if (value_temp == NULL)
	    return ASN1_MEM_ALLOC_ERROR;
	  memcpy (value_temp, value, len);
	}

      if (value_temp[0] & 0x80)
	negative = 1;
      else
	negative = 0;

      if (negative && (type_field (node->type) == ASN1_ETYPE_ENUMERATED))
	{
	  free (value_temp);
	  return ASN1_VALUE_NOT_VALID;
	}

      for (k = 0; k < len - 1; k++)
	if (negative && (value_temp[k] != 0xFF))
	  break;
	else if (!negative && value_temp[k])
	  break;

      if ((negative && !(value_temp[k] & 0x80)) ||
	  (!negative && (value_temp[k] & 0x80)))
	k--;

      _asn1_set_value_lv (node, value_temp + k, len - k);

      if (node->type & CONST_DEFAULT)
	{
	  p = node->down;
	  while (type_field (p->type) != ASN1_ETYPE_DEFAULT)
	    p = p->right;
	  if ((isdigit (p->value[0])) || (p->value[0] == '-'))
	    {
	      default_temp = malloc (SIZEOF_UNSIGNED_LONG_INT);
	      if (default_temp == NULL)
		{
		  free (value_temp);
		  return ASN1_MEM_ALLOC_ERROR;
		}

	      _asn1_convert_integer (p->value, default_temp,
				     SIZEOF_UNSIGNED_LONG_INT, &len2);
	    }
	  else
	    {			/* is an identifier like v1 */
	      if (!(node->type & CONST_LIST))
		{
		  free (value_temp);
		  return ASN1_VALUE_NOT_VALID;
		}
	      p2 = node->down;
	      while (p2)
		{
		  if (type_field (p2->type) == ASN1_ETYPE_CONSTANT)
		    {
		      if (!_asn1_strcmp (p2->name, p->value))
			{
			  default_temp = malloc (SIZEOF_UNSIGNED_LONG_INT);
			  if (default_temp == NULL)
			    {
			      free (value_temp);
			      return ASN1_MEM_ALLOC_ERROR;
			    }

			  _asn1_convert_integer (p2->value,
						 default_temp,
						 SIZEOF_UNSIGNED_LONG_INT,
						 &len2);
			  break;
			}
		    }
		  p2 = p2->right;
		}
	      if (p2 == NULL)
		{
		  free (value_temp);
		  return ASN1_VALUE_NOT_VALID;
		}
	    }


	  if ((len - k) == len2)
	    {
	      for (k2 = 0; k2 < len2; k2++)
		if (value_temp[k + k2] != default_temp[k2])
		  {
		    break;
		  }
	      if (k2 == len2)
		_asn1_set_value (node, NULL, 0);
	    }
	  free (default_temp);
	}
      free (value_temp);
      break;
    case ASN1_ETYPE_OBJECT_ID:
      for (i = 0; i < _asn1_strlen (value); i++)
	if ((!isdigit (value[i])) && (value[i] != '.') && (value[i] != '+'))
	  return ASN1_VALUE_NOT_VALID;
      if (node->type & CONST_DEFAULT)
	{
	  p = node->down;
	  while (type_field (p->type) != ASN1_ETYPE_DEFAULT)
	    p = p->right;
	  if (!_asn1_strcmp (value, p->value))
	    {
	      _asn1_set_value (node, NULL, 0);
	      break;
	    }
	}
      _asn1_set_value (node, value, _asn1_strlen (value) + 1);
      break;
    case ASN1_ETYPE_UTC_TIME:
      {
	len = _asn1_strlen (value);
	if (len < 11)
	  return ASN1_VALUE_NOT_VALID;
	for (k = 0; k < 10; k++)
	  if (!isdigit (value[k]))
	    return ASN1_VALUE_NOT_VALID;
	switch (len)
	  {
	  case 11:
	    if (value[10] != 'Z')
	      return ASN1_VALUE_NOT_VALID;
	    break;
	  case 13:
	    if ((!isdigit (value[10])) || (!isdigit (value[11])) ||
		(value[12] != 'Z'))
	      return ASN1_VALUE_NOT_VALID;
	    break;
	  case 15:
	    if ((value[10] != '+') && (value[10] != '-'))
	      return ASN1_VALUE_NOT_VALID;
	    for (k = 11; k < 15; k++)
	      if (!isdigit (value[k]))
		return ASN1_VALUE_NOT_VALID;
	    break;
	  case 17:
	    if ((!isdigit (value[10])) || (!isdigit (value[11])))
	      return ASN1_VALUE_NOT_VALID;
	    if ((value[12] != '+') && (value[12] != '-'))
	      return ASN1_VALUE_NOT_VALID;
	    for (k = 13; k < 17; k++)
	      if (!isdigit (value[k]))
		return ASN1_VALUE_NOT_VALID;
	    break;
	  default:
	    return ASN1_VALUE_NOT_FOUND;
	  }
	_asn1_set_value (node, value, len);
      }
      break;
    case ASN1_ETYPE_GENERALIZED_TIME:
      len = _asn1_strlen (value);
      _asn1_set_value (node, value, len);
      break;
    case ASN1_ETYPE_OCTET_STRING:
    case ASN1_ETYPE_GENERALSTRING:
    case ASN1_ETYPE_NUMERIC_STRING:
    case ASN1_ETYPE_IA5_STRING:
    case ASN1_ETYPE_TELETEX_STRING:
    case ASN1_ETYPE_PRINTABLE_STRING:
    case ASN1_ETYPE_UNIVERSAL_STRING:
    case ASN1_ETYPE_BMP_STRING:
    case ASN1_ETYPE_UTF8_STRING:
    case ASN1_ETYPE_VISIBLE_STRING:
      if (len == 0)
	len = _asn1_strlen (value);
      _asn1_set_value_lv (node, value, len);
      break;
    case ASN1_ETYPE_BIT_STRING:
      if (len == 0)
	len = _asn1_strlen (value);
      asn1_length_der ((len >> 3) + 2, NULL, &len2);
      temp = malloc ((len >> 3) + 2 + len2);
      if (temp == NULL)
	return ASN1_MEM_ALLOC_ERROR;

      asn1_bit_der (value, len, temp, &len2);
      _asn1_set_value_m (node, temp, len2);
      temp = NULL;
      break;
    case ASN1_ETYPE_CHOICE:
      p = node->down;
      while (p)
	{
	  if (!_asn1_strcmp (p->name, value))
	    {
	      p2 = node->down;
	      while (p2)
		{
		  if (p2 != p)
		    {
		      asn1_delete_structure (&p2);
		      p2 = node->down;
		    }
		  else
		    p2 = p2->right;
		}
	      break;
	    }
	  p = p->right;
	}
      if (!p)
	return ASN1_ELEMENT_NOT_FOUND;
      break;
    case ASN1_ETYPE_ANY:
      _asn1_set_value_lv (node, value, len);
      break;
    case ASN1_ETYPE_SEQUENCE_OF:
    case ASN1_ETYPE_SET_OF:
      if (_asn1_strcmp (value, "NEW"))
	return ASN1_VALUE_NOT_VALID;
      _asn1_append_sequence_set (node);
      break;
    default:
      return ASN1_ELEMENT_NOT_FOUND;
      break;
    }

  return ASN1_SUCCESS;
}