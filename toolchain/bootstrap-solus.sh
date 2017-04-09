#!/bin/bash

set -x
sudo eopkg it flex-devel make bison-devel gmp-devel mpfr-devel mpc-devel texinfo -c system.devel
