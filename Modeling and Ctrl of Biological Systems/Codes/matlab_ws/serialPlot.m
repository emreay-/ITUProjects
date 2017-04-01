clear
clc

%User Defined Properties 
serialPort = 'COM3';           % define COM port #
baudRate = 9600;
plotTitle = 'Volumetric Flow Rate Plot';               % plot title
xLabel = 'Elapsed Time [s]';                           % x-axis label
yLabel = 'Volumetric Flow Rate [cc/s]';                % y-axis label
plotGrid = 'on';                % 'off' to turn off grid
min = -3000;                    % set y-min
max = 3000;                     % set y-max
scrollWidth = 15;               % display period in plot, plot entire data log if <= 0
delay = .01;                    % make sure sample faster than resolution

%Define Function Variables
time = 0;
data = 0;
count = 0;

%Set up Plot
plotGraph = plot(time,data,'LineWidth',3,'Color',[0.2,0,0.5]);

title(plotTitle,'FontSize',25);
xlabel(xLabel,'FontSize',15);
ylabel(yLabel,'FontSize',15);
axis([0 1 min max]);
set(gca,'ytick',[min:200:max])
grid(plotGrid);

%Open Serial COM Port
s = serial(serialPort,'BaudRate',baudRate)
disp('Close Plot to End Session');
fopen(s);

tic

while ishandle(plotGraph) %Loop when Plot is Active

    dat = fscanf(s,'%f'); %Read Data from Serial as Float

    if(~isempty(dat) && isfloat(dat)) %Make sure Data Type is Correct        
        count = count + 1;    
        time(count) = toc;    %Extract Elapsed Time
        data(count) = dat(1); %Extract 1st Data Element

        %Set Axis according to Scroll Width
        if(scrollWidth > 0)
        set(plotGraph,'XData',time(time > time(count)-scrollWidth),'YData',data(time > time(count)-scrollWidth));
        axis([time(count)-scrollWidth time(count) min max]);
        
        else
        set(plotGraph,'XData',time,'YData',data);
        axis([0 time(count) min max]);
        end

        %Allow MATLAB to Update Plot
        pause(delay);
    end
end

%Close Serial COM Port and Delete useless Variables
fclose(s);
clear count dat delay max min baudRate plotGraph plotGrid plotTitle s ...
        scrollWidth serialPort xLabel yLabel;


disp('Session Terminated...');
%plot(time,data)