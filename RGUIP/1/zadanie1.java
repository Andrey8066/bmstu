public class zadanie1 {
    public static boolean isLucky(int num) {
        String s = String.format("%06d", num);
        int sum1 = s.charAt(0)+s.charAt(1)+s.charAt(2);

        int sum2 = s.charAt(3)+s.charAt(4)+s.charAt(5);
        return sum1==sum2;
    }
    public static void main(String[] args) {
        int k = 0;
        for (int i=0; i<10000000;i++){
            if (isLucky(i)) ++k;
                
            

        }
        System.out.print(k);
    }
    
}