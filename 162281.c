void tcp_parse_options(const struct sk_buff *skb,
		       struct tcp_options_received *opt_rx, int estab,
		       struct tcp_fastopen_cookie *foc)
{
	const unsigned char *ptr;
	const struct tcphdr *th = tcp_hdr(skb);
	int length = (th->doff * 4) - sizeof(struct tcphdr);

	ptr = (const unsigned char *)(th + 1);
	opt_rx->saw_tstamp = 0;

	while (length > 0) {
		int opcode = *ptr++;
		int opsize;

		switch (opcode) {
		case TCPOPT_EOL:
			return;
		case TCPOPT_NOP:	/* Ref: RFC 793 section 3.1 */
			length--;
			continue;
		default:
			opsize = *ptr++;
			if (opsize < 2) /* "silly options" */
				return;
			if (opsize > length)
				return;	/* don't parse partial options */
			switch (opcode) {
			case TCPOPT_MSS:
				if (opsize == TCPOLEN_MSS && th->syn && !estab) {
					u16 in_mss = get_unaligned_be16(ptr);
					if (in_mss) {
						if (opt_rx->user_mss &&
						    opt_rx->user_mss < in_mss)
							in_mss = opt_rx->user_mss;
						opt_rx->mss_clamp = in_mss;
					}
				}
				break;
			case TCPOPT_WINDOW:
				if (opsize == TCPOLEN_WINDOW && th->syn &&
				    !estab && sysctl_tcp_window_scaling) {
					__u8 snd_wscale = *(__u8 *)ptr;
					opt_rx->wscale_ok = 1;
					if (snd_wscale > 14) {
						net_info_ratelimited("%s: Illegal window scaling value %d >14 received\n",
								     __func__,
								     snd_wscale);
						snd_wscale = 14;
					}
					opt_rx->snd_wscale = snd_wscale;
				}
				break;
			case TCPOPT_TIMESTAMP:
				if ((opsize == TCPOLEN_TIMESTAMP) &&
				    ((estab && opt_rx->tstamp_ok) ||
				     (!estab && sysctl_tcp_timestamps))) {
					opt_rx->saw_tstamp = 1;
					opt_rx->rcv_tsval = get_unaligned_be32(ptr);
					opt_rx->rcv_tsecr = get_unaligned_be32(ptr + 4);
				}
				break;
			case TCPOPT_SACK_PERM:
				if (opsize == TCPOLEN_SACK_PERM && th->syn &&
				    !estab && sysctl_tcp_sack) {
					opt_rx->sack_ok = TCP_SACK_SEEN;
					tcp_sack_reset(opt_rx);
				}
				break;

			case TCPOPT_SACK:
				if ((opsize >= (TCPOLEN_SACK_BASE + TCPOLEN_SACK_PERBLOCK)) &&
				   !((opsize - TCPOLEN_SACK_BASE) % TCPOLEN_SACK_PERBLOCK) &&
				   opt_rx->sack_ok) {
					TCP_SKB_CB(skb)->sacked = (ptr - 2) - (unsigned char *)th;
				}
				break;
#ifdef CONFIG_TCP_MD5SIG
			case TCPOPT_MD5SIG:
				/*
				 * The MD5 Hash has already been
				 * checked (see tcp_v{4,6}_do_rcv()).
				 */
				break;
#endif
			case TCPOPT_FASTOPEN:
				tcp_parse_fastopen_option(
					opsize - TCPOLEN_FASTOPEN_BASE,
					ptr, th->syn, foc, false);
				break;

			case TCPOPT_EXP:
				/* Fast Open option shares code 254 using a
				 * 16 bits magic number.
				 */
				if (opsize >= TCPOLEN_EXP_FASTOPEN_BASE &&
				    get_unaligned_be16(ptr) ==
				    TCPOPT_FASTOPEN_MAGIC)
					tcp_parse_fastopen_option(opsize -
						TCPOLEN_EXP_FASTOPEN_BASE,
						ptr + 2, th->syn, foc, true);
				break;

			}
			ptr += opsize-2;
			length -= opsize;
		}
	}
}