import java.lang.Math;
import java.util.InputMismatchException;
import java.util.Scanner;

public class zadanie3 {
    Prism prisma;

    public void main(String[] args) {
        create();
        System.out.println("Площадь боковой поверхности равна " + prisma.square());
        System.out.println("Объем призмы " + prisma.volume());

    }

    protected int create() {
        Scanner sc = new Scanner(System.in);
        double a, b, c, h;
        double H;
        while (true) {
            try {
                System.out.println("Введите сторону основания a: ");
                a = sc.nextDouble();
                System.out.println("Введите сторону основания b: ");
                b = sc.nextDouble();
                System.out.println("Введите сторону основания c: ");
                c = sc.nextDouble();
                System.out.println("Введите боковую сторону h: ");
                h = sc.nextDouble();
                H = 90;
                prisma = new Prism(a, b, c, h, H);
                sc.close();
                return 0;
            } catch (RuntimeException e) {
                System.out.println("Вы ввели неверные данные попробуйте еще раз");
            }
        }
        
    }

}

class Prism {
    double a, b, c, h;
    double H;

    public Prism(double a, double b, double c, double h, double H) {
        if (a + b <= c || a + c <= b || b + c <= a)
            throw new RuntimeException("Заданный треугольник не существует");

        this.a = a;
        this.b = b;
        this.c = c;
        this.h = h;
        this.H = H;
    }

    protected double square() {
        return (a + b + c) * h;
    }

    protected double volume() {
        double per = a + b + c;
        return Math.pow(per * (per - a) * (per - b) * (per - c), 0.5) * h * Math.sin(H);
    }
}