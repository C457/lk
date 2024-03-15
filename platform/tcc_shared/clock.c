#include <clock.h>

#if !defined(CONFIG_TCC_CODESONAR_BLOCKED)
#define CLK_CHK_NULL(x)	if(x == NULL)	\
				return 0
#endif

static struct tcc_ckc_ops *clock_ops = NULL;

int tcc_set_ipisol_pwdn(int id, bool pwdn)
{
#if !defined(CONFIG_TCC_CODESONAR_BLOCKED)
	CLK_CHK_NULL(clock_ops);

	if (clock_ops->ckc_ip_pwdn)
		return clock_ops->ckc_ip_pwdn(id, pwdn);
#else
	if (clock_ops->ckc_ip_pwdn && clock_ops)
		return clock_ops->ckc_ip_pwdn(id, pwdn);
#endif

	return -CKC_NO_OPS_DATA;
}

int tcc_set_pll(int id, bool en, unsigned long rate, plldiv_t div)
{
	int ret = -CKC_NO_OPS_DATA;

#if !defined(CONFIG_TCC_CODESONAR_BLOCKED)
	CLK_CHK_NULL(clock_ops);

	if (clock_ops->ckc_pll_set_rate)
		ret = clock_ops->ckc_pll_set_rate(id, rate);

	if (div) {
		if (clock_ops->ckc_plldiv_set)
			clock_ops->ckc_plldiv_set(id, (div>1)?(div-1):0);
	}
#else
	if (clock_ops->ckc_pll_set_rate && clock_ops)
		ret = clock_ops->ckc_pll_set_rate(id, rate);

	if (div) {
		if (clock_ops->ckc_plldiv_set && clock_ops)
			clock_ops->ckc_plldiv_set(id, (div > 1) ? (div - 1) : 0);
	}
#endif

	return ret;
}

unsigned long tcc_get_pll(int id)
{
#if !defined(CONFIG_TCC_CODESONAR_BLOCKED)
	CLK_CHK_NULL(clock_ops);

	if (clock_ops->ckc_pll_get_rate)
		return clock_ops->ckc_pll_get_rate(id);
#else
	if (clock_ops->ckc_pll_get_rate && clock_ops)
		return clock_ops->ckc_pll_get_rate(id);
#endif

	return 0;
}

int tcc_set_clkctrl(int id, bool en, unsigned long rate)
{
	int ret;

#if !defined(CONFIG_TCC_CODESONAR_BLOCKED)
	CLK_CHK_NULL(clock_ops);

	if (clock_ops->ckc_clkctrl_set_rate)
#else
	if (clock_ops->ckc_clkctrl_set_rate && clock_ops)
#endif
	{
		ret = clock_ops->ckc_clkctrl_set_rate(id, rate);
		if (ret)
			return ret;

		if (en) {
			if (clock_ops->ckc_pmu_pwdn)
				clock_ops->ckc_pmu_pwdn(id, 0);
			if (clock_ops->ckc_swreset)
				clock_ops->ckc_swreset(id, 0);
			if (clock_ops->ckc_clkctrl_enable)
				ret = clock_ops->ckc_clkctrl_enable(id);
		}
		else {
			if (clock_ops->ckc_clkctrl_disable)
				ret = clock_ops->ckc_clkctrl_disable(id);
			if (clock_ops->ckc_swreset)
				clock_ops->ckc_swreset(id, 1);
			if (clock_ops->ckc_pmu_pwdn)
				clock_ops->ckc_pmu_pwdn(id, 1);
		}
		return ret;
	}

	return -CKC_NO_OPS_DATA;
}

unsigned long tcc_get_clkctrl(int id)
{
#if !defined(CONFIG_TCC_CODESONAR_BLOCKED)
	CLK_CHK_NULL(clock_ops);

	if (clock_ops->ckc_clkctrl_get_rate)
		return clock_ops->ckc_clkctrl_get_rate(id);
#else
	if (clock_ops->ckc_clkctrl_get_rate && clock_ops)
		return clock_ops->ckc_clkctrl_get_rate(id);
#endif

	return 0;
}

