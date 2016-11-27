clear all;
close all;

filename = '20161117.csv';
size = csvread(filename, 0, 0, [0, 0, 0, 1]);
data = csvread(filename, 2, 0);

% ���c�̃T�C�Y cm �� m�̕ϊ�
width = size(1)/100;
height = size(2)/100;

% �����̈�
xin = [1, width-1, width-1, 1, 1];
yin = [1, 1, height-1, height-1, 1];

% x���d�S cm �� m�̕ϊ�
xpos = data(:, 1)/100;
% y���␳�O�d�S cm �� m�̕ϊ�
ydef = data(:, 2)/100;
% y���␳��d�S cm �� m�̕ϊ�
ypos = data(:, 3)/100;

figure;
plot(xpos, ydef, 'r', 'LineWidth', 1.5);
hold on;
plot(xin, yin, 'b', 'LineWidth', 2);
axis([0, width, 0, height]);
title('���{�b�g�̋O�Ձi�␳�O�d�S�j', 'FontSize', 24);
xlabel('x���d�S [m]', 'FontSize', 18);
ylabel('y���␳�O�d�S [m]', 'FontSize', 18);
grid on;
ax = gca;
% �O���b�h�̓����x, ���C����
ax.GridAlpha = 0.5;
ax.LineWidth = 1;
% ���̕\����
ax.XTick = [0:1:width];
ax.YTick = [0:1:height];

figure;
plot(xpos, ypos, 'r', 'LineWidth', 1.5);
hold on;
plot(xin, yin, 'b', 'LineWidth', 2);
axis([0, width, 0, height]);
title('���{�b�g�̋O�Ձi�␳��d�S�j', 'FontSize', 24);
xlabel('x���d�S [m]', 'FontSize', 18);
ylabel('y���␳�O�d�S [m]', 'FontSize', 18);
grid on;
ax = gca;
% �O���b�h�̓����x, ���C����
ax.GridAlpha = 0.5;
ax.LineWidth = 1;
% ���̕\����
ax.XTick = [0:1:width];
ax.YTick = [0:1:height];