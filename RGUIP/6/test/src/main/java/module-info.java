module com.buttondrug {
    requires javafx.controls;
    requires javafx.fxml;

    opens com.buttondrug to javafx.fxml;
    exports com.buttondrug;
}
