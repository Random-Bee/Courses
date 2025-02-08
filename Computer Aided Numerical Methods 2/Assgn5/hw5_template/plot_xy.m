basedir = './';
tst = 0; ten = 1000; dt = 100;

for tid = tst:dt:ten
  fname = strcat(basedir,'T_x_', num2str(tid, '%04d'), '.dat')
  rawdat = dlmread(fname);
  x = rawdat(:,1); T = rawdat(:,2); np = length(x);
  figure(1), clf
  plot(x, T, '-o')
  xlabel('x'), ylabel('T')
  title(strcat('t = ', num2str(tid)))
  axis([-0.1 1.1 -1.1 1.1])
  set(gca,'fontsize',14)
  screen2jpeg(strcat(basedir,'plot_T_x_t',num2str(tid,'%04d'),'.png'))
end
