%% ЛР3. Программное управление краном с ограничениями
% Требуется Global Optimization Toolbox.
% Выполнил(а): Соин Андрей Дмитриевич, Величко Федор Тимофеевич
% Общие функции: decodeCommand, simulateCrane, controlCost,
% constraintViolation, makeGAOptions; затем функция своего варианта.
% Функцию другого варианта заполнять не нужно.

%% Настройки опыта
clear cfg strategies trials;
close all;
variant = 2;
cfg.smoothWeight = 0.05;
cfg.population = 50;
cfg.generations = 120;
cfg.seeds = [11 22 33 44 55];
cfg.crossoverFraction = 0.8;
cfg.elite = 2;
cfg.distance = 1;
cfg.moveTime = 4;
cfg.tailTime = 2;
cfg.segments = 8;
cfg.dt = 0.02;
cfg.length = 1;
cfg.uMax = 0.8;
cfg.vMax = 0.65;
cfg.angleMax = 4*pi/180;
cfg.residualMax = 0.5*pi/180;
cfg.positionTol = 0.02;
cfg.velocityTol = 0.01;
cfg.pathHalfWidth = 0.55;
cfg.tol = 1e-6;
cfg.stopIndex = round(cfg.moveTime/cfg.dt)+1;
cfg.baseU = [0.4 0.1 0.1 0.4 -0.4 -0.1 -0.1 -0.4];

%% Запуски и графики — готовая часть
model = makeModel(cfg, cfg.length);
strategies = experimentSettings(variant, cfg);
trials = runExperiments(cfg, model, strategies);
drawResults(trials, cfg, model, strategies, variant);
printSummary(trials, cfg, strategies);

%% Общие функции для заполнения
function U = decodeCommand(P, uMax)
    % P: N x 8; U: ускорения тех же размеров, м/с^2.
    U = uMax .* P;
end

function trace = simulateCrane(U, model)
    % Строка — одна команда; столбцы результата — моменты времени.
    trace.x = U * model.Hx;
    trace.v = U * model.Hv;
    trace.theta = U * model.Htheta;
    trace.omega = U * model.Homega;
end

function J = controlCost(U, cfg)
    % Верните столбец J: величина ускорения и скачки команды.
    z = U ./ cfg.uMax;
    zPadded = [zeros(size(z,1),1), z, zeros(size(z,1),1)];
    dz = diff(zPadded, 1, 2);
    J = mean(z.^2, 2) + cfg.smoothWeight .* mean(dz.^2, 2);
end

function [V, ratios] = constraintViolation(trace, cfg)
    % ratios: N x 6; V: сумма превышений единицы по строке.
    s = cfg.stopIndex;
    move = 1:s;
    tail = s:size(trace.x,2);

    r1 = abs(trace.x(:,s) - cfg.distance) ./ cfg.positionTol;
    r2 = abs(trace.v(:,s)) ./ cfg.velocityTol;
    r3 = max(abs(trace.v(:,move)), [], 2) ./ cfg.vMax;
    r4 = max(abs(trace.theta(:,move)), [], 2) ./ cfg.angleMax;
    r5 = max(abs(trace.theta(:,tail)), [], 2) ./ cfg.residualMax;
    r6 = max(abs(trace.x(:,move) - cfg.distance/2), [], 2) ./ cfg.pathHalfWidth;

    ratios = [r1 r2 r3 r4 r5 r6];
    V = sum(max(0, ratios - 1), 2);
end

function options = makeGAOptions(cfg, P0)
    % Верните optimoptions для ga; P0 — готовая начальная популяция.
    options = optimoptions('ga', ...
        'PopulationSize', cfg.population, ...
        'MaxGenerations', cfg.generations, ...
        'EliteCount', cfg.elite, ...
        'CrossoverFraction', cfg.crossoverFraction, ...
        'InitialPopulationMatrix', P0);
end

%% Вариант 1. Целевая функция со штрафом
function score = penaltyObjective(p, cfg, model)
    % Одна строка p: 1 x 8; верните одно число J + cfg.rho*V.
    % pass
end

