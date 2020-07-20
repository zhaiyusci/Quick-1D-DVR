# Quick 1D DVR

An interface of yDVR for non-programmer

Yu Zhai <yuzhai@mail.huiligroup.org> 

Jilin University, Changchun, China

## Introduction

The `Quick 1D DVR` code is an interface of part of the functionality of (yDVR)[https://github.com/zhaiyusci/ydvr] for non-programmer.

Idea behind the code is to do a sinc-DVR calculation, with automatic converge check, etc.

In initial version (v 0.1.0), I will focus on the energy only, since sinc-DVR eigenvectors are too large to use directly, in most cases.

## Installation guide

As is said in the Introduction part, it is for the non-programmer, thus it is shipped with all its reliance, including a copy of Eigen 3.3.7 and yDVR 2.1.0.

To build it, simply type `make` and press `enter`.

Also I provide a binary version so the user can use it directly.

## Usage
Usage: /path/to/quick1ddvr <filename>.inp

The input file should be a plain text file with format shown in `sample_hcl.inp`
`#` is the starting of comments.


