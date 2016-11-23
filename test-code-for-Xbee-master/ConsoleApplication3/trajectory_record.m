clear all;
close all;

inputFile = 'data/2016_11_19.csv';
str = strsplit(inputFile, {'/', '.'});
outputFile1 = strcat({'data/figure_'}, str(2), {'_補正前.png'});
outputFile2 = strcat({'data/figure_'}, str(2), {'_補正後.png'});

size = csvread(inputFile, 0, 0, [0, 0, 0, 1]);
data = csvread(inputFile, 2, 0);

% 水田のサイズ cm → mの変換
width = size(1)/100;
height = size(2)/100;

% 内側領域
xin = [1, width-1, width-1, 1, 1];
yin = [1, 1, height-1, height-1, 1];

% x軸重心 cm → mの変換
xpos = data(:, 1)/100;
% y軸補正前重心 cm → mの変換
ydef = data(:, 2)/100;
% y軸補正後重心 cm → mの変換
ypos = data(:, 3)/100;

figure;
plot(xpos, ydef, 'r', 'LineWidth', 1.5);
hold on;
plot(xin, yin, 'b', 'LineWidth', 2);
axis([0, width, 0, height]);
title('ロボットの軌跡（補正前重心）', 'FontSize', 24);
xlabel('x軸重心 [m]', 'FontSize', 18);
ylabel('y軸補正前重心 [m]', 'FontSize', 18);
grid on;
ax = gca;
% グリッドの透明度, ライン幅
ax.GridAlpha = 0.5;
ax.LineWidth = 1;
% 軸の表示幅
ax.XTick = [0:1:width];
ax.YTick = [0:1:height];
saveas(gcf, char(outputFile1));

figure;
plot(xpos, ypos, 'r', 'LineWidth', 1.5);
hold on;
plot(xin, yin, 'b', 'LineWidth', 2);
axis([0, width, 0, height]);
title('ロボットの軌跡（補正後重心）', 'FontSize', 24);
xlabel('x軸重心 [m]', 'FontSize', 18);
ylabel('y軸補正後重心 [m]', 'FontSize', 18);
grid on;
ax = gca;
% グリッドの透明度, ライン幅
ax.GridAlpha = 0.5;
ax.LineWidth = 1;
% 軸の表示幅
ax.XTick = [0:1:width];
ax.YTick = [0:1:height];
saveas(gcf, char(outputFile2));