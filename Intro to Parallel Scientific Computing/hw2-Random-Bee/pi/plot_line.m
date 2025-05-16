err_n = dlmread('error_n.dat');

figure(1), clf
plot(err_n(:,1), err_n(:,2), 'o', 'LineWidth', 2)
loglog(err_n(:,1), err_n(:,2), 'o', 'LineWidth', 2)
hold on
loglog(err_n(:,1), err_n(:,1).^(-1), 'k--', 'LineWidth', 2)
loglog(err_n(:,1), err_n(:,1).^(-2), 'r--', 'LineWidth', 2)
xlabel('n'), ylabel('Error')
set(gca,'fontsize',14)
screen2jpeg(strcat('error_n.png'))
