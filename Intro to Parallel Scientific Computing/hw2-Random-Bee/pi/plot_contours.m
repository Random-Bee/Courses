xg = dlmread('xgrid.dat');
yg = dlmread('ygrid.dat');
ff = dlmread('flags.dat');
size(xg)
size(yg)
size(ff)
n = length(xg);

figure(1)
contourf(xg, yg, ff')
xlabel('x'), ylabel('y')
set(gca,'fontsize',14)
colorbar
screen2jpeg(strcat('flags_',num2str(n,'%04d'),'.png'))

