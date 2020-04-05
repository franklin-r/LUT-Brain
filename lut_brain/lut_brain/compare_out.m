clc
close all
clear

% number of outputs
n_out = 9;

p          = "output_data/";
path       = "output_data_addr/";   %Base output
path_hw    = "output_data_addrhard/";
path_hwopt = "output_data_addrhardopt/";

for i = 0:n_out

    filename = "result_"+i+".csv";
    tab0 = load(p+filename);    
    tab1 = load(path+filename);
    tab2 = load(path_hw+filename);
    tab3 = load(path_hwopt+filename);
    current(i+1) = isequal(tab0,tab1);
    a(i+1) = isequal(tab1,tab2);
    b(i+1) = isequal(tab1,tab3);
end

current %Print the current output vs base output comparison