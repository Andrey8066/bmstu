import java.util.ArrayList;
import java.util.Scanner;

public class zadanie {
    public void main(String[] args) {setData();}


    protected int setData() {
        System.out.println("Сколько всего комнат?");
        Scanner sc = new Scanner(System.in);
        int n = sc.nextInt();
        Graph g = new Graph(n);

        while (true) {
            System.out.println("1. Добавить проход\n2. Рассчитать длину кратчайшего пути\n3. Посмотреть таблицу графа\n0. Выход");
            switch (sc.nextInt()) {
                case 1:
                    g.addEdge(setEdge());
                    break;
                case 2:
                    System.out.println("Введите номер начальной комнаты: ");
                    g.claculatePath(sc.nextInt());
                    System.out.println("Введите номер конечной комнаты: ");
                    g.print(sc.nextInt());
                    break;
                case 3:
                    g.printAll();
                    break;
                case 0:
                    return 0;
                default:
                    break;
            }
        }
    }

    protected int[] setEdge() {
        int[] res = new int[3];
        Scanner sc = new Scanner(System.in);
        System.out.print("Введите номер 1 комнаты: ");
        res[0] = sc.nextInt();
        System.out.print("Введите номер 2 комнаты: ");
        res[1] = sc.nextInt();
        System.out.print("Введите расстояние м/у ними: ");
        res[2] = sc.nextInt();
        return res;
    }
}

class Graph {
    protected ArrayList<Vertex> Vertexes = new ArrayList<Vertex>();
    protected int paths[][];
    protected int shortestPaths[];
    protected int numbers;

    public Graph(int numbers, ArrayList<int[]> ways) {
        this.numbers = numbers;
        this.paths = new int[numbers][numbers];
        this.shortestPaths = new int[numbers];
        for (int i = 0; i < this.numbers; i++) {
            for (int j = 0; j < this.numbers; j++) {
                paths[i][j] = Integer.MAX_VALUE;
            }
            shortestPaths[i] = Integer.MAX_VALUE;
            Vertexes.addLast(new Vertex(Integer.toString(i)));
        }

        print(this.paths);

    }

    public Graph(int numbers) {
        this.numbers = numbers;
        this.paths = new int[numbers][numbers];
        this.shortestPaths = new int[numbers];
        for (int i = 0; i < this.numbers; i++) {
            for (int j = 0; j < this.numbers; j++) {
                paths[i][j] = Integer.MAX_VALUE;
            }
            shortestPaths[i] = Integer.MAX_VALUE;
            Vertexes.addLast(new Vertex(Integer.toString(i)));
        }

        print(this.paths);

    }

    public void addEdge(int[] input) {
        this.paths[input[0]][input[1]] = input[2];
        this.paths[input[1]][input[0]] = input[2];
    }

    public void claculatePath(int vertIndex, int pastPath, ArrayList<Integer> history) {
        for (int i = 0; i < this.numbers; i++) {
            if (paths[vertIndex][i] < Integer.MAX_VALUE && history.indexOf(i) == -1) {
                if (shortestPaths[i] > pastPath + paths[vertIndex][i]) {
                    history.addLast(i);
                    shortestPaths[i] = pastPath + paths[vertIndex][i];
                    claculatePath(i, shortestPaths[i], history);
                    history.removeLast();
                }
            }
        }
    }

    public void claculatePath(int vertIndex) {
        claculatePath(vertIndex, 0, new ArrayList<Integer>());
    }

    public void print() {
        for (int i = 0; i < this.numbers; i++) {
            System.out.print(this.shortestPaths[i] + " ");
        }
    }

    public void print(int x) {
        if (this.shortestPaths[x] == Integer.MAX_VALUE) {
            System.out.println("Сокровище недостижимо");
        }
        else {
            System.out.println("Длина кратчайшего пути до сокровища " + this.shortestPaths[x]);
        }
    }

    public void print(int mas[][]) {
        for (int i = 0; i < this.numbers; i++) {
            for (int j = 0; j < this.numbers; j++) {
                System.out.print(mas[i][j] + " ");
            }
            System.out.print("\n");
        }
    }

    public void printAll(){
        print(this.paths);
    }

    public int min(int a, int b) {
        if (a < b)
            return a;
        return b;
    }

}

class Vertex {
    protected String name;
    public Boolean isVisited = false;

    public Vertex(String name) {
        this.name = name;
    }

    public void visit() {
        this.isVisited = true;
    }
}