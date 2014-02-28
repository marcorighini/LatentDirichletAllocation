close all

% f_0 = fopen('kos/kos_50/kos_0_perpl.dat');
% f_1 = fopen('kos/kos_50/kos_1_perpl.dat');
% f_2 = fopen('kos/kos_50/kos_2_perpl.dat');
% f_3 = fopen('kos/kos_50/kos_3_perpl.dat');

f_0 = fopen('nips/nips_100/nips_0_perpl.dat');
f_1 = fopen('nips/nips_100/nips_1_perpl.dat');
f_2 = fopen('nips/nips_100/nips_2_perpl.dat');
f_3 = fopen('nips/nips_100/nips_3_perpl.dat');

% f_0 = fopen('enron/enron_50/enron_0_perpl.dat');
% f_1 = fopen('enron/enron_50/enron_1_perpl.dat');
% f_2 = fopen('enron/enron_50/enron_2_perpl.dat');
% f_3 = fopen('enron/enron_50/enron_3_perpl.dat');

p_0= fscanf(f_0, '%d %f',[2 inf]);
p_1= fscanf(f_1, '%d %f',[2 inf]);
p_2= fscanf(f_2, '%d %f',[2 inf]);
p_3= fscanf(f_3, '%d %f',[2 inf]);

hfig = figure;
plot(p_0(1,:),p_0(2,:),'-s','MarkerFaceColor','b')
set(gca, 'fontsize', 16)
grid on
hold on
plot(p_1(1,:),p_1(2,:),'-s','MarkerFaceColor','b')
plot(p_2(1,:),p_2(2,:),'-s','MarkerFaceColor','b')
plot(p_3(1,:),p_3(2,:),'-s','MarkerFaceColor','b')

xlabel('Iteration','FontSize',16)    
ylabel('Perplexity','FontSize',16)         
title('4-fold cross-validation perplexities (topics=100)','FontSize',22)

set(gca,'XLim',[80 800])
set(gca,'YLim',[1400 1490])
set(gca,'XTick',p_1(1,:))

fclose(f_0);
fclose(f_1);
fclose(f_2);
fclose(f_3);