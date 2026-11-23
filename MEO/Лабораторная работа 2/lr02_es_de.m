%% ЛР2. Эволюционная стратегия и дифференциальная эволюция
% Выполнил(а): Соин Андрей Дмитриевич, Величко Федор Тимофеевич
% Вариант 1 — ES: заполните только E1, E2, E3.
% Вариант 2 — DE: заполните только D2, D3, D4.
% Функции другого варианта оставьте как есть: они не вызываются.
% Выполните весь файл и покажите графики на защите.
% Случайные числа переданы аргументами: новые rand/randn в блоках не нужны.

%% Настройки опыта
clear cfg settings trials;
close all;
variant = 1;                % Только 1 (ES) или 2 (DE)
cfg.dimension = 5; % размерность пространства поиска
cfg.budget = 4000; % максимально допустимое число обращений к целевой функции за один запуск
cfg.repeats = 5; % количество независимых повторов ( позволяет оценить ср результаь и разброс)

if variant == 1
    cfg.sigma0Rel = 0.05;    % Начальный шаг: доля средней ширины области
    cfg.window = 20;         % Предложений между изменениями шага
    cfg.c = 0.85;            % Коэффициент изменения шага

    settings(1).adaptive = false;
    settings(1).label = "Фиксированный шаг";
    settings(2).adaptive = true;
    settings(2).label = "Правило 1/5";
    search = @runES;
elseif variant == 2
    cfg.population = 40;
    cfg.CR = 0.8;
    fValues = [0.3, 0.6, 0.9];

    for j = 1:numel(fValues)
        settings(j).F = fValues(j);
        settings(j).label = "F = " + fValues(j);
    end
    search = @runDE;
else
    error('Укажите variant = 1 для ES или variant = 2 для DE.');
end

%% Запуск и графики — готовая часть
% Для сравниваемых настроек используются одинаковые начальные точки
% или популяции. Пять повторов имеют разные случайные последовательности.
baseSeed = 20000 + 1000 * variant;
trials = runExperiments(search, settings, cfg, baseSeed);
printConsoleReport(trials, cfg);
drawResults(trials, cfg.dimension);
if variant == 1
    drawSigma(trials);
end

%% Вариант 1. Три функции ES для заполнения
function y = esProposal(x, sigma, z, lb, ub)
    % E1. x и z — строки 1 x d; sigma — положительное число.
    % z содержит стандартные нормальные случайные числа.
    % Получите x + sigma * z и ограничьте точку через clipPoint.
    y = clipPoint(x + sigma * z, lb, ub);
end

function [nextX, nextF, success] = esAccept(x, fx, y, fy)
    % E2. x, fx — родитель и его значение; y, fy — новая точка.
    % При fy < fx верните y, fy, true.
    % Иначе, включая равенство, верните x, fx, false.
    % Функцию повторно вычислять не нужно.
    if fy < fx
        nextX = y;
        nextF = fy;
        success = true;
    else
        nextX = x;
        nextF = fx;
        success = false;
    end
end

function sigma = adaptSigma(sigma, successes, window, c, sigmaMin, sigmaMax)
    % E3. successes — число успехов за полные window предложений.
    % Если доля успехов > 1/5, разделите sigma на c.
    % Если доля < 1/5, умножьте sigma на c; при равенстве не меняйте.
    % Ограничьте результат интервалом [sigmaMin, sigmaMax].
    if successes / window > 1/5
        sigma = sigma / c;
    elseif successes / window < 1/5
        sigma = sigma * c;
    end
    sigma = clipPoint(sigma, sigmaMin, sigmaMax);
end

%% Вариант 2. Три функции DE для заполнения
function donor = deDonor(P, ids, F)
    % D2. P — популяция N x d; ids = [r1, r2, r3] уже выбраны.
    % Верните P(r1,:) + F * (P(r2,:) - P(r3,:)).
    % Пока не обрезайте координаты по границам.
    error('LAB:TODO_D2', 'Заполните D2: разностный вектор.');
end

function trial = deTrial(parent, donor, u, jrand, CR, lb, ub)
    % D3. Все векторы — строки 1 x d; u — числа из [0,1).
    % Маска замены: u < CR; координата jrand включается обязательно.
    % Скопируйте parent и перенесите отмеченные координаты из donor.
    % Ограничьте результат с помощью clipPoint.
    error('LAB:TODO_D3', 'Заполните D3: пробную точку DE.');
end

function [nextX, nextF] = deAccept(parent, parentF, trial, trialF)
    % D4. Сравните пробную точку со своим родителем.
    % При trialF < parentF верните trial, trialF.
    % Иначе, включая равенство, верните parent, parentF.
    error('LAB:TODO_D4', 'Заполните D4: замену в DE.');
end

