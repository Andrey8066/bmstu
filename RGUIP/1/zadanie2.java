import java.util.Scanner;
public class zadanie2 {
    
    public static void main(String args[])
    {
        Scanner sc = new Scanner(System.in);
        System.out.println("Введите шаг");
        double dx = sc.nextDouble();
        System.out.println("Введите a");
        double a = sc.nextDouble();
        System.out.println("Введите b");
        double b = sc.nextDouble();
        double square = 0;
        for (double x = a; x < b; x+=dx){
            square += dx * (x*x*x + x + 1);
        }
        System.out.println(square);
        sc.close();
        

    }
}
