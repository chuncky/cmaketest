


autobuild.bat -f craneg_bld_rel.conf -t framework
autobuild.bat -f craneg_bld_rel.conf -t daily_build


autobuild-boot33.bat -f craneg_bld_rel.conf -t daily_build

autobuild-boot33.bat -f craneg_evb_a0_bld_lwg_mipi.conf -t daily_build

env.bat
cmake -f craneg_evb_a0_bld_lwg_mipi.conf

make




