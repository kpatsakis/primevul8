static PyObject *convert_certinfo(struct curl_certinfo *cinfo)
{
    PyObject *certs;
    int cert_index;

    if (!cinfo)
        Py_RETURN_NONE;

    certs = PyList_New((Py_ssize_t)(cinfo->num_of_certs));
    if (!certs)
        return NULL;

    for (cert_index = 0; cert_index < cinfo->num_of_certs; cert_index ++) {
        struct curl_slist *fields = cinfo->certinfo[cert_index];
        struct curl_slist *field_cursor;
        int field_count, field_index;
        PyObject *cert;

        field_count = 0;
        field_cursor = fields;
        while (field_cursor != NULL) {
            field_cursor = field_cursor->next;
            field_count ++;
        }

        
        cert = PyTuple_New((Py_ssize_t)field_count);
        if (!cert)
            goto error;
        PyList_SetItem(certs, cert_index, cert); /* Eats the ref from New() */
        
        for(field_index = 0, field_cursor = fields;
            field_cursor != NULL;
            field_index ++, field_cursor = field_cursor->next) {
            const char *field = field_cursor->data;
            PyObject *field_tuple;

            if (!field) {
                field_tuple = Py_None; Py_INCREF(field_tuple);
            } else {
                const char *sep = strchr(field, ':');
                if (!sep) {
                    field_tuple = PyText_FromString(field);
                } else {
                    /* XXX check */
                    field_tuple = Py_BuildValue("s#s", field, (int)(sep - field), sep+1);
                }
                if (!field_tuple)
                    goto error;
            }
            PyTuple_SET_ITEM(cert, field_index, field_tuple); /* Eats the ref */
        }
    }

    return certs;
    
 error:
    Py_DECREF(certs);
    return NULL;
}