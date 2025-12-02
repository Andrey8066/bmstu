
import java.io.File;
import java.io.FileNotFoundException;
import java.util.ArrayList;
import java.util.Scanner;

public class laba {
    protected ArrayList<Double> mas = new ArrayList<>();

    protected void split(String str) { // разделение строки по запятым
        String[] numbers = str.split(",");
        for (String number : numbers) {
            mas.addLast(Double.parseDouble(number));
        }

    }

    protected void print(ArrayList<Double> a) { // вывод массива
        System.out.print("Массив: [");
        for (int i = 0; i < a.size() - 1; i++) {
            System.out.print(a.get(i));
            System.out.print(", ");
        }
        System.out.print(a.getLast());
        System.out.println("]");
    }

    protected double sum() { // вычисление суммы эллементов массива
        double r = 0;
        for (int i = 0; i < mas.size(); i++) {
            r += mas.get(i);
        }
        return r;
    }

    protected double avg() { // вычисление математического ожидания
        return sum() / mas.size();
    }

    protected double max() { // поиск максимального эллемента массива
        double max = mas.get(0);
        for (int i = 0; i < mas.size(); i++)
            if (mas.get(i) > max)
                max = mas.get(i);
        return max;
    }

    protected double min() { // поиск минимального эллемента массива
        double min = mas.get(0);
        for (int i = 0; i < mas.size(); i++)
            if (mas.get(i) < min)
                min = mas.get(i);
        return min;
    }

    protected double dispersion() { // вычисление дисперсии
        double r = 0;
        for (int i = 0; i < mas.size(); i++) {
            r += (mas.get(i) - avg()) * (mas.get(i) - avg());
        }
        return r / mas.size();
    }

    private int part(ArrayList<Double> arr, int l, int r) { // реализация деления
        double x = arr.get(r);
        int i = l - 1;
        for (int j = l; j < r; j++) {
            if (arr.get(j) < x) {
                i++;
                double tempi = arr.get(i);
                double tempj = arr.get(j);
                arr.set(i, tempj);
                arr.set(j, tempi);

            }
        }
        double tempi = arr.get(i + 1);
        arr.set(i + 1, arr.get(r));
        arr.set(r, tempi);
        return i + 1;
    }

    private void sort(ArrayList<Double> arr, int l, int r) { // реализация метода quicksort
        if (l < r) {
            int p = part(arr, l, r);
            sort(arr, l, p - 1);
            sort(arr, p + 1, r);

        }

    }

    public void main(String[] args) throws FileNotFoundException {
        // считывание массива
        File f = new File("input.txt");
        Scanner sc = new Scanner(f);
        ArrayList<String> input = new ArrayList<>();
        while (sc.hasNextLine()) {
            input.add(sc.nextLine());
        }
        sc.close();
        // преобразование массива
        if (input.size() > 1)
            for (int i = 0; i < input.size(); i++)
                mas.addLast(Double.parseDouble(input.get(i)));
        else
            split(input.get(0));

        print(mas); // вывод массива
        System.out.println(sum()); // вывод суммы элементов массива
        System.out.println(avg()); // вывод математичского ожидания массива
        System.out.println(min()); // вывод минимального элементов массива
        System.out.println(max()); // вывод максимального элементов массива
        System.out.println(dispersion()); // вывод дисперсии элементов массива
        sort(mas, 0, mas.size() - 1); // сортировка массива
        print(mas);  // вывод отсортированного массива
    }
}
