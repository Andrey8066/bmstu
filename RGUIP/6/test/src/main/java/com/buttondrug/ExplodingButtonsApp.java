package com.buttondrug;

import javafx.animation.AnimationTimer;
import javafx.application.Application;
import javafx.scene.Scene;
import javafx.scene.control.Button;
import javafx.scene.layout.Pane;
import javafx.stage.Stage;

import java.util.ArrayList;
import java.util.Iterator;
import java.util.List;

public class ExplodingButtonsApp extends Application {

    private Pane root;
    private final double START_SIZE = 50;
    private final double MAX_SIZE = 120;
    private final double SPEED = 3;

    private class MovingButton {
        Button button;
        double dx = SPEED;
        double dy = SPEED;

        public MovingButton(double x, double y, double size) {
            button = new Button("Click!");
            button.setPrefSize(size, size);
            button.setLayoutX(x-50);
            button.setLayoutY(y-50);

            root.getChildren().add(button);
        }

        void move() {
            double x = button.getLayoutX();
            double y = button.getLayoutY();
            double width = button.getWidth();
            double height = button.getHeight();

            boolean hitWall = false;

            if (x + dx < 0 || x + width + dx > root.getWidth()) {
                dx = -dx;
                grow();
            }
            if (y + dy < 0 || y + height + dy > root.getHeight()) {
                dy = -dy;
                grow();
            }

            button.setLayoutX(x + dx);
            button.setLayoutY(y + dy);

            if (hitWall) {
                grow();
            }
        }

        void grow() {
            double newSize = button.getWidth() + 10;
            if (newSize >= MAX_SIZE) {
                explode();
            } else {
                button.setPrefSize(newSize, newSize);
            }
        }

        void explode() {
            root.getChildren().remove(button);
            buttons.remove(this);

            for (int i = 0; i < 4; i++) {
                double offsetX = (i % 2 == 0 ? -1 : 1) * 10;
                double offsetY = (i < 2 ? -1 : 1) * 10;

                MovingButton newBtn = new MovingButton(
                        button.getLayoutX() + offsetX,
                        button.getLayoutY() + offsetY,
                        START_SIZE
                );
                buttons.add(newBtn);
            }
        }
    }

    private List<MovingButton> buttons = new ArrayList<>();
    private boolean started = false;

    @Override
    public void start(Stage primaryStage) {
        root = new Pane();
        Scene scene = new Scene(root, 600, 400);

        Button starter = new Button("Start!");
        starter.setPrefSize(START_SIZE, START_SIZE);
        starter.setLayoutX(275);
        starter.setLayoutY(175);
        root.getChildren().add(starter);

        starter.setOnAction(e -> {
            if (!started) {
                MovingButton mb = new MovingButton(starter.getLayoutX(), starter.getLayoutY(), START_SIZE);
                buttons.add(mb);
                root.getChildren().remove(starter);
                started = true;
            }
        });

        AnimationTimer timer = new AnimationTimer() {
            @Override
            public void handle(long now) {
                for (Iterator<MovingButton> it = buttons.iterator(); it.hasNext(); ) {
                    MovingButton mb = it.next();
                    mb.move();
                }
            }
        };
        timer.start();

        primaryStage.setScene(scene);
        primaryStage.setTitle("Exploding Buttons");
        primaryStage.show();
    }

    public static void main(String[] args) {
        launch(args);
    }
}
