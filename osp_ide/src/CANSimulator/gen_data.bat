osp-dbc -c command_data.json
for %%a in (*.aml) do amlc -c %%a

move *.aml src_avm
move *.avmc avmc