%% Вариант 2. Ограничения для ga
function [c, ceq] = craneConstraints(p, cfg, model)
    U = decodeCommand(p, cfg.uMax);
    trace = simulateCrane(U, model);
    [~, ratios] = constraintViolation(trace, cfg);
    c = (ratios - 1)';
    ceq = [];
end

%% Модель и запуск ga — готовая часть
function model = makeModel(cfg, length)
    A = [0 1 0 0;0 0 0 0;0 0 0 1;0 0 -9.81/length 0];
    B = [0;1;0;-1/length];
    E = expm([A B;zeros(1,5)]*cfg.dt);
    Ad = E(1:4,1:4);
    Bd = E(1:4,5);
    model.t = 0:cfg.dt:cfg.moveTime+cfg.tailTime;
    model.Hx = zeros(cfg.segments,numel(model.t));
    model.Hv = model.Hx;
    model.Htheta = model.Hx;
    model.Homega = model.Hx;
    states = zeros(cfg.segments,4);
    holdSteps = round(cfg.moveTime/cfg.segments/cfg.dt);
    for k=1:numel(model.t)-1
        input = zeros(cfg.segments,1);
        if k<cfg.stopIndex
            input(ceil(k/holdSteps))=1;
        end
        states = states*Ad'+input*Bd';
        model.Hx(:,k+1)=states(:,1);
        model.Hv(:,k+1)=states(:,2);
        model.Htheta(:,k+1)=states(:,3);
        model.Homega(:,k+1)=states(:,4);
    end
end

function strategies = experimentSettings(variant, cfg)
    base = struct('variant',variant,'rho',1, ...
        'crossoverFraction',cfg.crossoverFraction,'label',"");
    strategies = repmat(base,1,3);
    if variant==1
        weights = [0.001 0.01 1];
        for j=1:3
            strategies(j).rho = weights(j);
            strategies(j).label = "Штраф "+weights(j);
        end
    else
        fractions = [0.2 0.8 1];
        for j=1:3
            strategies(j).crossoverFraction = fractions(j);
            strategies(j).label = "Доля скрещивания "+fractions(j);
        end
    end
end

function trials = runExperiments(cfg, model, strategies)
    trials = cell(numel(strategies),numel(cfg.seeds));
    anchor = cfg.baseU/cfg.uMax;
    for k=1:numel(cfg.seeds)
        rng(cfg.seeds(k),'twister');
        P0 = 2*rand(cfg.population,cfg.segments)-1;
        P0(1,:) = anchor;
        near = 2:round(0.7*cfg.population);
        P0(near,:) = min(max(anchor+0.2*randn(numel(near),cfg.segments),-1),1);
        for j=1:numel(strategies)
            rng(1000+cfg.seeds(k),'twister');
            trials{j,k} = runSearch(P0,cfg,model,strategies(j));
        end
    end
end

