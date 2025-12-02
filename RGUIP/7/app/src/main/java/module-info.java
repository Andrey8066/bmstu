module com.miner {
    requires javafx.controls;
    requires javafx.fxml;

    opens com.miner to javafx.fxml;
    exports com.miner;
}
