set xml=LicensePlate

cd %~dp0

component_mod_compiler.com %xml%.xml lp.mesh
rem del "C:\Program Files (x86)\Steam\steamapps\common\Stormworks\rom\data\components\%xml%.bin"
rem copy %xml%.bin "C:\Program Files (x86)\Steam\steamapps\common\Stormworks\rom\data\components"\%xml%.bin

exit