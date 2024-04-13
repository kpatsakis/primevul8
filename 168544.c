ncp_read_bounce(struct ncp_server *server, const char *file_id,
	 __u32 offset, __u16 to_read, struct iov_iter *to,
	 int *bytes_read, void *bounce, __u32 bufsize)
{
	int result;

	ncp_init_request(server);
	ncp_add_byte(server, 0);
	ncp_add_mem(server, file_id, 6);
	ncp_add_be32(server, offset);
	ncp_add_be16(server, to_read);
	result = ncp_request2(server, 72, bounce, bufsize);
	ncp_unlock_server(server);
	if (!result) {
		int len = get_unaligned_be16((char *)bounce +
			  sizeof(struct ncp_reply_header));
		result = -EIO;
		if (len <= to_read) {
			char* source;

			source = (char*)bounce + 
			         sizeof(struct ncp_reply_header) + 2 + 
			         (offset & 1);
			*bytes_read = len;
			result = 0;
			if (copy_to_iter(source, len, to) != len)
				result = -EFAULT;
		}
	}
	return result;
}