xcopy /y %1\File\*
mkdir avmc
xcopy /y  %1\File\src_avm\* src_avm\
xcopy /y %1\File\src_avm\send_preodic_instance\* src_avm\send_preodic_instance\
exit