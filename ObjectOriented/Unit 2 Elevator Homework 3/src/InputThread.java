import com.oocourse.elevator3.ElevatorInput;
import com.oocourse.elevator3.ElevatorRequest;
import com.oocourse.elevator3.PersonRequest;
import com.oocourse.elevator3.Request;

import java.io.IOException;

public class InputThread extends Thread {

    private final ElevatorInput input;

    InputThread() {
        super();
        this.input = new ElevatorInput(System.in);
    }

    @Override
    public void run() {
        while (true) {
            Request request = input.nextRequest();
            if (request == null) {
                Manager.add(new Person(null));
                break;
            } else if (request instanceof PersonRequest) {
                Manager.add(new Person((PersonRequest) request));
            } else if (request instanceof ElevatorRequest) {
                String type = ((ElevatorRequest) request).getElevatorType();
                String id = ((ElevatorRequest) request).getElevatorId();
                WaitQueue waitQueue = new WaitQueue();
                Manager.addElevator(type, waitQueue);
                if (type.compareTo("A") == 0) {
                    ElevatorA elevatorA = new ElevatorA(waitQueue, id);
                    Manager.register(elevatorA);
                    elevatorA.start();
                } else if (type.compareTo("B") == 0) {
                    ElevatorB elevatorB = new ElevatorB(waitQueue, id);
                    Manager.register(elevatorB);
                    elevatorB.start();
                } else if (type.compareTo("C") == 0) {
                    ElevatorC elevatorC = new ElevatorC(waitQueue, id);
                    Manager.register(elevatorC);
                    elevatorC.start();
                }
            }
        }
        try {
            input.close();
        } catch (IOException e) {
            e.printStackTrace();
        }
    }
}
