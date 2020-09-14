amlc -c src_avm/test.aml	
move test.avmc avmc

start "can_simulator_consol" osp_rtw_app -d 0.0.0.0:9222 -e test:loop/0 --alive avmc/test.avmc %*