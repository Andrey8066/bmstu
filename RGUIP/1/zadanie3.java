public class zadanie3 {
    public static double f(double x){
        return (x+99.9)*(x-100.0001);
    }
    public static void main(String[] args) {
        double l = -100.0;
        double r = 100.0;
        double m = 0;
        
        while ((f(l)*f(r))<0){
            m = (l+r)/2;
            if (f(l)*f(m)<0){
             r = m;
            }
            else {
                l = m;
            }
            
            
            
        }
        
        System.out.print((l+r)/2);
            
            
        

    }
}
