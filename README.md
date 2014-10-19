README
=========
The Active Suspension Program has support for both Code Composer Studio(CCS) and GCC
How to Setup Project on CCS

You MUST use CCS6 or above.
----
1. Open CCS and click File>Import
2. The import menu will appear, you need to select Code Composer Studio>CCS Projects as your import source then click next
3. Click the browse button and locate the folder containing the repository of this project cloned from git the click OK
4. The project ENCE463_A should appear under "Discovered projects", select it and click "Finish"
5. You must exclude the following files (by default these instructions will build WUS)
    - Exclude wus.c, wus_pulse_out.c and sus_simulate_task.c to build for ASC(Active Suspension Controller)
    - Exclude asc.c, asc_pulse_out.c and asc_control_task.c to build for WUS(Wheel Unit Simulator)
6. Build and Execute your project

How to compile and load with GCC
----
Requirements:
arm-none-eabi-gcc
CMake (and make)
OpenOCD

1. Run
	cmake -DCMAKE_TOOLCHAIN_FILE=linux/arm-none-eabi_toolchain.cmake .
	make
2. To load on code add the udev rules linux/55-ence463.rules (or run as root)
3. Start OpenOCD
	openocd -f linux/ek-lm3s1968.cfg
4. Start GDB
	arm-none-eabi-gdb
5. Load the code, macros have been defined in .gdbinit
	load-wus
or
	load-asc

[New project config](https://www.dropbox.com/s/1y5pj6wavyf8rel/NewProject.png?dl=0)
[import config](https://www.dropbox.com/s/zqc0fircsv25gjk/ImportFolderSettings.png?dl=0)
[Include directories](https://www.dropbox.com/s/3l3sa0gvs8gzi9g/IncludePaths.png?dl=0)
[Files to exclude from build](https://www.dropbox.com/s/h0rfr44wj66e0m8/ExcludeFromBuild.png?dl=0)
[StellarisWare files to exclude from build](https://www.dropbox.com/s/6apmgl2tiws309r/ExcludeFromBuildStellarisWare.png?dl=0)
[Include libraies](https://www.dropbox.com/s/unwtzkau690odsv/Libaries.png?dl=0)