function out = runSearch(P0, cfg, model, setting)
    cfg.rho = setting.rho;
    cfg.crossoverFraction = setting.crossoverFraction;
    options = makeGAOptions(cfg,P0);
    options = optimoptions(options, ...
        'CreationFcn',@gacreationuniform, ...
        'SelectionFcn',@selectiontournament, ...
        'CrossoverFcn',@crossoverarithmetic, ...
        'MutationFcn',@mutationadaptfeasible, ...
        'NonlinearConstraintAlgorithm','penalty', ...
        'ConstraintTolerance',cfg.tol, ...
        'MaxStallGenerations',cfg.generations+1, ...
        'FunctionTolerance',0, ...
        'UseParallel',false, ...
        'UseVectorized',false, ...
        'Display','off');
    options.OutputFcn = @recordGeneration;
    history = zeros(cfg.generations+1,1);
    fractions = history;
    counts = history;
    bestP = cfg.baseU/cfg.uMax;
    bestJ = controlCost(cfg.baseU,cfg);
    objectiveCalls = 0;
    constraintCalls = 0;
    lb = -ones(1,cfg.segments);
    ub = ones(1,cfg.segments);
    nonlcon = [];
    if setting.variant==2
        nonlcon = @constraints;
    end
    [p,fval,exitflag,output,population] = ga(@objective,cfg.segments, ...
        [],[],[],[],lb,ub,nonlcon,options);
    [selectedJ,selectedV,selectedRatios] = evaluate(p,cfg,model);
    out.bestJ = bestJ;
    out.bestP = bestP;
    out.selectedP = p;
    out.selectedJ = selectedJ;
    out.selectedV = selectedV;
    out.selectedRatios = selectedRatios;
    out.fval = fval;
    out.exitflag = exitflag;
    out.output = output;
    out.history = history;
    out.feasibleFraction = fractions;
    out.evaluationHistory = counts;
    out.evaluations = output.funccount;
    out.objectiveCalls = objectiveCalls;
    out.constraintCalls = constraintCalls;
    out.population = population;

    function value = objective(p)
        objectiveCalls = objectiveCalls+1;
        if setting.variant==1
            value = penaltyObjective(p,cfg,model);
        else
            value = controlCost(decodeCommand(p,cfg.uMax),cfg);
        end
    end

    function [c,ceq] = constraints(p)
        constraintCalls = constraintCalls+1;
        [c,ceq] = craneConstraints(p,cfg,model);
    end

    function [state,options,changed] = recordGeneration(options,state,flag)
        changed = false;
        [J,~,ratios] = evaluate(state.Population,cfg,model);
        valid = all(ratios<=1+cfg.tol,2);
        ids = find(valid);
        if ~isempty(ids)
            [value,where] = min(J(ids));
            if value<bestJ
                bestJ = value;
                bestP = state.Population(ids(where),:);
            end
        end
        index = state.Generation+1;
        history(index) = bestJ;
        fractions(index) = mean(valid);
        counts(index) = state.FunEval;
    end
end

function [J,V,ratios] = evaluate(P,cfg,model)
    U = decodeCommand(P,cfg.uMax);
    trace = simulateCrane(U,model);
    J = controlCost(U,cfg);
    [V,ratios] = constraintViolation(trace,cfg);
end

function printSummary(trials,cfg,strategies)
    baseJ = controlCost(cfg.baseU,cfg);
    for j=1:numel(strategies)
        runs = [trials{j,:}];
        fprintf('%s: медиана J/J0 = %.3f; V ответа ga = %.3g; счётчик ga = %.0f; вызовов цели = %.0f; exitflag = %s\n', ...
            strategies(j).label,median([runs.bestJ])/baseJ, ...
            median([runs.selectedV]),median([runs.evaluations]),median([runs.objectiveCalls]),mat2str([runs.exitflag]));
    end
end

