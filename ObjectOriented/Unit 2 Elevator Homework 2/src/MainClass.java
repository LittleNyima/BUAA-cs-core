import com.oocourse.TimableOutput;
import com.oocourse.elevator2.ElevatorInput;

public class MainClass {

    public static void main(String[] args) {
        TimableOutput.initStartTimestamp();
        ElevatorInput elevatorInput = new ElevatorInput(System.in);
        int elevatorNumber = elevatorInput.getElevatorNum();
        InputThread inputThread = new InputThread(elevatorInput);
        Manager.init(elevatorNumber);
        for (int i = 0; i < elevatorNumber; i++) {
            Assistant assistant = new Assistant();
            WaitQueue queue = new WaitQueue();
            ElevatorThread elevator = new ElevatorThread(assistant, queue, i, elevatorNumber);
            Manager.setUpPair(queue, assistant);
            elevator.start();
        }
        inputThread.start();
    }
}
