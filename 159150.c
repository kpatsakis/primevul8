void rtl_p2p_ps_cmd(struct ieee80211_hw *hw , u8 p2p_ps_state)
{
	struct rtl_priv *rtlpriv = rtl_priv(hw);
	struct rtl_ps_ctl *rtlps = rtl_psc(rtl_priv(hw));
	struct rtl_p2p_ps_info  *p2pinfo = &(rtlpriv->psc.p2p_ps_info);

	RT_TRACE(rtlpriv, COMP_FW, DBG_LOUD, " p2p state %x\n" , p2p_ps_state);
	switch (p2p_ps_state) {
	case P2P_PS_DISABLE:
		p2pinfo->p2p_ps_state = p2p_ps_state;
		rtlpriv->cfg->ops->set_hw_reg(hw, HW_VAR_H2C_FW_P2P_PS_OFFLOAD,
					      &p2p_ps_state);
		p2pinfo->noa_index = 0;
		p2pinfo->ctwindow = 0;
		p2pinfo->opp_ps = 0;
		p2pinfo->noa_num = 0;
		p2pinfo->p2p_ps_mode = P2P_PS_NONE;
		if (rtlps->fw_current_inpsmode) {
			if (rtlps->smart_ps == 0) {
				rtlps->smart_ps = 2;
				rtlpriv->cfg->ops->set_hw_reg(hw,
					 HW_VAR_H2C_FW_PWRMODE,
					 &rtlps->pwr_mode);
			}

		}
		break;
	case P2P_PS_ENABLE:
		if (p2pinfo->p2p_ps_mode > P2P_PS_NONE) {
			p2pinfo->p2p_ps_state = p2p_ps_state;

			if (p2pinfo->ctwindow > 0) {
				if (rtlps->smart_ps != 0) {
					rtlps->smart_ps = 0;
					rtlpriv->cfg->ops->set_hw_reg(hw,
						 HW_VAR_H2C_FW_PWRMODE,
						 &rtlps->pwr_mode);
				}
			}
			rtlpriv->cfg->ops->set_hw_reg(hw,
				 HW_VAR_H2C_FW_P2P_PS_OFFLOAD,
				 &p2p_ps_state);

		}
		break;
	case P2P_PS_SCAN:
	case P2P_PS_SCAN_DONE:
	case P2P_PS_ALLSTASLEEP:
		if (p2pinfo->p2p_ps_mode > P2P_PS_NONE) {
			p2pinfo->p2p_ps_state = p2p_ps_state;
			rtlpriv->cfg->ops->set_hw_reg(hw,
				 HW_VAR_H2C_FW_P2P_PS_OFFLOAD,
				 &p2p_ps_state);
		}
		break;
	default:
		break;
	}
	RT_TRACE(rtlpriv, COMP_FW, DBG_LOUD,
		 "ctwindow %x oppps %x\n",
		 p2pinfo->ctwindow , p2pinfo->opp_ps);
	RT_TRACE(rtlpriv, COMP_FW, DBG_LOUD,
		 "count %x duration %x index %x interval %x start time %x noa num %x\n",
		 p2pinfo->noa_count_type[0],
		 p2pinfo->noa_duration[0],
		 p2pinfo->noa_index,
		 p2pinfo->noa_interval[0],
		 p2pinfo->noa_start_time[0],
		 p2pinfo->noa_num);
	RT_TRACE(rtlpriv, COMP_FW, DBG_LOUD, "end\n");
}