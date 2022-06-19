import com.oocourse.TimableOutput;
import pers.littlenyima.utils.ProcessorMonitor;

public class MainClass {

    public static void main(String[] args) {
        TimableOutput.initStartTimestamp();
        ProcessorMonitor.setUp();
        Manager.init();
        WaitQueue waitQueueA = new WaitQueue();
        Manager.addElevator("A", waitQueueA);
        ElevatorA elevatorA = new ElevatorA(waitQueueA, "A");
        Manager.register(elevatorA);
        elevatorA.start();
        WaitQueue waitQueueB = new WaitQueue();
        Manager.addElevator("B", waitQueueB);
        ElevatorB elevatorB = new ElevatorB(waitQueueB, "B");
        Manager.register(elevatorB);
        elevatorB.start();
        WaitQueue waitQueueC = new WaitQueue();
        Manager.addElevator("C", waitQueueC);
        ElevatorC elevatorC = new ElevatorC(waitQueueC, "C");
        Manager.register(elevatorC);
        elevatorC.start();
        InputThread inputThread = new InputThread();
        inputThread.start();
    }
}
