package com.miner;

import javafx.application.Application;
import javafx.event.ActionEvent;
import javafx.event.EventHandler;
import javafx.fxml.FXMLLoader;
import javafx.geometry.Pos;
import javafx.scene.Parent;
import javafx.scene.Scene;
import javafx.scene.control.Button;
import javafx.scene.control.Label;
import javafx.scene.image.Image;
import javafx.scene.image.ImageView;
import javafx.stage.Stage;
import javafx.scene.layout.VBox;
import javafx.scene.layout.HBox;
import javafx.scene.text.Font;

import java.io.FileInputStream;
import java.io.FileNotFoundException;
import java.io.IOException;
import java.util.ArrayList;

/**
 * JavaFX App
 */
public class App extends Application {

    private static Scene scene;

    @Override
    public void start(Stage stage) throws IOException {
        Map map = new Map(7);

        scene = new Scene(map.getVBox());
        stage.setScene(scene);
        stage.show();
    }

    static void setRoot(String fxml) throws IOException {
        scene.setRoot(loadFXML(fxml));
    }

    private static Parent loadFXML(String fxml) throws IOException {
        FXMLLoader fxmlLoader = new FXMLLoader(App.class.getResource(fxml + ".fxml"));
        return fxmlLoader.load();
    }

    public static void main(String[] args) {
        launch();
    }

}

class Map {
    protected VBox vBox = new VBox();
    public ArrayList<ArrayList<Boolean>> bombMap = new ArrayList<>();
    public ArrayList<ArrayList<Boolean>> isOpened = new ArrayList<>();
    protected boolean islose = false;
    protected int opened = 0;
    public double count = 0;
    public int maxSteps = 3;

    public Map(int n) throws FileNotFoundException {
        for (int i = 0; i < n; i++) {
            HBox hBox = new HBox();
            ArrayList<Boolean> line= new ArrayList<>();
            for (int j = 0; j < n; j++) {
                int x = j;
                int y = i;
                line.add(false);

                FileInputStream input = new FileInputStream("app/src/main/resources/com/miner/bomb.png");
                Image image = new Image(input);
                ImageView imageView = new ImageView(image);
                imageView.setFitWidth(80);
                imageView.setFitHeight(80);
                Button btn = new Button();
                btn.setPrefSize(100, 100);
                btn.setMaxHeight(100);
                btn.setMaxHeight(100);
                btn.setOnAction(new EventHandler<ActionEvent>() {
                    @Override
                    public void handle(ActionEvent actionEvent) {
                        count+=1;

                        if (bombMap.isEmpty()) placeBomb(n);
                        //if (count % maxSteps == 0) rebomb();
                        btn.setDisable(true);
                        btn.setText(countBombs(x, y));
                        btn.setStyle("-fx-background-color: #7a7873; ");
                        if (bombMap.get(x).get(y)) {
                            
                            btn.setStyle("-fx-background-color: #a33b35; ");
                            btn.setText(null);
                            System.out.println("Ты проиграл");
                            btn.setGraphic(imageView);
                            endGame("Вы проиграли((((");
                        }
                        System.out.println(count/(n*n));
                        System.out.println(1-countBombsPercent());
                        System.out.println(count/(n*n) == (1-countBombsPercent()));
                        if (count/(n*n) == (1-countBombsPercent())){
                            endGame("Поздравляю!!!\nВы победили!!!!");
                        }
                    }
                });
                hBox.getChildren().add(btn);

            }
            this.vBox.getChildren().add(hBox);
            isOpened.add(line);
        }
    }

    public VBox getVBox() {
        return this.vBox;
    }

    protected void placeBomb(int n) {

        for (int i = 0; i < n; i++) {
            bombMap.add(new ArrayList<Boolean>());
            for (int j = 0; j < n; j++) {
                bombMap.get(i).add((((int) (Math.random() * 10)) % 8 == 3));
            }
        }
        if ((countBombsPercent() > 0.25 ||  countBombsPercent() < 0.15)){
            bombMap.clear();
            placeBomb(n);
        };
        System.out.println(countBombsPercent());
    }

    /*protected void rebomb() {

        for (int i = 0; i < bombMap.size(); i++) {
            for (int j = 0; j < bombMap.size(); j++) {
                if (this.isOpened.get(i).get(j))  this.isOpened.get(i).set(j, (((int) (Math.random() * 10)) % 8 == 3));
            }
        }
        if ((countBombsPercent() > 0.25 ||  countBombsPercent() < 0.15)){
            rebomb();
        };
        for (int i = 0; i < bombMap.size(); i++) {
            for (int j = 0; j < bombMap.size(); j++) {
                HBox hBox =  this.vBox.getChildren().get(i)
            }
        }
        System.out.println(countBombsPercent());
    }*/

    protected String countBombs(int i, int j) {
        int res = 0;
        for (int dx = -1; dx <= 1; dx++) {
            for (int dy = -1; dy <= 1; dy++) {
                try {
                    if (this.bombMap.get(i + dy).get(j + dx)) {
                        res += 1;
                        if (dx == 0 && dy == 0)
                            res -= 1;
                    }
                } catch (IndexOutOfBoundsException e) {
                    res += 0;
                }

            }
        }

        return Integer.toString(res);
    }

    protected double countBombsPercent(){
        double res = 0;
        for (int i = 0; i < this.bombMap.size(); i++){
            for (int j = 0; j < this.bombMap.size(); j++){
                if (this.bombMap.get(i).get(j)) res+=1;
            }
        }
        return res/(this.bombMap.size()*this.bombMap.size());
    }

    public void endGame(String text){
        Label loseLabel = new Label(text);
        loseLabel.setFont(new Font("Arial", 48));
        this.vBox.setAlignment(Pos.CENTER);
        this.vBox.getChildren().clear();
        this.vBox.getChildren().add(loseLabel);
    }
}