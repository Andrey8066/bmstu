%% ЛР1. Генетический алгоритм
% Выполнил(а):Соин Андрей
% Один файл для всех четырёх вариантов.
% Заполните только G2, G3 и G4; правила своего варианта — в задании.
% Начальная популяция и новое поколение с элитой уже реализованы.
% Выполните весь файл и покажите графики на защите.
% Случайные числа переданы аргументами: новые rand/randn в блоках не нужны.

%% Настройки опыта
clear cfg settings trials;
close all;
variant = 3;                % Номер варианта из задания
cfg.dimension = 5;          % Число переменных
cfg.budget = 4000;          % Вычислений функции на один запуск
cfg.repeats = 5;            % Независимых повторов для каждой настройки
cfg.population = 40;
cfg.elite = 2;
cfg.tournament = 3;
cfg.pc = 0.9;
cfg.sigmaRel = 0.05;

pmValues = [0.02, 0.10, 0.30];
for j = 1:numel(pmValues)
    settings(j).pm = pmValues(j);
    settings(j).label = "pm = " + pmValues(j);
end

%% Запуск и графики — готовая часть
% seed задаёт повторяемую последовательность случайных чисел.
% При одинаковом seed разные настройки начинают с одной популяции.
baseSeed = 10000 + 1000 * variant;
trials = runExperiments(@runGA, settings, cfg, baseSeed);
drawResults(trials, cfg.dimension);

%% Три функции для самостоятельного заполнения
function ids = tournament(score, candidates)
    % G2. score — столбец значений функции; меньше — лучше.
    % Строка candidates содержит номера участников одного турнира.
    % Верните столбец номеров победителей в исходной популяции.
    % При равенстве выбирайте первого участника в строке.
    % Один и тот же номер может встретиться среди участников повторно.

    candidateScores = score(candidates);
    [~, minIdx] = min(candidateScores, [], 2);
    ids = candidates(sub2ind(size(candidates), (1:size(candidates, 1))', minIdx));

    % error('LAB:TODO_G2', 'Заполните G2: турнирный отбор.');
end

function [a, b] = crossPair(p, q, u)
    % G3. p, q — родители; u — равномерные числа из [0,1).
    % Все входы — строки 1 x d. Верните две строки потомков a и b.
    % Правило скрещивания возьмите из своего варианта в задании.
    alpha = u(1);
    a = alpha * p + (1 - alpha) * q;
    b = (1 - alpha) * p + alpha * q;
    
    % error('LAB:TODO_G3', 'Заполните G3: скрещивание.');
end

function y = mutatePoint(x, lb, ub, pm, sigmaRel, u, z, v)
    % G4. Все векторы — строки 1 x d; pm и sigmaRel — числа.
    % u, v — равномерные числа из [0,1); z — стандартные нормальные.
    % Изменяйте только координаты с u(j) < pm, остальные сохраните.
    % Величина изменения — по своему варианту в задании.
    % В конце примените готовую clipPoint(y, lb, ub).
    y = x; % Изменяем только координаты, для которых u(j) < pm 
    for j = 1:numel(u) 
        if u(j) < pm % Вариант 1: гауссовская мутация - прибавление 
            y(j) = lb(j) +  (ub(j) - lb(j)) * v(j); 
        end 
    end % Применяем clipPoint чтобы точка осталась в допустимой области 
    y = clipPoint(y, lb, ub);

    % error('LAB:TODO_G4', 'Заполните G4: мутацию.');
end

%% Готовый цикл генетического алгоритма
function out = runGA(fun, lb, ub, cfg, setting)
    d = numel(lb);
    count = 0;
    bestF = inf;
    bestX = nan(1, d);
    history = zeros(cfg.budget, 1);

    % Создание и оценка начальной популяции.
    P = initialPopulation(rand(cfg.population, d), lb, ub);
    score = zeros(cfg.population, 1);
    for i = 1:cfg.population
        score(i) = evaluate(P(i, :));
    end

    while count < cfg.budget
        % Последняя группа может быть меньше полного поколения.
        amount = min(cfg.population - cfg.elite, cfg.budget - count);
        children = zeros(amount, d);
        childScore = zeros(amount, 1);
        k = 1;

        while k <= amount
            candidates = randi(cfg.population, 2, cfg.tournament);
            ids = tournament(score, candidates);
            a = P(ids(1), :);
            b = P(ids(2), :);

            if rand < cfg.pc
                [a, b] = crossPair(a, b, rand(1, d));
            end

            pair = [a; b];
            for j = 1:2
                if k > amount
                    break
                end
                children(k, :) = mutatePoint(pair(j, :), lb, ub, ...
                    setting.pm, cfg.sigmaRel, rand(1, d), randn(1, d), rand(1, d));
                childScore(k) = evaluate(children(k, :));
                k = k + 1;
            end
        end

        % Лучшие точки из неполной группы уже учтены функцией evaluate.
        if amount < cfg.population - cfg.elite
            break
        end
        [P, score] = nextPopulation(P, score, children, childScore, cfg.elite);
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

%% Готовые создание популяции и переход к новому поколению
function P = initialPopulation(U, lb, ub)
    % Каждый столбец U переносится из [0,1) в свой допустимый диапазон.
    P = lb + U .* (ub - lb);
end

function [Q, nextScore] = nextPopulation(P, score, children, childScore, elite)
    % При равенстве значений сохраняем порядок исходных индексов.
    parentIndex = (1:size(P, 1))';
    [~, order] = sortrows([score, parentIndex], [1, 2]);
    keep = order(1:elite);

    Q = [P(keep, :); children];
    nextScore = [score(keep); childScore];
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
