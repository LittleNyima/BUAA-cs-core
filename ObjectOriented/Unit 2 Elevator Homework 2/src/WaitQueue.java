import java.util.ArrayList;

public class WaitQueue {

    private final ArrayList<Person> people;

    WaitQueue() {
        this.people = new ArrayList<>();
    }

    public void enqueue(Person person) {
        synchronized (people) {
            people.add(person);
            people.notifyAll();
        }
    }

    public ArrayList<Person> dequeue(int floor, int direction, int loads, int capacity) {
        int maxNum = capacity - loads;
        ArrayList<Person> ret = new ArrayList<>();
        int i = 0;
        synchronized (people) {
            while (i < people.size() && ret.size() < maxNum) {
                Person person = people.get(i);
                if (person.getFrom() == floor && person.getDirection() == direction) {
                    ret.add(person);
                    people.remove(i);
                } else {
                    i++;
                }
            }
            if (people.size() == 1 && people.get(0).isEmpty()) {
                ret.add(people.get(0));
                people.remove(0);
            }
            people.notifyAll();
        }
        return ret;
    }

    public int destination(int floor, int direction) {
        int ret;
        if (direction == 1) {
            ret = -1;
            synchronized (people) {
                for (Person person : people) {
                    if (person.getFrom() >= floor) {
                        ret = Math.max(ret, person.getFrom());
                    }
                }
                people.notifyAll();
            }
        } else {
            ret = 20;
            synchronized (people) {
                for (Person person : people) {
                    if (person.getFrom() <= floor && !person.isEmpty()) {
                        ret = Math.min(ret, person.getFrom());
                    }
                }
                people.notifyAll();
            }
        }
        return (ret == -1 || ret == 20) ? floor : ret;
    }

    public boolean isEmpty() {
        boolean ret;
        synchronized (people) {
            ret = people.size() == 0;
            people.notifyAll();
        }
        return ret;
    }
}
