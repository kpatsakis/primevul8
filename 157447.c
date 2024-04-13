static bool bnxt_re_is_vlan_pkt(struct bnxt_qplib_cqe *orig_cqe,
				u16 *vid, u8 *sl)
{
	bool ret = false;
	u32 metadata;
	u16 tpid;

	metadata = orig_cqe->raweth_qp1_metadata;
	if (orig_cqe->raweth_qp1_flags2 &
		CQ_RES_RAWETH_QP1_RAWETH_QP1_FLAGS2_META_FORMAT_VLAN) {
		tpid = ((metadata &
			 CQ_RES_RAWETH_QP1_RAWETH_QP1_METADATA_TPID_MASK) >>
			 CQ_RES_RAWETH_QP1_RAWETH_QP1_METADATA_TPID_SFT);
		if (tpid == ETH_P_8021Q) {
			*vid = metadata &
			       CQ_RES_RAWETH_QP1_RAWETH_QP1_METADATA_VID_MASK;
			*sl = (metadata &
			       CQ_RES_RAWETH_QP1_RAWETH_QP1_METADATA_PRI_MASK) >>
			       CQ_RES_RAWETH_QP1_RAWETH_QP1_METADATA_PRI_SFT;
			ret = true;
		}
	}

	return ret;
}