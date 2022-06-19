import com.oocourse.TimableOutput;

import java.util.ArrayList;

public class ElevatorThread extends Thread {

    private static final int[] reflect =
            new int[]{0, -3, -2, -1, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16};
    private final int waitFloor;
    private ArrayList<Person> passengers;
    private Assistant assistant;
    private WaitQueue queue;
    private String mark;
    private int floor;
    private int direction;
    private int destination;
    private int status; // 0 = idle, 1 = occupied
    private int capacity;

    ElevatorThread(Assistant assistant, WaitQueue queue, int idNum, int totalNum) {
        this.passengers = new ArrayList<>();
        this.assistant = assistant;
        this.queue = queue;
        this.floor = 4;
        this.direction = 1;
        this.destination = 1;
        this.status = 0;
        this.capacity = 7;
        //this.waitFloor = 20 / (totalNum + 1) * (idNum + 1);
        this.waitFloor = 9;
        this.mark = (char) ('A' + idNum) + "";
    }

    @Override
    public void run() {
        while (true) {
            while (true) {
                if (this.isEmpty() && this.queue.isEmpty() && this.floor == this.waitFloor) {
                    this.status = 0;
                    this.assistant.setStatus(this.status);
                    try {
                        synchronized (this) {
                            this.wait(5L);
                            this.notifyAll();
                        }
                    } catch (InterruptedException e) {
                        e.printStackTrace();
                    }
                } else if (this.isEmpty() && this.queue.isEmpty() && floor != waitFloor) {
                    this.status = 0;
                    this.assistant.setStatus(this.status);
                    this.direction = (this.waitFloor > this.floor) ? 1 : -1;
                    this.assistant.setDirection(this.direction);
                    this.move();
                } else if (passengers.size() == 1 && passengers.get(0).isEmpty()) {
                    return;
                } else {
                    this.status = 1;
                    this.assistant.setStatus(this.status);
                    break;
                }
            }
            exchange();
            if (passengers.size() == 1 && passengers.get(0).isEmpty()) {
                return;
            }
            destination = calDestination();
            assistant.setDestination(this.destination);
            if ((destination == floor) || (destination < floor && direction > 0) ||
                    !movable() || (destination > floor && direction < 0)) {
                direction = -direction;
                this.assistant.setDirection(this.direction);
                continue;
            }
            move();
        }
    }

    private boolean isEmpty() {
        return passengers.size() == 0;
    }

    private boolean movable() {
        if (floor == 1 && direction == -1) {
            return false;
        } else {
            return floor != 19 || direction != 1;
        }
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
        ArrayList<Person> income = queue.dequeue(floor, direction, passengers.size(), capacity);
        if (outcome.size() == 0 && income.size() == 1 && income.get(0).isEmpty()) {
            passengers.add(income.get(0));
            capacity = 8;
            return;
        }
        if (outcome.size() != 0 || income.size() != 0) {
            TimableOutput.println("OPEN-" + reflect[floor] + "-" + mark);
            try {
                sleep(400L);
            } catch (InterruptedException e) {
                e.printStackTrace();
            }
            for (Person person : outcome) {
                TimableOutput.println(
                        "OUT-" + person.getId() + "-" + reflect[floor] + "-" + mark);
            }
            for (Person person : income) {
                passengers.add(person);
                if (!person.isEmpty()) {
                    TimableOutput.println(
                            "IN-" + person.getId() + "-" + reflect[floor] + "-" + mark);
                } else {
                    capacity = 8;
                }
            }
            for (Person person : queue.dequeue(floor, direction, passengers.size(), capacity)) {
                passengers.add(person);
                if (!person.isEmpty()) {
                    TimableOutput.println(
                            "IN-" + person.getId() + "-" + reflect[floor] + "-" + mark);
                } else {
                    capacity = 8;
                }
            }
            TimableOutput.println("CLOSE-" + reflect[floor] + "-" + mark);
        }
        assistant.setLoad(this.passengers.size());
        assistant.setCapacity(this.capacity);
    }

    private void move() {
        floor = floor + direction;
        this.assistant.setFloor(this.floor);
        try {
            sleep(400L);
        } catch (InterruptedException e) {
            e.printStackTrace();
        }
        TimableOutput.println("ARRIVE-" + reflect[floor] + "-" + this.mark);
    }

    private int calDestination() {
        int ret;
        if (direction == 1) {
            ret = -1;
            for (Person person : passengers) {
                if (person.isEmpty()) {
                    continue;
                }
                ret = Math.max(ret, person.getTo());
            }
            ret = Math.max(queue.destination(floor, direction), ret);
        } else {
            ret = 20;
            for (Person person : passengers) {
                if (person.isEmpty()) {
                    continue;
                }
                ret = Math.min(ret, person.getTo());
            }
            ret = Math.min(queue.destination(floor, direction), ret);
        }
        return (ret == -1 || ret == 20) ? floor : ret;
    }
}
