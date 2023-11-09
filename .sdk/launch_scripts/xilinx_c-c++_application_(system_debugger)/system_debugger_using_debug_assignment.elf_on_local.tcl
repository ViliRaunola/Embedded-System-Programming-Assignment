connect -url tcp:127.0.0.1:3121
source G:/Opiskelu/Yliopisto/5.vuosi/EmbeddedSystemsProgramming/assignment/zybo_platform/ps7_init.tcl
targets -set -nocase -filter {name =~"APU*" && jtag_cable_name =~ "Digilent Zybo Z7 210351AF232EA"} -index 0
loadhw -hw G:/Opiskelu/Yliopisto/5.vuosi/EmbeddedSystemsProgramming/assignment/zybo_platform/system.hdf -mem-ranges [list {0x40000000 0xbfffffff}]
configparams force-mem-access 1
targets -set -nocase -filter {name =~"APU*" && jtag_cable_name =~ "Digilent Zybo Z7 210351AF232EA"} -index 0
stop
ps7_init
ps7_post_config
targets -set -nocase -filter {name =~ "ARM*#0" && jtag_cable_name =~ "Digilent Zybo Z7 210351AF232EA"} -index 0
rst -processor
targets -set -nocase -filter {name =~ "ARM*#0" && jtag_cable_name =~ "Digilent Zybo Z7 210351AF232EA"} -index 0
dow G:/Opiskelu/Yliopisto/5.vuosi/EmbeddedSystemsProgramming/assignment/assignment/Debug/assignment.elf
configparams force-mem-access 0
targets -set -nocase -filter {name =~ "ARM*#0" && jtag_cable_name =~ "Digilent Zybo Z7 210351AF232EA"} -index 0
con