%% Готовый цикл (1+1)-ES
function out = runES(fun, lb, ub, cfg, setting)
    d = numel(lb);
    count = 0;
    bestF = inf;
    bestX = nan(1, d);
    history = zeros(cfg.budget, 1);
    sigmaUsed = zeros(cfg.budget, 1);

    x = lb + rand(1, d) .* (ub - lb);
    fx = evaluate(x);
    width = mean(ub - lb);
    sigma = cfg.sigma0Rel * width;
    sigmaMin = 1e-6 * width;
    sigmaMax = width;
    sigmaUsed(1) = sigma;
    successes = 0;
    steps = 0;

    while count < cfg.budget
        y = esProposal(x, sigma, randn(1, d), lb, ub);
        fy = evaluate(y);
        sigmaUsed(count) = sigma;
        [x, fx, success] = esAccept(x, fx, y, fy);

        % Учёт окна уже готов; шаг меняется только после полного окна.
        successes = successes + double(success);
        steps = steps + 1;
        if steps == cfg.window
            if setting.adaptive
                sigma = adaptSigma(sigma, successes, cfg.window, ...
                    cfg.c, sigmaMin, sigmaMax);
            end
            successes = 0;
            steps = 0;
        end
    end

    out.bestX = bestX;
    out.bestF = bestF;
    out.evaluations = count;
    out.history = history;
    out.sigmaUsed = sigmaUsed;

    function value = evaluate(x)
        value = fun(x);
        count = count + 1;
        if value < bestF
            bestF = value;
            bestX = x;
        end
        history(count) = bestF;
    end
end

%% Готовый цикл DE/rand/1/bin
function out = runDE(fun, lb, ub, cfg, setting)
    d = numel(lb);
    count = 0;
    bestF = inf;
    bestX = nan(1, d);
    history = zeros(cfg.budget, 1);

    P = lb + rand(cfg.population, d) .* (ub - lb);
    score = zeros(cfg.population, 1);
    for i = 1:cfg.population
        score(i) = evaluate(P(i, :));
    end

    while count < cfg.budget
        % Все доноры строятся из прежнего поколения.
        oldP = P;
        oldScore = score;
        Q = P;
        nextScore = score;

        for i = 1:cfg.population
            if count >= cfg.budget
                break
            end

            % Три разных индекса, ни один не равен i: готовая часть.
            order = randperm(cfg.population);
            pool = order(order ~= i);
            ids = pool(1:3);

            donor = deDonor(oldP, ids, setting.F);
            trial = deTrial(oldP(i, :), donor, rand(1, d), ...
                randi(d), cfg.CR, lb, ub);
            trialF = evaluate(trial);
            [Q(i, :), nextScore(i)] = deAccept( ...
                oldP(i, :), oldScore(i), trial, trialF);
        end

        P = Q;
        score = nextScore;
    end

    out.bestX = bestX;
    out.bestF = bestF;
    out.evaluations = count;
    out.history = history;
    out.sigmaUsed = [];

    function value = evaluate(x)
        value = fun(x);
        count = count + 1;
        if value < bestF
            bestF = value;
            bestX = x;
        end
        history(count) = bestF;
    end
end

%% Готовая серия опытов
function trials = runExperiments(search, settings, cfg, baseSeed)
    % Две функции, несколько настроек и независимые повторы.
    trials = struct([]);
    counter = 0;

    for problemId = 1:2
        [fun, lb, ub, name] = objective(problemId, cfg.dimension);

        for condition = 1:numel(settings)
            for repetition = 1:cfg.repeats
                seed = baseSeed + 100 * problemId + repetition;
                rng(seed, 'twister');
                trial = search(fun, lb, ub, cfg, settings(condition));
                trial.problemId = problemId;
                trial.problemName = name;
                trial.condition = condition;
                trial.label = settings(condition).label;
                trial.repetition = repetition;

                counter = counter + 1;
                if counter == 1
                    trials = trial;
                else
                    trials(counter) = trial;
                end
            end
        end
    end
end

%% Готовые целевые функции и обработка границ
function [fun, lb, ub, name] = objective(id, d)
    % Обе функции минимизируются.
    if id == 1
        % Розенброк: минимум 0 при x = [1, ..., 1].
        fun = @(x) sum(100 * (x(2:end) - x(1:end-1).^2).^2 ...
            + (1 - x(1:end-1)).^2);
        lb = -2 * ones(1, d);
        ub = 2 * ones(1, d);
        name = "Розенброк";
    else
        % Растригин: минимум 0 при x = [0, ..., 0].
        fun = @(x) 10 * d + sum(x.^2 - 10 * cos(2 * pi * x));
        lb = -5.12 * ones(1, d);
        ub = 5.12 * ones(1, d);
        name = "Растригин";
    end
end

function y = clipPoint(x, lb, ub)
    y = max(lb, min(ub, x));
end

