#!/bin/sh
lli=${LLVMINTERP-lli}
exec $lli \
    /home/userfs/m/ms1516/w2k/embs_open2/particle_sim_hls/solution1/.autopilot/db/a.g.bc ${1+"$@"}