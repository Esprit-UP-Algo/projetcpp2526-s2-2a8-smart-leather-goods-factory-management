@echo off
set PARASITE=C:\Users\lynam\Desktop\projetcpp2526-s2-2a8-smart-leather-goods-factory-management\build\Desktop_Qt_6_7_3_MinGW_64_bit-Debug\build
echo Suppression de: %PARASITE%
rmdir /s /q "%PARASITE%"
if exist "%PARASITE%" (
    echo ECHEC - dossier toujours present
) else (
    echo SUCCES - dossier supprime
)