%% Вывод результатов и точек сходимости в консоль
function printConsoleReport(trials, cfg)
    fprintf('\n========================================================================================\n');
    fprintf('                   ОТЧЁТ ПО РЕЗУЛЬТАТАМ ЭКСПЕРИМЕНТОВ (d = %d, бюджет = %d)\n', ...
        cfg.dimension, cfg.budget);
    fprintf('========================================================================================\n');

    for problemId = 1:2
        subset = trials([trials.problemId] == problemId);
        probName = subset(1).problemName;
        if problemId == 1
            optStr = 'x* = [1, 1, 1, 1, 1], f* = 0';
        else
            optStr = 'x* = [0, 0, 0, 0, 0], f* = 0';
        end

        fprintf('\n>>> ЦЕЛЕВАЯ ФУНКЦИЯ: %s (%s)\n', probName, optStr);
        nConditions = max([subset.condition]);

        for c = 1:nConditions
            rows = subset([subset.condition] == c);
            fprintf('----------------------------------------------------------------------------------------\n');
            fprintf('  Настройка: %s\n', rows(1).label);
            fprintf('  %-6s | %-12s | %-15s | %-12s | %s\n', ...
                'Повтор', 'Итоговое f', 'Сходимость (шаг)', 'Финальный σ', 'Найденная точка bestX');
            fprintf('  --------------------------------------------------------------------------------------\n');

            convSteps = zeros(1, numel(rows));
            for r = 1:numel(rows)
                % Вычисление, на котором впервые было достигнуто итоговое лучшее значение
                convStep = find(rows(r).history == rows(r).bestF, 1, 'first');
                convSteps(r) = convStep;

                if ~isempty(rows(r).sigmaUsed)
                    finSigma = sprintf('%12.3e', rows(r).sigmaUsed(end));
                else
                    finSigma = '     —      ';
                end

                xStr = sprintf('%7.4f ', rows(r).bestX);
                fprintf('  %-6d | %12.6f | %15d | %s | [%s]\n', ...
                    rows(r).repetition, rows(r).bestF, convStep, finSigma, strtrim(xStr));
            end

            vals = [rows.bestF];
            fprintf('  --------------------------------------------------------------------------------------\n');
            fprintf('  Сводка по "%s":\n', rows(1).label);
            fprintf('    • Медиана f        = %.6f  (log10(1 + мед.f) = %.4f)\n', ...
                median(vals), log10(1 + median(vals)));
            fprintf('    • Среднее ± СКО    = %.6f ± %.6f\n', mean(vals), std(vals));
            fprintf('    • Разброс [min;max]= [%.6f; %.6f]\n', min(vals), max(vals));
            fprintf('    • Медианный шаг последнего улучшения = %d из %d вычислений\n', ...
                round(median(convSteps)), cfg.budget);
        end
    end
    fprintf('========================================================================================\n\n');
end

%% Готовые графики
function drawResults(trials, dimension)
    % Графики показываются на экране. Файлы не создаются.
    % log10(1 + f) сохраняет ноль и позволяет увидеть большой диапазон f.
    colors = [0.20, 0.38, 0.58; 0.73, 0.31, 0.22; 0.22, 0.56, 0.38];

    for problemId = 1:2
        subset = trials([trials.problemId] == problemId);
        nConditions = max([subset.condition]);
        labels = strings(1, nConditions);

        figure('Color', 'w', 'Name', char(subset(1).problemName), ...
            'Position', [100, 100, 1150, 470]);
        tiledlayout(1, 2, 'TileSpacing', 'compact');

        nexttile;
        hold on;
        for c = 1:nConditions
            rows = subset([subset.condition] == c);
            H = [rows.history];
            labels(c) = rows(1).label;
            plot(1:size(H, 1), log10(1 + median(H, 2)), ...
                'LineWidth', 1.7, 'Color', colors(c, :));
        end
        xlabel('Вычисления целевой функции');
        ylabel('log10(1 + лучшее f)');
        title('Медиана по повторам');
        legend(labels, 'Location', 'northeast');
        grid on;

        nexttile;
        hold on;
        for c = 1:nConditions
            rows = subset([subset.condition] == c);
            values = [rows.bestF];
            offset = linspace(-0.10, 0.10, numel(values));
            scatter(c + offset, log10(1 + values), ...
                45, colors(c, :), 'filled');
            middle = log10(1 + median(values));
            plot([c - 0.16, c + 0.16], [middle, middle], ...
                'Color', 'k', 'LineWidth', 2);
        end
        xticks(1:nConditions);
        xticklabels(labels);
        xtickangle(15);
        xlim([0.5, nConditions + 0.5]);
        ylabel('log10(1 + итоговое f)');
        title('Точки — запуски; чёрная черта — медиана');
        grid on;
        sgtitle(subset(1).problemName + "; n = " + dimension);
    end
end

%% Готовая история шага ES
function drawSigma(trials)
    for problemId = 1:2
        subset = trials([trials.problemId] == problemId);
        nConditions = max([subset.condition]);
        labels = strings(1, nConditions);

        figure('Color', 'w', 'Name', 'Шаг ES', ...
            'Position', [100, 100, 1000, 430]);
        hold on;
        for c = 1:nConditions
            rows = subset([subset.condition] == c);
            labels(c) = rows(1).label;
            plot(1:numel(rows(1).sigmaUsed), rows(1).sigmaUsed, ...
                'LineWidth', 1.6);
        end
        set(gca, 'YScale', 'log');
        xlabel('Вычисления целевой функции');
        ylabel('Использованный шаг sigma');
        title(subset(1).problemName + ": шаг в первом повторе");
        legend(labels, 'Location', 'best');
        grid on;
    end
end