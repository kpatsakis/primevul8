smp_fetch_hdr_ip(struct proxy *px, struct session *l4, void *l7, unsigned int opt,
                 const struct arg *args, struct sample *smp)
{
	int ret;

	while ((ret = smp_fetch_hdr(px, l4, l7, opt, args, smp)) > 0) {
		if (url2ipv4((char *)smp->data.str.str, &smp->data.ipv4)) {
			smp->type = SMP_T_IPV4;
			break;
		} else {
			struct chunk *temp = get_trash_chunk();
			if (smp->data.str.len < temp->size - 1) {
				memcpy(temp->str, smp->data.str.str, smp->data.str.len);
				temp->str[smp->data.str.len] = '\0';
				if (inet_pton(AF_INET6, temp->str, &smp->data.ipv6)) {
					smp->type = SMP_T_IPV6;
					break;
				}
			}
		}

		/* if the header doesn't match an IP address, fetch next one */
		if (!(smp->flags & SMP_F_NOT_LAST))
			return 0;
	}
	return ret;
}