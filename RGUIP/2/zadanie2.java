import java.util.Scanner;

public class zadanie2 {
    private static boolean is_simple(long x){
        if (x==2) return true;
        if (x%2==0) return false;
        for (long i = 3; i <= Math.sqrt(x+1); i+=2){
            if (x%i == 0) {
                return false;
            }
            
        }
        return true;
    }
    public static void main(String[] args) {
        Scanner sc = new Scanner(System.in);
        System.out.println("Введите число");
        long x = sc.nextLong();

        if (is_simple(x)) System.out.println("Число простое");
        
        else 
        {
            for (long i = 2; i <= 6; i+=1){
                if (x%(i) == 0) if(is_simple(i)) System.out.println(i);
            }
            for (long i = 6; i < x+1; i+=6){
            if (x%(i+1) == 0) if(is_simple(i+1)) System.out.println(i+1);
            if (x%(i-1) == 0) if(is_simple(i-1)) System.out.println(i-1);
            }
        }
        sc.close();
    }
}
