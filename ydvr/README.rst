This is **NOT** a full version of yDVR.

To developers
=============

This is not a full version of `yDVR <https://github.com/zhaiyusci/ydvr>`__.
Instead, one the one-dimension sinc-DVR part are shown here.

A script is written to strip the sinc-DVR part out out a full yDVR, see ``strip.sh``.
To use it, modify the ``FULLYDVR`` variable there to your position where it is cloned.
However, typically you do not need to use the script. 
Needed files are already included in Quick-1D-DVR repo.

Why do not have a full yDVR?
============================

- We do not want to included a full yDVR which is also include too much functions
  beyond 1D sinc-DVR.  Compile it is time-consuming.
- yDVR is built with CMake while a non-programmer likely do not have one.
- However, the users sometimes still need compile it on their own.
  The reason varies, e.g., old Linux kernel.



