	/* Enable TRNG peripheral clock */
	PMC_EnablePeripheral(ID_TRNG);

	/* Configure TC interrupts */
	NVIC_ClearPendingIRQ(TRNG_IRQn);
	NVIC_SetPriority(TRNG_IRQn ,1);
	NVIC_EnableIRQ(TRNG_IRQn);

	/* Enable TRNG interrupt */
	TRNG_EnableIt();

	/* Enable TRNG */
	TRNG_Enable();
	
======================
	
    if (BUTTON_read()) cnt = 0;
    else if (cnt < 5001) cnt++;

    if (5000 == cnt) {
		fast = !fast;
		timer_set_period(fast ? PERIOD_FAST : PERIOD_SLOW);
    }
