target remote :3333

define load-wus
	file src/wus.axf
	load
	monitor reset init
end

define load-asc
	file src/asc.axf
	load
	monitor reset init
end
