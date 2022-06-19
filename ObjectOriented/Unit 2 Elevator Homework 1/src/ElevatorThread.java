import com.oocourse.TimableOutput;

import java.util.ArrayList;

public class ElevatorThread extends Thread {

    private ArrayList<Person> passengers;
    private int floor;
    private int direction;
    private int destiny;

    ElevatorThread() {
        super();
        passengers = new ArrayList<>();
        floor = 1;
        direction = 1;
        destiny = 1;
    }

    @Override
    public void run() {
        while (true) {
            while (true) {
                if (this.isEmpty() && Manager.isEmpty()) {
                    try {
                        synchronized (this) {
                            this.wait(5L);
                            this.notifyAll();
                        }
                    } catch (InterruptedException e) {
                        e.printStackTrace();
                    }
                } else if (passengers.size() == 1 && passengers.get(0).isEmpty()) {
                    return;
                } else {
                    break;
                }
            }
            exchange();
            if (passengers.size() == 1 && passengers.get(0).isEmpty()) {
                return;
            }
            destiny = calDestiny();
            if (destiny == floor || !movable()) {
                direction = -direction;
                continue;
            }
            int rollBack = Manager.rollBack(floor, direction, destiny);
            if (rollBack != -1 && rollBack != 16) {
                direction = -direction;
                while (floor != rollBack && movable()) {
                    floor = floor + direction;
                    try {
                        sleep(400L);
                    } catch (InterruptedException e) {
                        e.printStackTrace();
                    }
                    TimableOutput.println("ARRIVE-" + floor);
                    exchange();
                    rollBack = calDestiny();
                }
                direction = -direction;
                continue;
            }
            floor = floor + direction;
            try {
                sleep(400L);
            } catch (InterruptedException e) {
                e.printStackTrace();
            }
            TimableOutput.println("ARRIVE-" + floor);
        }
    }

    private boolean isEmpty() {
        return passengers.size() == 0;
    }

    private boolean movable() {
        if (floor == 1 && direction == -1) {
            return false;
        } else {
            return floor != 15 || direction != 1;
        }
    }

    private int calDestiny() {
        int ret;
        if (direction == 1) {
            ret = -1;
            for (Person person : passengers) {
                if (person.isEmpty()) {
                    continue;
                }
                ret = Math.max(ret, person.getTo());
            }
            ret = Math.max(Manager.destiny(floor, direction), ret);
        } else {
            ret = 16;
            for (Person person : passengers) {
                if (person.isEmpty()) {
                    continue;
                }
                ret = Math.min(ret, person.getTo());
            }
            ret = Math.min(Manager.destiny(floor, direction), ret);
        }
        return (ret == -1 || ret == 16) ? floor : ret;
    }

    private void exchange() {
        ArrayList<Person> outcome = new ArrayList<>();
        int i = 0;
        while (i < passengers.size()) {
            Person person = passengers.get(i);
            if (person.getTo() == floor) {
                outcome.add(person);
                passengers.remove(i);
            } else {
                i++;
            }
        }
        ArrayList<Person> income = Manager.get(floor);
        if (outcome.size() == 0 && income.size() == 1 && income.get(0).isEmpty()) {
            passengers.add(income.get(0));
            return;
        }
        if (outcome.size() != 0 || income.size() != 0) {
            TimableOutput.println("OPEN-" + floor);
            try {
                sleep(400L);
            } catch (InterruptedException e) {
                e.printStackTrace();
            }
            for (Person person : outcome) {
                TimableOutput.println("OUT-" + person.getId() + "-" + floor);
            }
            for (Person person : income) {
                passengers.add(person);
                if (!person.isEmpty()) {
                    TimableOutput.println("IN-" + person.getId() + "-" + floor);
                }
            }
            for (Person person : Manager.get(floor)) {
                passengers.add(person);
                if (!person.isEmpty()) {
                    TimableOutput.println("IN-" + person.getId() + "-" + floor);
                }
            }
            TimableOutput.println("CLOSE-" + floor);
        }
    }
}
