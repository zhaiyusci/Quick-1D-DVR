This is **NOT** a full version of yDVR.

To developers
=============

This is not a full version of `yDVR <https://github.com/zhaiyusci/ydvr>`__.
Instead, only the one-dimension sinc-DVR part is presented here.

A script is written to strip the sinc-DVR part out out a full yDVR, see ``strip.sh``.
To use it, modify the ``FULLYDVR`` variable therein to your position where yDVR is cloned.
However, typically you do not need to use the script. 
Needed files are already included in Quick-1D-DVR repo.

Why do not we have a full yDVR?
===============================

- We do not want to included a full yDVR which is also include functions
  beyond 1D sinc-DVR.  Compile them is time-consuming.
- yDVR is built with CMake while a non-programmer likely do not have one.
- Although a binray is released, the users sometimes still need compile
  on their own. The reason varies, e.g., old Linux kernel.



