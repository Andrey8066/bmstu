import java.util.Scanner;

import javax.management.RuntimeErrorException;

public class zadanie1 {
    public static void main(String[] args) {
        Scanner sc = new Scanner(System.in);
        double x;
        try {
            System.out.println("Введите число");
            x = (double) sc.nextLong();
            sc.close();
            if (x<0) throw new RuntimeException("Неверное число");
            double[] a = new double[3];
            a[0] = 1;
            a[1] = 1;
            a[2] = 2;
            while (a[2] < x) {
                a[0] = a[1];
                a[1] = a[2];
                a[2] = a[0] + a[1];
            }
            if (a[2] == x) {
                System.out.println("Число принадлежит последовательности");
            } else {
                System.out.println("Число не принадлежит последовательности");
            }
            
        } catch (RuntimeException e) {
            System.out.println("Введено неверное число, попробуйте еще раз");
        }
        
    }
}
