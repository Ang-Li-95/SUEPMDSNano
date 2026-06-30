#python3 crab_multi_dataset.py -i muonSkim2024.txt -p config_exoSkim.py -t MDSnano

#python3 crab_multi_dataset.py -i muonSkims.txt -p config_exoSkim.py -t MDSnano  -o /store/group/lpclonglived/MDSnano/

#python3 crab_multi_dataset.py -i signal2023.txt -p config_AOD_mc.py -t MDSnano  -o /store/group/lpclonglived/MDSnano/signals2023/

#python3 crab_multi_dataset.py -i muonSkims.txt -p config_AOD.py -t MDSnano  -o /store/group/lpclonglived/MDSnano/
python3 crab_multi_dataset.py -i muonSkims.txt -p custom_config_AOD_data.py -t MDSnano  -o /store/group/lpclonglived/MDSnano/

#python3 crab_multi_dataset.py -i zerobias.txt -p runCSCShowerAnalyzer_cfg.py -t HMTnTuple  -o /store/group/lpclonglived/HLT/zerobias22/