%% Рисунки — готовая часть
function drawResults(trials,cfg,model,strategies,variant)
    colors=lines(numel(strategies));
    labels=string({strategies.label});
    base=simulateCrane(cfg.baseU,model);
    baseJ=controlCost(cfg.baseU,cfg);
    selected=cell(1,numel(strategies));
    commands=zeros(numel(strategies),cfg.segments);
    for j=1:numel(strategies)
        commands(j,:)=decodeCommand(trials{j,1}.bestP,cfg.uMax);
        selected{j}=simulateCrane(commands(j,:),model);
    end
    suffix="Вариант "+variant+", вес плавности "+cfg.smoothWeight+", популяция "+cfg.population;
    figure('Name','Движение и управление','Color','w','Position',[60 50 1250 850]);
    tiledlayout(2,2,'TileSpacing','compact');
    fields={'x','v','theta'};
    units={'Положение тележки, м','Скорость, м/с','Угол груза, градусы'};
    for panel=1:3
        nexttile;
        hold on;
        scale=1;
        if panel==3
            scale=180/pi;
        end
        plot(model.t,base.(fields{panel})*scale,'--k','LineWidth',1.5);
        for j=1:numel(strategies)
            plot(model.t,selected{j}.(fields{panel})*scale,'Color',colors(j,:),'LineWidth',1.3);
        end
        xline(cfg.moveTime,':k','Остановка','HandleVisibility','off');
        if panel==1
            yline(cfg.distance,':k','HandleVisibility','off');
        end
        if panel==2
            yline(cfg.vMax,':k','HandleVisibility','off');
        end
        if panel==3
            limitTime=[0 cfg.moveTime cfg.moveTime cfg.moveTime+cfg.tailTime];
            limitAngle=[cfg.angleMax cfg.angleMax cfg.residualMax cfg.residualMax]*180/pi;
            plot(limitTime,limitAngle,':k','HandleVisibility','off');
            plot(limitTime,-limitAngle,':k','HandleVisibility','off');
        end
        xlabel('Время, с');
        ylabel(units{panel});
        grid on;
    end
    nexttile;
    hold on;
    times=[0:cfg.moveTime/cfg.segments:cfg.moveTime, cfg.moveTime+cfg.tailTime];
    stairs(times,[cfg.baseU 0 0],'--k','LineWidth',1.5);
    for j=1:numel(strategies)
        stairs(times,[commands(j,:) 0 0],'Color',colors(j,:),'LineWidth',1.3);
    end
    xlabel('Время, с');
    ylabel('Ускорение, м/с^2');
    grid on;
    legend(["Исходная команда" labels],'Location','best','FontSize',8);
    sgtitle({char("Допустимые рекорды первого запуска, seed = "+cfg.seeds(1)),char(suffix)});

    figure('Name','Ход поиска и повторяемость','Color','w','Position',[80 80 1250 550]);
    tiledlayout(1,2,'TileSpacing','compact');
    nexttile;
    hold on;
    for j=1:numel(strategies)
        history=zeros(cfg.generations+1,numel(cfg.seeds));
        for k=1:numel(cfg.seeds)
            history(:,k)=trials{j,k}.history;
        end
        plot(0:cfg.generations,median(history,2)/baseJ, ...
            'Color',colors(j,:),'LineWidth',1.5);
    end
    yline(1,'--k','Исходная команда','HandleVisibility','off');
    xlabel('Поколение');
    ylabel('Медиана допустимого рекорда J/J_0');
    grid on;
    legend(labels,'Location','best','FontSize',8);
    nexttile;
    hold on;
    for j=1:numel(strategies)
        values=zeros(1,numel(cfg.seeds));
        for k=1:numel(cfg.seeds)
            values(k)=trials{j,k}.bestJ/baseJ;
        end
        showPoints(j,values,colors(j,:));
    end
    categories(labels);
    ylabel('Допустимые итоги J/J_0');
    sgtitle({char("Меньше — лучше. Точка — запуск; черта и число — медиана"),char(suffix)});

    figure('Name','Допустимость','Color','w','Position',[100 100 1250 550]);
    tiledlayout(1,2,'TileSpacing','compact');
    nexttile;
    hold on;
    for j=1:numel(strategies)
        history=zeros(cfg.generations+1,numel(cfg.seeds));
        for k=1:numel(cfg.seeds)
            history(:,k)=trials{j,k}.feasibleFraction;
        end
        plot(0:cfg.generations,100*median(history,2),'Color',colors(j,:),'LineWidth',1.5);
    end
    xlabel('Поколение');
    ylabel('Допустимые особи, %');
    ylim([0 103]);
    grid on;
    legend(labels,'Location','best','FontSize',8);
    nexttile;
    hold on;
    for j=1:numel(strategies)
        values=zeros(1,numel(cfg.seeds));
        for k=1:numel(cfg.seeds)
            values(k)=trials{j,k}.selectedV;
        end
        showPoints(j,values,colors(j,:));
    end
    categories(labels);
    ylabel('Нарушение V выбранного кандидата');
    sgtitle({char("Выбранный по правилу метода кандидат может отличаться от допустимого рекорда"),char(suffix)});

    figure('Name','Выполнение ограничений','Color','w','Position',[100 100 1250 560]);
    ratios=zeros(numel(strategies)+1,6);
    [~,ratios(1,:)]=constraintViolation(base,cfg);
    for j=1:numel(strategies)
        [~,ratios(j+1,:)]=constraintViolation(selected{j},cfg);
    end
    bar(ratios');
    hold on;
    yline(1,'--k','Граница');
    xticks(1:6);
    xticklabels({'Точность','Остановка','Скорость','Угол в пути','Остаточный угол','Коридор'});
    ylabel('Отношение показателя к допустимому значению');
    ylim([0 1.14]);
    grid on;
    legend(["Исходная команда" labels],'Location','southoutside','NumColumns',3,'FontSize',8);
    title({char("Ограничения допустимых рекордов первого запуска"),char(suffix)});

    figure('Name','Проверка модели и длины подвеса','Color','w','Position',[120 100 1250 560]);
    checkIndex=3-(variant==2);
    U=commands(checkIndex,:);
    nominal=selected{checkIndex};
    nonlinear=nonlinearTrace(U,cfg,cfg.length,0.005);
    changed=nonlinearTrace(U,cfg,1.3*cfg.length,0.005);
    tiledlayout(1,2,'TileSpacing','compact');
    nexttile;
    hold on;
    plot(model.t,nominal.theta*180/pi,'k-','LineWidth',1.5);
    plot(nonlinear.t,nonlinear.theta*180/pi,'b--','LineWidth',1.3);
    plot(changed.t,changed.theta*180/pi,'r-','LineWidth',1.3);
    xline(cfg.moveTime,':k','Остановка','HandleVisibility','off');
    limitTime=[0 cfg.moveTime cfg.moveTime cfg.moveTime+cfg.tailTime];
    limitAngle=[cfg.angleMax cfg.angleMax cfg.residualMax cfg.residualMax]*180/pi;
    plot(limitTime,limitAngle,':k','HandleVisibility','off');
    plot(limitTime,-limitAngle,':k','HandleVisibility','off');
    xlabel('Время, с');
    ylabel('Угол, градусы');
    grid on;
    legend('Линейная, L = 1 м','Нелинейная, L = 1 м','Нелинейная, L = 1.3 м','Location','best','FontSize',8);
    nexttile;
    hold on;
    plot(nonlinear.t,nonlinear.x+cfg.length*sin(nonlinear.theta),'b--','LineWidth',1.4);
    plot(changed.t,changed.x+1.3*cfg.length*sin(changed.theta),'r-','LineWidth',1.4);
    yline(cfg.distance,':k','Цель','HandleVisibility','off');
    xline(cfg.moveTime,':k','HandleVisibility','off');
    xlabel('Время, с');
    ylabel('Горизонтальное положение груза, м');
    grid on;
    legend('L = 1 м','L = 1.3 м','Location','best');
    sgtitle({char("Та же команда без перенастройки: "+strategies(checkIndex).label+", seed = "+cfg.seeds(1)),char(suffix)});
end

function showPoints(index,values,color)
    scatter(index+linspace(-.15,.15,numel(values)),values,35,color,'filled');
    plot(index+[-.2 .2],[median(values) median(values)],'k-','LineWidth',1.4);
    text(index,median(values),sprintf(' %.3g',median(values)), ...
        'HorizontalAlignment','center','VerticalAlignment','bottom','FontSize',9);
end

function categories(labels)
    xticks(1:numel(labels));
    xticklabels(labels);
    xtickangle(18);
    xlim([.5 numel(labels)+.5]);
    grid on;
    limits=ylim;
    span=max(limits(2)-limits(1),.05);
    ylim([min(0,limits(1)-.03*span),limits(2)+.15*span]);
end

function trace=nonlinearTrace(U,cfg,length,dt)
    trace.t=0:dt:cfg.moveTime+cfg.tailTime;
    z=zeros(4,1);
    history=zeros(4,numel(trace.t));
    holdSteps=round(cfg.moveTime/cfg.segments/dt);
    for k=1:numel(trace.t)-1
        u=0;
        if k<=round(cfg.moveTime/dt)
            u=U(ceil(k/holdSteps));
        end
        k1=derivative(z,u,length);
        k2=derivative(z+dt*k1/2,u,length);
        k3=derivative(z+dt*k2/2,u,length);
        k4=derivative(z+dt*k3,u,length);
        z=z+dt*(k1+2*k2+2*k3+k4)/6;
        history(:,k+1)=z;
    end
    trace.x=history(1,:);
    trace.v=history(2,:);
    trace.theta=history(3,:);
    trace.omega=history(4,:);
end

function dz=derivative(z,u,length)
    dz=[z(2);u;z(4);-(9.81*sin(z(3))+u*cos(z(3)))/length];
end
