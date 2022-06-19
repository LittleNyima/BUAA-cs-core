import com.oocourse.elevator2.ElevatorInput;
import com.oocourse.elevator2.PersonRequest;

import java.io.IOException;

public class InputThread extends Thread {

    private final ElevatorInput input;

    InputThread(ElevatorInput input) {
        super();
        this.input = input;
    }

    @Override
    public void run() {
        while (true) {
            PersonRequest request = input.nextPersonRequest();
            Manager.distribute(new Person(request));
            if (request == null) {
                break;
            }
        }
        try {
            input.close();
        } catch (IOException e) {
            e.printStackTrace();
        }
    }
}
