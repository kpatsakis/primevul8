static int put_compat_request_table(struct compat_sg_req_info __user *o,
				    struct sg_req_info *rinfo)
{
	int i;
	for (i = 0; i < SG_MAX_QUEUE; i++) {
		if (copy_to_user(o + i, rinfo + i, offsetof(sg_req_info_t, usr_ptr)) ||
		    put_user((uintptr_t)rinfo[i].usr_ptr, &o[i].usr_ptr) ||
		    put_user(rinfo[i].duration, &o[i].duration) ||
		    put_user(rinfo[i].unused, &o[i].unused))
			return -EFAULT;
	}
	return 0;
}