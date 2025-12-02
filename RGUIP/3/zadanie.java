/*Семинар 3. Смоделировать работу АЗС. На заправку будут подъезжать автомобили, 
у каждого автомобиля свой объем бака, %его заполненности и желание заполнить бак
на сколько-то литров(нельзя налить больше, чем свободное место в баке). 
На заправке будет некоторое количество колонок, если есть свободная колонка,
то машина поедет туда. Если все заняты, то формируется очередь из автомобилей,
если время ождания автомобиля становится более 12 минут,
то через два дня открывают дополнительную колонку. 
Будет здорово сделать изменяющуюся нагрузку в разное время дня.
Нужно сделать так чтобы машины появлялись случайным образом в конкретный момент времени
с некоторой вероятностью. Стоит учесть, что количество бензина на заправке ограничено 
и его привозят только раз в какое-то время.  Пусть скорость заполнения бака будет 20 литров в минуту
и она может регулироваться для каждой колонки.
*/

import java.util.ArrayList;
import java.util.Scanner;

class Car {
    protected double volume;
    protected double percent;
    protected double need;
    protected int time_waiting;

    public Car(double v, double p, double n) {
        this.volume = v;
        this.percent = p;
        this.need = n;
        this.time_waiting = 0;
    }

    public void wait(int time) {
        this.time_waiting += time;
    }

    public int getTimeWaiting() {
        return time_waiting;
    }

    public void setTimeWaiting(int time) {
        this.time_waiting = 0;
    }

    public double refueling(double speed) {
        if (this.need < this.volume * (1 - this.percent))
            return this.need / speed;
        else
            return this.volume * (1 - this.percent) / speed;
    }

    public double getRealNeededVolume() {
        if (this.need < this.volume * (1 - this.percent))
            return this.need;
        else
            return this.volume * (1 - this.percent);
    }

    public String print() {
        return (this.volume + " " + this.percent + " " + this.need + " " + this.time_waiting);
    }
}

class Gasstation {
    protected int quantity;
    protected int maxWaiting = 0;
    protected int skipedCarsQuantity = 0;
    protected int allCarsQuantity = 0;
    protected int[] chances;
    protected double stock;
    protected ArrayList<Double> data = new ArrayList<Double>();
    protected ArrayList<Car> queue = new ArrayList<Car>();
    protected ArrayList<Gasoline> fueling = new ArrayList<Gasoline>();

    public Gasstation(int quantity, int maxWaiting, int[] chances) {
        this.quantity = quantity;
        this.maxWaiting = maxWaiting;
        this.chances = chances;

        for (int i = 0; i < quantity; i++) {
            this.fueling.addLast(new Gasoline(20));
        }
    }

    public void addCar(Car c) {
        queue.addLast(c);
        this.allCarsQuantity += 1;
    }

    public void wait(int time) {
        if (!this.queue.isEmpty()) {
            for (int i = 0; i < queue.size(); i++) {
                this.queue.get(i).wait(time);
            }
            // checkWaiting();
        }
        for (int i = 0; i < this.fueling.size(); i++) {
            this.fueling.get(i).refuel();
        }
    }

    public void addCars(double coeficient) {
        int cars_quantity = chances[(int) (Math.random() * 1000) % chances.length];
        for (int j = 0; j < cars_quantity * coeficient; j++) {
            double v = (int) (Math.random() * 1000) % 300;
            double p = ((int) ((Math.random() * 1000) % 100));
            p = p / 100;
            double n = (int) ((Math.random() * 1000) % (1.1 * v));
            addCar(new Car(v, p, n));
        }
    }

    public void saveData() {
        data.addLast((double) skipedCarsQuantity / allCarsQuantity);
        skipedCarsQuantity = 0;
        allCarsQuantity = 0;
    }

    public void printQueue() {
        if (!this.queue.isEmpty()) {
            for (int i = 0; i < this.queue.size(); i++) {
                System.out.println(i + " " + this.queue.get(i).print());
            }
        }
    }

    protected void checkRefuled() {
        for (int i = 0; i < this.fueling.size(); i++) {
            if (!queue.isEmpty() && this.stock - this.queue.getFirst().getRealNeededVolume() > 0) {
                if (this.fueling.get(i).Car != null) {
                    if (this.fueling.get(i).checkRefuled()) {
                        if (this.queue.getFirst().getTimeWaiting() > this.maxWaiting)
                            this.skipedCarsQuantity += 1;
                        this.stock -= this.queue.getFirst().getRealNeededVolume();
                        this.fueling.get(i).setCar(this.queue.getFirst());
                        this.queue.removeFirst();

                    }
                } else if (this.fueling.get(i).Car == null) {
                    if (this.queue.getFirst().getTimeWaiting() > this.maxWaiting)
                        this.skipedCarsQuantity += 1;
                    this.stock -= this.queue.getFirst().getRealNeededVolume();
                    this.fueling.get(i).setCar(this.queue.getFirst());
                    this.queue.removeFirst();

                }
            }
        }
    }

    public void replenish(double s) {
        this.stock = s;
    }

    public void clearQueue() {
        this.queue.clear();
    }

    public int getQuantity() {
        return this.quantity;
    }

    public String getTodayData() {
        return this.skipedCarsQuantity + " " + this.allCarsQuantity;
    }

    public double getYesterdayPercent() {
        if (data.size() > 1) {
            return data.get(data.size() - 1);
        }
        return -1;
    }

    public double getTodayPercent() {
        return (double) skipedCarsQuantity / allCarsQuantity;
    }

