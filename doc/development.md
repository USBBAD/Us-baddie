# Issues

- [ ] Gets hardfaulted at some time.
	- [?] The issue might be that the EP 1 is getting disabled by EP0's driver while transmitting something;
		- tried to not to disable it -- didn't work
	- See Table 18 from the "Programming manual" to establish the source of the failure;
		- Placing hooks on various system ISRs didn't yield results;
- [ ] Judjing from the UART debug output, EP1 driver callbacks do not get invoked;
- [ ] Implement LED-based debugging
