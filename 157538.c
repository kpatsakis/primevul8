do_curl_traverse(CurlObject *self, visitproc visit, void *arg)
{
    int err;
#undef VISIT
#define VISIT(v)    if ((v) != NULL && ((err = visit(v, arg)) != 0)) return err

    VISIT(self->dict);
    VISIT((PyObject *) self->multi_stack);
    VISIT((PyObject *) self->share);

    VISIT(self->w_cb);
    VISIT(self->h_cb);
    VISIT(self->r_cb);
    VISIT(self->pro_cb);
    VISIT(self->debug_cb);
    VISIT(self->ioctl_cb);
    VISIT(self->opensocket_cb);
    VISIT(self->seek_cb);

    VISIT(self->readdata_fp);
    VISIT(self->writedata_fp);
    VISIT(self->writeheader_fp);
    
    VISIT(self->postfields_obj);

    return 0;
#undef VISIT
}