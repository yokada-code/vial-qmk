#!/bin/sh
version="1_0_2_rc"
make ble_micro_pro:vial:zip -j8
make ble_micro_pro:vial_no_msc:zip -j8

mkdir .build/release

unzip -o .build/ble_micro_pro_vial.zip -d .build/release
unzip -o .build/ble_micro_pro_vial_no_msc.zip -d .build/release

mv .build/release/ble_micro_pro_vial.bin .build/release/ble_micro_pro_vial_${version}.bin
mv .build/release/ble_micro_pro_vial.dat .build/release/ble_micro_pro_vial_${version}.dat
mv .build/release/ble_micro_pro_vial_no_msc.bin .build/release/ble_micro_pro_vial_${version}_no_msc.bin
mv .build/release/ble_micro_pro_vial_no_msc.dat .build/release/ble_micro_pro_vial_${version}_no_msc.dat