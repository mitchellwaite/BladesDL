# BladesDL

A basic Dashlaunch substitute for Blades kernel. Performs some of the basic tasks Dashlaunch would normally provide.

## Supported Kernels

- 6770
- 1888

## Features

- Live is blocked via a DNS hook.
- System update strings are patched to $$ystemupdate.
- OG Xbox emulator is fixed by auto toggling the HV memory protections using syscall 0.
- System link ping limit removed.
- (1888 only for now) Both devkit and retail unsigned xex'es are supported by hooking XexpLoadImage and XexpVerifyImageHeaders
- xextool automatically processes the xex as a post build step to remove encryption and compression so it will work regardless of kernel patches

## TODO

- 6717 support... should be simple
- Plugins? not that many would work, but a debugging plugin might be useful
- contpatch, licpatch, xblapatch, etc. The leaked DashLaunch source only has contpatch so we'll see.

## Credits

- Byrom90 - The original BladesDL implementation that this is based on.
- c0z - Majority of the functions/hooks were backported from an old Dashlaunch source found online.
