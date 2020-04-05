clc
close all
clear


n_out = 9;

p          = "output_data/";
path       = "output_data_addr/";
path_hw    = "output_data_addrhard/";
path_hwopt = "output_data_addrhardopt/";

filename = "result_"+1+".csv";
tab1 = load(path+filename);
tab2 = load(p+filename);    
isequal(tab1,tab2)

% for i = 0:n_out
%     filename = "result_"+i+".csv";
%     tab1 = load(path+filename);
%     tab2 = load(path_hw+filename);
%     tab3 = load(path_hwopt+filename);
%     a(i+1) = isequal(tab1,tab2);
%     b(i+1) = isequal(tab1,tab3);
% end