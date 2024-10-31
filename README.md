# DriverPrivesc
A privilege escalation vulnerability in the ECHOAC anti-cheat driver EchoDrv

# Issue

The problem lies in the `0xE6224248` IOCtl case, which returns a Handle to any specified process with **PROCESS_ALL_ACCESS** rights.

# Exploitation

You can use this handle, to spawn a `Child Process` in the `Winlogon Process` as `System`. 

For this you will need to modify the `PID` in the source code to the `PID` of the `Winlogon Process`