int tcc_set_peri(int id, bool en, unsigned long rate)
{
	int ret;

#if !defined(CONFIG_TCC_CODESONAR_BLOCKED)
	CLK_CHK_NULL(clock_ops);

	if (clock_ops->ckc_peri_set_rate)
#else
	if (clock_ops->ckc_peri_set_rate && clock_ops)
#endif
	{
		ret = clock_ops->ckc_peri_set_rate(id, rate);
		if (ret)
			return ret;

		if (en) {
			if (clock_ops->ckc_peri_enable)
				ret = clock_ops->ckc_peri_enable(id);
		}
		else {
			if (clock_ops->ckc_peri_disable)
				ret = clock_ops->ckc_peri_disable(id);
		}
		return ret;
	}

	return -CKC_NO_OPS_DATA;
}

unsigned long tcc_get_peri(int id)
{
#if !defined(CONFIG_TCC_CODESONAR_BLOCKED)
	CLK_CHK_NULL(clock_ops);

	if (clock_ops->ckc_peri_get_rate)
		return clock_ops->ckc_peri_get_rate(id);
#else
	if (clock_ops->ckc_peri_get_rate && clock_ops)
		return clock_ops->ckc_peri_get_rate(id);
#endif

	return 0;
}

int tcc_set_ddibus_pwdn(int id, bool pwdn)
{
#if !defined(CONFIG_TCC_CODESONAR_BLOCKED)
	CLK_CHK_NULL(clock_ops);

	if (clock_ops->ckc_ddibus_pwdn)
		return clock_ops->ckc_ddibus_pwdn(id, pwdn);
#else
	if (clock_ops->ckc_ddibus_pwdn && clock_ops)
		return clock_ops->ckc_ddibus_pwdn(id, pwdn);
#endif

	return -CKC_NO_OPS_DATA;
}

int tcc_set_ddibus_swreset(int id, bool reset)
{
#if !defined(CONFIG_TCC_CODESONAR_BLOCKED)
	CLK_CHK_NULL(clock_ops);

	if (clock_ops->ckc_ddibus_swreset)
		return clock_ops->ckc_ddibus_swreset(id, reset);
#else
	if (clock_ops->ckc_ddibus_swreset && clock_ops)
		return clock_ops->ckc_ddibus_swreset(id, reset);
#endif

	return -CKC_NO_OPS_DATA;
}

int tcc_set_iobus_pwdn(int id, bool pwdn)
{
#if !defined(CONFIG_TCC_CODESONAR_BLOCKED)
	CLK_CHK_NULL(clock_ops);

	if (clock_ops->ckc_iobus_pwdn)
		return clock_ops->ckc_iobus_pwdn(id, pwdn);
#else
	if (clock_ops->ckc_iobus_pwdn && clock_ops)
		return clock_ops->ckc_iobus_pwdn(id, pwdn);
#endif

	return -CKC_NO_OPS_DATA;
}

int tcc_set_iobus_swreset(int id, bool reset)
{
#if !defined(CONFIG_TCC_CODESONAR_BLOCKED)
	CLK_CHK_NULL(clock_ops);

	if (clock_ops->ckc_iobus_swreset)
		return clock_ops->ckc_iobus_swreset(id, reset);
#else
	if (clock_ops->ckc_iobus_swreset && clock_ops)
		return clock_ops->ckc_iobus_swreset(id, reset);
#endif

	return -CKC_NO_OPS_DATA;
}

int tcc_set_hsiobus_pwdn(int id, bool pwdn)
{
#if !defined(CONFIG_TCC_CODESONAR_BLOCKED)
	CLK_CHK_NULL(clock_ops);

	if (clock_ops->ckc_hsiobus_pwdn)
		return clock_ops->ckc_hsiobus_pwdn(id, pwdn);
#else
	if (clock_ops->ckc_hsiobus_pwdn && clock_ops)
		return clock_ops->ckc_hsiobus_pwdn(id, pwdn);
#endif

	return -CKC_NO_OPS_DATA;
}

int tcc_set_hsiobus_swreset(int id, bool reset)
{
#if !defined(CONFIG_TCC_CODESONAR_BLOCKED)
	CLK_CHK_NULL(clock_ops);

	if (clock_ops->ckc_hsiobus_swreset)
		return clock_ops->ckc_hsiobus_swreset(id, reset);
#else
	if (clock_ops->ckc_hsiobus_swreset && clock_ops)
		return clock_ops->ckc_hsiobus_swreset(id, reset);
#endif

	return -CKC_NO_OPS_DATA;
}

/*
 * Register clock ops
 */
void tcc_ckc_set_ops(struct tcc_ckc_ops *ops)
{
	clock_ops = ops;
}

