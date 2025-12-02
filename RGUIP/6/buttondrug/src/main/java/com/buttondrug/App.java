package com.buttondrug;

import javafx.animation.AnimationTimer;
import javafx.application.Application;
import javafx.scene.Scene;
import javafx.scene.control.Button;
import javafx.scene.control.Label;
import javafx.stage.Stage;
import javafx.scene.Group;

import java.io.IOException;
import java.util.ArrayList;


/**
 * JavaFX App
 */

public class App extends Application {
    protected int maxN = 3;
    protected int size = 500;
    protected int clicks = 0;
    protected ArrayList<ButtonData> buttons = new ArrayList<>();
    protected boolean f = false;

    Group group1 = new Group();
    Scene scene1 = new Scene(group1);
    

    @Override
    public void start(Stage stage) throws IOException {

        
        Label lbl = new Label();
        Button btn = new Button("Press me");

        AnimationTimer timer = new AnimationTimer() {
            @Override
            public void handle(long now) {
                replace();
            }
        };

        btn.setLayoutX(120);
        btn.setLayoutY(200);
        group1.getChildren().add(btn);

        btn.setOnAction(e -> {
            group1.getChildren().remove(btn);
            buttons.add(new ButtonData(100, 100, " "));
            group1.getChildren().add(buttons.get(0).getButton());
            timer.start();
        });

        

        


        stage.setWidth(size);
        stage.setHeight(size);
        stage.setScene(scene1);
        stage.show();
    }

    public void replace() {
        for (int i = 0; i < buttons.size(); i++) {
            buttons.get(i).replace();
            if (buttons.get(i).getN() >= maxN && (buttons.get(i).getDx()> 10 || buttons.get(i).getDy()>10)) {
                if (buttons.get(i).getNAll() >= maxN*2) {
                    group1.getChildren().remove(buttons.get(i).getButton());
                    buttons.remove(i);
                }
                else {
                    buttons.get(i).seth(10);
                    buttons.get(i).setw(10);
                    int x = (int) buttons.get(i).getButton().getLayoutX();
                    int y = (int) buttons.get(i).getButton().getLayoutY();
                    for (int j = 0; j < 2 ; j++) {
                    buttons.add(new ButtonData(x,y, " "));
                    group1.getChildren().add(buttons.get(buttons.size() - 1).getButton());
                }
                buttons.get(i).setN(0);
            }
            }
            if(buttons.get(i).checkOutOfBounds() && buttons.get(i).getDx() +buttons.get(i).getDy() < 5){
                group1.getChildren().remove(buttons.get(i).getButton());
                    buttons.remove(i);
            }
        }
    }

    public static void main(String[] args) {
        launch();
    }

}

class ButtonData {
    protected int size = 500;
    protected int speed = 3;
    protected int a = (int) (Math.random() * 5);
    protected int b = 1;
    protected int x0;
    protected int y0;
    protected int nAll = 0;
    protected Double sizeCoeficient = 1.5;
    protected int n = 0;
    protected int w = 100;
    protected int h = 100;
    protected Button button;

    public ButtonData(int x, int y, String text) {
        this.button = new Button(text);
        
        seth(10);
        setw(10);
        if (x>size-this.w) this.button.setLayoutX(x-w);
        this.button.setLayoutX(x);
        if (y>size-this.h) this.button.setLayoutY(y-h);
        this.button.setLayoutY(y);
        this.x0 = x;
        this.y0 = y;
    }

    public void setN(int ni) {
        this.n = ni;
    }

    public int getN() {
        return n;
    }

    public boolean checkOutOfBounds(){
        if (this.button.getLayoutX()>this.size-w/3 || this.button.getLayoutY()>this.size-h/3 )return true;
        return false;
    }
    public void seth(int h){
        this.h = h;
        button.setPrefHeight(h);
    }
    public void setw(int w){
        this.w = w;
        button.setPrefHeight(w);
    }

    public Button getButton() {

        return this.button;
    }

    public int getDx(){
        return Math.abs((int)this.button.getLayoutX()-this.x0);
    }
    
    public int getDy(){
        return Math.abs((int)this.button.getLayoutY()-this.y0);
    }
    public int getNAll(){
        return nAll;
    }
    public void replace() {
        this.button.setLayoutX(this.button.getLayoutX() + speed * b);
        this.button.setLayoutY(this.button.getLayoutY() + (this.a) * speed);
        if ((this.button.getLayoutX() <= 0 || this.button.getLayoutX() >= this.size-w) && getDx()>this.w/2) {
            b = -b;
            n += 1;
            nAll++;

            this.x0 = (int) this.button.getLayoutX();
            this.y0 = (int) this.button.getLayoutY();
            
            this.h = (int)(h*this.sizeCoeficient);
            this.w = (int)(w*this.sizeCoeficient);
            this.button.setPrefHeight(h);
            this.button.setPrefWidth(w);
        }
        if ((this.button.getLayoutY() <= 0 || this.button.getLayoutY() >= this.size-h ) && getDy()>this.h/2) {
            a = -a;
            n += 1;
            nAll++;

            this.x0 = (int) this.button.getLayoutX();
            this.y0 = (int) this.button.getLayoutY();

            this.h = (int)(h*this.sizeCoeficient);
            this.w = (int)(w*this.sizeCoeficient);
            this.button.setPrefHeight(h);
            this.button.setPrefWidth(w);
        }
    }
}