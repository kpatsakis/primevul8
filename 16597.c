static pyc_object *get_object(RzBinPycObj *pyc, RzBuffer *buffer) {
	bool error = false;
	pyc_object *ret = NULL;
	ut8 code = get_ut8(buffer, &error);
	ut8 flag = code & FLAG_REF;
	RzListIter *ref_idx = NULL;
	ut8 type = code & ~FLAG_REF;

	if (error) {
		return NULL;
	}

	if (flag) {
		ret = get_none_object();
		if (!ret) {
			return NULL;
		}
		ref_idx = rz_list_append(pyc->refs, ret);
		if (!ref_idx) {
			free_object(ret);
			return NULL;
		}
	}

	switch (type) {
	case TYPE_NULL:
		return NULL;
	case TYPE_TRUE:
		return get_true_object();
	case TYPE_FALSE:
		return get_false_object();
	case TYPE_NONE:
		return get_none_object();
	case TYPE_REF:
		return get_ref_object(pyc, buffer);
	case TYPE_SMALL_TUPLE:
		ret = get_small_tuple_object(pyc, buffer);
		break;
	case TYPE_TUPLE:
		ret = get_tuple_object(pyc, buffer);
		break;
	case TYPE_STRING:
		ret = get_string_object(buffer);
		break;
	case TYPE_CODE_v0:
		ret = get_code_object(pyc, buffer);
		if (ret) {
			ret->type = TYPE_CODE_v0;
		}
		break;
	case TYPE_CODE_v1:
		ret = get_code_object(pyc, buffer);
		if (ret) {
			ret->type = TYPE_CODE_v1;
		}
		break;
	case TYPE_INT:
		ret = get_int_object(buffer);
		break;
	case TYPE_ASCII_INTERNED:
		ret = get_ascii_interned_object(buffer);
		break;
	case TYPE_SHORT_ASCII:
		ret = get_short_ascii_object(buffer);
		break;
	case TYPE_ASCII:
		ret = get_ascii_object(buffer);
		break;
	case TYPE_SHORT_ASCII_INTERNED:
		ret = get_short_ascii_interned_object(buffer);
		break;
	case TYPE_INT64:
		ret = get_int64_object(buffer);
		break;
	case TYPE_INTERNED:
		ret = get_interned_object(pyc, buffer);
		break;
	case TYPE_STRINGREF:
		ret = get_stringref_object(pyc, buffer);
		break;
	case TYPE_FLOAT:
		ret = get_float_object(buffer);
		break;
	case TYPE_BINARY_FLOAT:
		ret = get_binary_float_object(buffer);
		break;
	case TYPE_COMPLEX:
		ret = get_complex_object(pyc, buffer); // behaviour depends on Python version
		break;
	case TYPE_BINARY_COMPLEX:
		ret = get_binary_complex_object(buffer);
		break;
	case TYPE_LIST:
		ret = get_list_object(pyc, buffer);
		break;
	case TYPE_LONG:
		ret = get_long_object(buffer);
		break;
	case TYPE_UNICODE:
		ret = get_unicode_object(buffer);
		break;
	case TYPE_DICT:
		ret = get_dict_object(pyc, buffer);
		break;
	case TYPE_FROZENSET:
	case TYPE_SET:
		ret = get_set_object(pyc, buffer);
		break;
	case TYPE_STOPITER:
		ret = RZ_NEW0(pyc_object);
		break;
	case TYPE_ELLIPSIS:
		ret = RZ_NEW0(pyc_object);
		break;
	case TYPE_UNKNOWN:
		RZ_LOG_ERROR("Get not implemented for type 0x%x\n", type);
		return NULL;
	default:
		RZ_LOG_ERROR("Undefined type in get_object (0x%x)\n", type);
		return NULL;
	}

	if (flag && ref_idx) {
		free_object(ref_idx->data);
		ref_idx->data = copy_object(ret);
	}
	return ret;
}