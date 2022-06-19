import java.util.ArrayList;

public class ElevatorA extends Thread implements ElevatorType {

    private static final int[] map = new int[]{0, -3, -2, -1, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10
            , 11, 12, 13, 14, 15, 16, 17, 18, 19, 20};
    private static final int waitFloor = 12;
    private static final long time = 400L;
    private ArrayList<Person> passengers;
    private WaitQueue queue;
    private String id;
    private int floor;
    private int direction;
    private int destination;
    private int capacity;
    private volatile boolean terminate;

    ElevatorA(WaitQueue queue, String id) {
        this.passengers = new ArrayList<>();
        this.queue = queue;
        this.id = id;
        this.floor = 4;
        this.direction = 1;
        this.destination = 4;
        this.capacity = 6;
        this.terminate = false;
    }

    @Override
    public void run() {
        while (true) {
            while (true) {
                if (!queue.isEmpty()) {
                    this.terminate = false;
                    break;
                } else if (this.isEmpty() && queue.isEmpty() && this.floor == waitFloor) {
                    this.terminate = false;
                    synchronized (this) {
                        try {
                            this.wait(5L);
                        } catch (InterruptedException e) {
                            e.printStackTrace();
                        }
                        this.notifyAll();
                    }
                } else if (this.isEmpty() && this.queue.isEmpty() && this.floor != waitFloor) {
                    this.terminate = false;
                    this.direction = (waitFloor > this.floor) ? 1 : -1;
                    this.move();
                } else if (passengers.size() == 1 && passengers.get(0).isEmpty()) {
                    this.terminate = true;
                    synchronized (this) {
                        try {
                            this.wait(5L);
                        } catch (InterruptedException e) {
                            e.printStackTrace();
                        }
                        this.notifyAll();
                    }
                    if (Manager.terminate()) {
                        return;
                    }
                } else {
                    this.terminate = false;
                    break;
                }
            }
            this.terminate = false;
            if (this.changeable() || this.queue.ultimate()) {
                exchange();
            }
            /*if (passengers.size() == 1 && passengers.get(0).isEmpty()) {
                this.terminate = true;
                if (Manager.terminate()) {
                    return;
                }
                this.terminate = false;
            }*/
            destination = calDestination();
            if ((destination == floor) || (destination < floor && direction > 0) ||
                    !movable() || (destination > floor && direction < 0)) {
                direction = -direction;
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
            return floor != 23 || direction != 1;
        }
    }

    private boolean changeable() {
        return this.floor >= 18 || this.floor <= 4;
    }

    private void exchange() {
        ArrayList<Person> outcome = new ArrayList<>();
        int i = 0;
        while (i < passengers.size()) {
            Person person = passengers.get(i);
            if (person.getTransfer() == floor) {
                outcome.add(person);
                passengers.remove(i);
            } else {
                i++;
            }
        }
        ArrayList<Person> income = queue.dequeue(floor, direction, passengers.size(), capacity);
        if (outcome.size() == 0 && income.size() == 1 && income.get(0).isEmpty()) {
            passengers.add(income.get(0));
            capacity = 7;
            return;
        }
        if (outcome.size() != 0 || income.size() != 0) {
            ConcurrentOutput.println("OPEN-" + map[floor] + "-" + id);
            for (Person person : outcome) {
                ConcurrentOutput.println("OUT-" + person.getId() + "-" + map[floor] + "-" + id);
                if (person.needTransfer()) {
                    Manager.transfer(person);
                }
            }
            try {
                sleep(400L);
            } catch (InterruptedException e) {
                e.printStackTrace();
            }
            for (Person person : income) {
                passengers.add(person);
                if (!person.isEmpty()) {
                    ConcurrentOutput.println("IN-" + person.getId() + "-" + map[floor] + "-" + id);
                } else {
                    capacity = 7;
                }
            }
            for (Person person : queue.dequeue(floor, direction, passengers.size(), capacity)) {
                passengers.add(person);
                if (!person.isEmpty()) {
                    ConcurrentOutput.println("IN-" + person.getId() + "-" + map[floor] + "-" + id);
                } else {
                    capacity = 7;
                }
            }
            ConcurrentOutput.println("CLOSE-" + map[floor] + "-" + id);
        }
    }

    private void move() {
        floor = floor + direction;
        try {
            sleep(time);
        } catch (InterruptedException e) {
            e.printStackTrace();
        }
        ConcurrentOutput.println("ARRIVE-" + map[floor] + "-" + id);
    }

    private int calDestination() {
        int ret;
        if (direction == 1) {
            ret = -1;
            for (Person person : passengers) {
                if (person.isEmpty()) {
                    continue;
                }
                ret = Math.max(ret, person.getTransfer());
            }
            ret = Math.max(queue.destination(floor, direction), ret);
        } else {
            ret = 25;
            for (Person person : passengers) {
                if (person.isEmpty()) {
                    continue;
                }
                ret = Math.min(ret, person.getTransfer());
            }
            ret = Math.min(queue.destination(floor, direction), ret);
        }
        return (ret == -1 || ret == 25) ? floor : ret;
    }

    @Override
    public boolean isTerminate() {
        return terminate;
    }
}