    public int getSkipedCarsQuantity() {
        return skipedCarsQuantity;
    }

    public int getAllCarsQuantity() {
        return allCarsQuantity;
    }

    /*
     * public double getTimeWaiting() {
     * double time_waiting = 0;
     * for (int i = 0; i < this.queue.size(); i += quantity) {
     * time_waiting += this.queue.get(i).refueling();
     * }
     * return time_waiting;
     * }
     */
    public void setQuantity(int n) {
        this.quantity = n;
        this.fueling.addLast(new Gasoline(20));
    }
}

class Gasoline {
    protected double speed;
    protected int timeOfRefueling;
    public boolean active = false;
    protected Car Car;

    public Gasoline(double speed) {
        this.speed = speed;
    }

    public void setActive(boolean a) {
        this.active = a;
    }

    public Car getCar() {
        return this.Car;
    }

    public int getTimeOfRefueling() {
        return timeOfRefueling;
    }

    public int getTimeWaiting() {
        return this.Car.getTimeWaiting();
    }

    public void setCar(Car c) {
        this.Car = c;
        this.active = true;
        this.timeOfRefueling = 0;
    }

    public void refueling(Car c) {
        this.Car = c;
        this.timeOfRefueling = 0;
    }

    public void refuel() {
        this.timeOfRefueling += 1;
    }

    public boolean checkRefuled() {
        if (this.Car.refueling(this.speed) < this.timeOfRefueling) {
            this.active = false;
            return true;
        }
        return false;

    }

}

class Menu {
    protected double duration = 24;
    protected double frequency = 120;
    protected double stock = 50000;
    protected int precision = 100;
    protected int[] chances = { 0, 1, 2, 3 };
    protected double[] dayCoeficients = { 1 };
    private Scanner sc = new Scanner(System.in);
    String menu = "1. Задать вероятности\n2. Начать симуляцию\n3. Задать время работы\n4. Задать максимальный объём топлива на заправке\n5. Задать частоту обновления запасов топлива\n6. Задать временные коэфициенты\n0. Выйти";

    public int process() {

        while (true) {
            System.out.println(menu);

            String input = sc.next();

            switch (input) {
                case "1":
                    setChances();
                    break;
                case "2":
                    simulate();
                    break;
                case "3":
                    setDuration();
                    break;
                case "4":
                    setStock();
                    break;
                case "5":
                    setFrequency();
                    break;
                case "6":
                    setDayCoeficients();
                    break;
                case "0":
                    return 0;
                default:
                    break;
            }
        }
    }

    protected void setDuration() {
        System.out.println("Сколько часов в день работает заправка?");
        this.duration = sc.nextDouble();
    }

    protected void setStock() {
        System.out.println("Какое максимальное количество топлива в литрах может храниться на заправке");
        this.stock = sc.nextDouble();
    }

    protected void setFrequency() {
        System.out.println("Как часто на заправку привозят топливо (в минутах)");
        this.frequency = sc.nextInt();
    }

    protected void setDayCoeficients() {
        System.out.println("Сколько временных периодов вы хотите задать?");
        int quantity = sc.nextInt();
        double[] res = new double[quantity];
        for (int i = 1; i < quantity + 1; i++) {
            System.out.println("Какой коэфициент вы хотите задать для " + i + " временного периода?");
            double input = sc.nextDouble();
            res[i - 1] = input;

        }

        this.dayCoeficients = res;
    }

    protected void setChances() {
        System.out.println("Сколько максимум машин может приехать на заправку за минуту?");
        int quantity = sc.nextInt();
        String end = "";
        if (quantity == 1)
            end = "а";
        else if (quantity % 5 == 0)
            end = "";
        else
            end = "ы";
        int[] res = new int[precision];
        int last = 0;
        for (int i = 0; i < quantity + 1; i++) {
            System.out.println("В скольких из " + precision + " случаев может приехать " + i + " машин" + end);
            int input = sc.nextInt();
            for (int j = last; j < last + input; j++) {
                res[j] = i;
            }
            last += input;
        }
        if (last != precision - 1) {
            for (int j = last; j < precision - 1; j++) {
                res[j] = 0;
            }
        }

        this.chances = res;
    }

    public int simulate() {
        Gasstation Gasstation = new Gasstation(3, 12, this.chances);

        while (true) {
            for (int t = 0; t < 60 * this.duration; t++) {
                if (t % this.frequency == 0)
                    Gasstation.replenish(this.stock);
                Gasstation.wait(1);

                Gasstation.checkRefuled();
                double split = (double) (this.duration * 60 / dayCoeficients.length) + 1;
                Gasstation.addCars(this.dayCoeficients[(int) (60 * duration / split)]);

                // Gasstation.printQueue();
            }

            System.out.println(Gasstation.getYesterdayPercent() + " " + Gasstation.getTodayPercent() + " "
                    + Gasstation.getTodayData() + " "
                    + Gasstation.getQuantity());

            Gasstation.saveData();
            if (Gasstation.getYesterdayPercent() > 0.001) {
                Gasstation.setQuantity(Gasstation.getQuantity() + 1);
            }

            if (Gasstation.getYesterdayPercent() == 0) {
                System.out.println("Достаточно " + Gasstation.getQuantity() + " колонок");
                return 0;
            }
            if (Gasstation.getQuantity() > 100) {
                System.out.println("На заправке не хватает толива");
                return 0;
            }
            Gasstation.clearQueue();
        }
    }
}

public class zadanie {
    public static void main() {
        Menu Menu = new Menu();
        Menu.process();
    }
}