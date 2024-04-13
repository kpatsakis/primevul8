static bool rtl_ps_set_rf_state(struct ieee80211_hw *hw,
				enum rf_pwrstate state_toset,
				u32 changesource)
{
	struct rtl_priv *rtlpriv = rtl_priv(hw);
	struct rtl_ps_ctl *ppsc = rtl_psc(rtl_priv(hw));
	enum rf_pwrstate rtstate;
	bool actionallowed = false;
	u16 rfwait_cnt = 0;

	/*Only one thread can change
	 *the RF state at one time, and others
	 *should wait to be executed.
	 */
	while (true) {
		spin_lock(&rtlpriv->locks.rf_ps_lock);
		if (ppsc->rfchange_inprogress) {
			spin_unlock(&rtlpriv->locks.rf_ps_lock);

			RT_TRACE(rtlpriv, COMP_ERR, DBG_WARNING,
				 "RF Change in progress! Wait to set..state_toset(%d).\n",
				  state_toset);

			/* Set RF after the previous action is done.  */
			while (ppsc->rfchange_inprogress) {
				rfwait_cnt++;
				mdelay(1);
				/*Wait too long, return false to avoid
				 *to be stuck here.
				 */
				if (rfwait_cnt > 100)
					return false;
			}
		} else {
			ppsc->rfchange_inprogress = true;
			spin_unlock(&rtlpriv->locks.rf_ps_lock);
			break;
		}
	}

	rtstate = ppsc->rfpwr_state;

	switch (state_toset) {
	case ERFON:
		ppsc->rfoff_reason &= (~changesource);

		if ((changesource == RF_CHANGE_BY_HW) &&
		    (ppsc->hwradiooff)) {
			ppsc->hwradiooff = false;
		}

		if (!ppsc->rfoff_reason) {
			ppsc->rfoff_reason = 0;
			actionallowed = true;
		}

		break;

	case ERFOFF:

		if ((changesource == RF_CHANGE_BY_HW) && !ppsc->hwradiooff) {
			ppsc->hwradiooff = true;
		}

		ppsc->rfoff_reason |= changesource;
		actionallowed = true;
		break;

	case ERFSLEEP:
		ppsc->rfoff_reason |= changesource;
		actionallowed = true;
		break;

	default:
		pr_err("switch case %#x not processed\n", state_toset);
		break;
	}

	if (actionallowed)
		rtlpriv->cfg->ops->set_rf_power_state(hw, state_toset);

	spin_lock(&rtlpriv->locks.rf_ps_lock);
	ppsc->rfchange_inprogress = false;
	spin_unlock(&rtlpriv->locks.rf_ps_lock);

	return actionallowed;
}