static int bnxt_re_build_qp1_send_v2(struct bnxt_re_qp *qp,
				     const struct ib_send_wr *wr,
				     struct bnxt_qplib_swqe *wqe,
				     int payload_size)
{
	struct bnxt_re_ah *ah = container_of(ud_wr(wr)->ah, struct bnxt_re_ah,
					     ib_ah);
	struct bnxt_qplib_ah *qplib_ah = &ah->qplib_ah;
	const struct ib_gid_attr *sgid_attr = ah->ib_ah.sgid_attr;
	struct bnxt_qplib_sge sge;
	u8 nw_type;
	u16 ether_type;
	union ib_gid dgid;
	bool is_eth = false;
	bool is_vlan = false;
	bool is_grh = false;
	bool is_udp = false;
	u8 ip_version = 0;
	u16 vlan_id = 0xFFFF;
	void *buf;
	int i, rc = 0;

	memset(&qp->qp1_hdr, 0, sizeof(qp->qp1_hdr));

	rc = rdma_read_gid_l2_fields(sgid_attr, &vlan_id, NULL);
	if (rc)
		return rc;

	/* Get network header type for this GID */
	nw_type = rdma_gid_attr_network_type(sgid_attr);
	switch (nw_type) {
	case RDMA_NETWORK_IPV4:
		nw_type = BNXT_RE_ROCEV2_IPV4_PACKET;
		break;
	case RDMA_NETWORK_IPV6:
		nw_type = BNXT_RE_ROCEV2_IPV6_PACKET;
		break;
	default:
		nw_type = BNXT_RE_ROCE_V1_PACKET;
		break;
	}
	memcpy(&dgid.raw, &qplib_ah->dgid, 16);
	is_udp = sgid_attr->gid_type == IB_GID_TYPE_ROCE_UDP_ENCAP;
	if (is_udp) {
		if (ipv6_addr_v4mapped((struct in6_addr *)&sgid_attr->gid)) {
			ip_version = 4;
			ether_type = ETH_P_IP;
		} else {
			ip_version = 6;
			ether_type = ETH_P_IPV6;
		}
		is_grh = false;
	} else {
		ether_type = ETH_P_IBOE;
		is_grh = true;
	}

	is_eth = true;
	is_vlan = (vlan_id && (vlan_id < 0x1000)) ? true : false;

	ib_ud_header_init(payload_size, !is_eth, is_eth, is_vlan, is_grh,
			  ip_version, is_udp, 0, &qp->qp1_hdr);

	/* ETH */
	ether_addr_copy(qp->qp1_hdr.eth.dmac_h, ah->qplib_ah.dmac);
	ether_addr_copy(qp->qp1_hdr.eth.smac_h, qp->qplib_qp.smac);

	/* For vlan, check the sgid for vlan existence */

	if (!is_vlan) {
		qp->qp1_hdr.eth.type = cpu_to_be16(ether_type);
	} else {
		qp->qp1_hdr.vlan.type = cpu_to_be16(ether_type);
		qp->qp1_hdr.vlan.tag = cpu_to_be16(vlan_id);
	}

	if (is_grh || (ip_version == 6)) {
		memcpy(qp->qp1_hdr.grh.source_gid.raw, sgid_attr->gid.raw,
		       sizeof(sgid_attr->gid));
		memcpy(qp->qp1_hdr.grh.destination_gid.raw, qplib_ah->dgid.data,
		       sizeof(sgid_attr->gid));
		qp->qp1_hdr.grh.hop_limit     = qplib_ah->hop_limit;
	}

	if (ip_version == 4) {
		qp->qp1_hdr.ip4.tos = 0;
		qp->qp1_hdr.ip4.id = 0;
		qp->qp1_hdr.ip4.frag_off = htons(IP_DF);
		qp->qp1_hdr.ip4.ttl = qplib_ah->hop_limit;

		memcpy(&qp->qp1_hdr.ip4.saddr, sgid_attr->gid.raw + 12, 4);
		memcpy(&qp->qp1_hdr.ip4.daddr, qplib_ah->dgid.data + 12, 4);
		qp->qp1_hdr.ip4.check = ib_ud_ip4_csum(&qp->qp1_hdr);
	}

	if (is_udp) {
		qp->qp1_hdr.udp.dport = htons(ROCE_V2_UDP_DPORT);
		qp->qp1_hdr.udp.sport = htons(0x8CD1);
		qp->qp1_hdr.udp.csum = 0;
	}

	/* BTH */
	if (wr->opcode == IB_WR_SEND_WITH_IMM) {
		qp->qp1_hdr.bth.opcode = IB_OPCODE_UD_SEND_ONLY_WITH_IMMEDIATE;
		qp->qp1_hdr.immediate_present = 1;
	} else {
		qp->qp1_hdr.bth.opcode = IB_OPCODE_UD_SEND_ONLY;
	}
	if (wr->send_flags & IB_SEND_SOLICITED)
		qp->qp1_hdr.bth.solicited_event = 1;
	/* pad_count */
	qp->qp1_hdr.bth.pad_count = (4 - payload_size) & 3;

	/* P_key for QP1 is for all members */
	qp->qp1_hdr.bth.pkey = cpu_to_be16(0xFFFF);
	qp->qp1_hdr.bth.destination_qpn = IB_QP1;
	qp->qp1_hdr.bth.ack_req = 0;
	qp->send_psn++;
	qp->send_psn &= BTH_PSN_MASK;
	qp->qp1_hdr.bth.psn = cpu_to_be32(qp->send_psn);
	/* DETH */
	/* Use the priviledged Q_Key for QP1 */
	qp->qp1_hdr.deth.qkey = cpu_to_be32(IB_QP1_QKEY);
	qp->qp1_hdr.deth.source_qpn = IB_QP1;

	/* Pack the QP1 to the transmit buffer */
	buf = bnxt_qplib_get_qp1_sq_buf(&qp->qplib_qp, &sge);
	if (buf) {
		ib_ud_header_pack(&qp->qp1_hdr, buf);
		for (i = wqe->num_sge; i; i--) {
			wqe->sg_list[i].addr = wqe->sg_list[i - 1].addr;
			wqe->sg_list[i].lkey = wqe->sg_list[i - 1].lkey;
			wqe->sg_list[i].size = wqe->sg_list[i - 1].size;
		}

		/*
		 * Max Header buf size for IPV6 RoCE V2 is 86,
		 * which is same as the QP1 SQ header buffer.
		 * Header buf size for IPV4 RoCE V2 can be 66.
		 * ETH(14) + VLAN(4)+ IP(20) + UDP (8) + BTH(20).
		 * Subtract 20 bytes from QP1 SQ header buf size
		 */
		if (is_udp && ip_version == 4)
			sge.size -= 20;
		/*
		 * Max Header buf size for RoCE V1 is 78.
		 * ETH(14) + VLAN(4) + GRH(40) + BTH(20).
		 * Subtract 8 bytes from QP1 SQ header buf size
		 */
		if (!is_udp)
			sge.size -= 8;

		/* Subtract 4 bytes for non vlan packets */
		if (!is_vlan)
			sge.size -= 4;

		wqe->sg_list[0].addr = sge.addr;
		wqe->sg_list[0].lkey = sge.lkey;
		wqe->sg_list[0].size = sge.size;
		wqe->num_sge++;

	} else {
		dev_err(rdev_to_dev(qp->rdev), "QP1 buffer is empty!");
		rc = -ENOMEM;
	}
	return rc;
}