import com.oocourse.TimableOutput;

public class MainClass {

    public static void main(String[] args) {
        TimableOutput.initStartTimestamp();
        Manager.init();
        InputThread inputThread = new InputThread();
        ElevatorThread elevatorThread = new ElevatorThread();
        inputThread.start();
        elevatorThread.start();
    }
}
