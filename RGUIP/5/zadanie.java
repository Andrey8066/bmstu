import java.util.ArrayList;
import java.util.Scanner;

public class zadanie {
    public  void main() {
        menu();
    }

    public int menu(){
        Scanner sc = new Scanner(System.in);
        int n = 10;
        int k = 3;
        while (true) {
            System.out.println("1. Задать размер поля\n2. Задать частоту обновления поля\n3. Запустить игру\n0. Выход");
            switch (sc.nextInt()) {
                case 1:
                    System.out.println("Введите количество полей по вертикали: ");
                    n = sc.nextInt();
                    break;
                case 2:
                    System.out.println("Ввкдите количество шагов между обновлениями: ");
                    k = sc.nextInt();
                    break;
                case 3:
                    Map m = new Map(n, k);
                    m.game();
                case 0:
                    return 0;
                default:
                    break;
            }
        }
    }

}

class Map {
    protected Cell[][] map;
    protected int updateTimes;
    protected int countOpen = 0;
    protected int steps = 0;

    public Map(int n, int k) {
        this.map = new Cell[n][n];
        this.updateTimes = k;
        int countBombs = 0;

        for (int x = 0; x < n; x++) {
            for (int y = 0; y < n; y++) {
                map[x][y] = new Cell(x, y, ((int) Math.random() * 10) % 10 == 0);
                if (map[x][y].isBomb())
                    countBombs += 1;
            }
        }
        double percent = countBombs / map.length * map.length;
        if (!(0.075 < percent && percent < 0.0125))
            rebomb();
    }

    protected int rebomb() {
        double countBombs = 0;
        for (int x = 0; x < map.length; x++) {
            for (int y = 0; y < map.length; y++) {
                if (!this.map[x][y].isOpen()) {
                    map[x][y].setBomb(((int) (Math.random() * 10)) % 10 == 0);
                    if (map[x][y].isBomb())
                        countBombs += 1;
                }
            }
        }
        double percent = countBombs / ((map.length * map.length) - countOpen);
        if (!(0.05 < percent && percent < 0.15))
            return rebomb();
        for (int x = 0; x < map.length; x++) {
            for (int y = 0; y < map.length; y++) {
                for (int i = -1; i < 2; i++)
                    for (int j = -1; j < 2; j++)
                        if (x + i >= 0 && y + j >= 0 && x + i <= map.length - 1 && y + j <= map.length - 1)
                            this.map[x][y].addNearest(x + i, y + j, this.map[x + i][y + j].isBomb());
                if (map[x][y].isBomb())
                    countBombs += 1;
            }
        }
        return 0;

    }

    public int openCell(int x, int y) {
        System.out.println(map[x][y].countBombs());
        map[x][y].setOpen(true);
        this.countOpen+=1;
        if (map[x][y].isBomb())
            return 1;
        return 0;
    }

    public String[][] generateMap() {
        String[][] res = new String[map.length][map.length];
        for (int x = 0; x < map.length; x++) {
            for (int y = 0; y < map.length; y++) {
                if (!this.map[x][y].isOpen())
                    res[x][y] = "#";
                else if (this.map[x][y].isOpen() && this.map[x][y].isBomb())
                    res[x][y] = "*";
                else
                    res[x][y] = Integer.toString(this.map[x][y].countBombs());
            }
        }
        return res;
    }

    public int printMap() {
        String[][] output = generateMap();
        for (int x = 0; x < output.length; x++) {
            for (int y = 0; y < output.length; y++) {
                System.out.print(output[x][y] + " ");
            }
            System.out.println();
        }
        return 0;
    }

    public int game() {
        Scanner sc = new Scanner(System.in);
        int x;
        int y;
        while (true) {
            System.out.print("Введите x клетки которую хотите открыть: ");
            x = sc.nextInt();
            System.out.print("Введите y клетки которую хотите открыть: ");
            y = sc.nextInt();
            if (openCell(y, x) == 1) {
                printMap();
                System.out.println("Вы проиграли");
                return 1;
            } else
                printMap();
            this.steps += 1;
            if (this.steps % this.updateTimes == 0)
                rebomb();
        }
    }
}

class Cell {
    protected int x;
    protected int y;
    protected Boolean isBomb;
    protected Boolean isOpen;
    protected ArrayList<Cell> nearest = new ArrayList<Cell>();

    public int addNearest(int x, int y, Boolean bomb) {
        nearest.addLast(new Cell(x, y, bomb));
        return 0;
    }

    public Cell(int x, int y, Boolean bomb) {
        this.x = x;
        this.y = y;
        this.isBomb = bomb;
        this.isOpen = false;
    }

    public int countBombs() {
        int res = 0;
        for (int i = 0; i < this.nearest.size(); i++) {
            if (this.nearest.get(i).isBomb())
                res += 1;
        }
        return res;
    }

    public int clearNearest() {
        nearest = new ArrayList<Cell>();
        return 0;
    }

    protected boolean isBomb() {
        return this.isBomb;
    }

    protected boolean isOpen() {
        return this.isOpen;
    }

    public int setBomb(boolean b) {
        this.isBomb = b;
        return 0;
    }

    public int setOpen(boolean b) {
        this.isOpen = b;
        return 0;
    }
}