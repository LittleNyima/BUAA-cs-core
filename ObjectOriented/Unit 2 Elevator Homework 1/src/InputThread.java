import com.oocourse.elevator1.ElevatorInput;
import com.oocourse.elevator1.PersonRequest;

import java.io.IOException;

public class InputThread extends Thread {

    private static final ElevatorInput input = new ElevatorInput(System.in);

    InputThread() {
        super();
    }

    @Override
    public void run() {
        while (true) {
            PersonRequest request = input.nextPersonRequest();
            Manager.put(new Person(request));
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
