target_compile_options(application PUBLIC
	"$<$<COMPILE_LANGUAGE:ASM>:SHELL:-mthumb -mcpu=cortex-m3 -g3>"
	"$<$<COMPILE_LANGUAGE:CXX>:SHELL:-mthumb -mcpu=cortex-m3 -g3 --specs=nosys.specs -fdata-sections -ffunction-sections>"
	"$<$<COMPILE_LANGUAGE:C>:SHELL:-mthumb -mcpu=cortex-m3 -g3 --specs=nosys.specs -fdata-sections -ffunction-sections>"
	)
