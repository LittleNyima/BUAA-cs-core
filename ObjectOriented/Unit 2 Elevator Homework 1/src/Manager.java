import java.util.ArrayList;

public class Manager {

    private static final ArrayList<Person> people = new ArrayList<>();

    private Manager() {
    }

    public static void init() {
    }

    public static void put(Person person) {
        synchronized (people) {
            people.add(person);
            people.notifyAll();
        }
    }

    public static ArrayList<Person> get(int floor) {
        ArrayList<Person> ret = new ArrayList<>();
        int i = 0;
        synchronized (people) {
            while (i < people.size()) {
                if (people.get(i).getFrom() == floor) {
                    ret.add(people.get(i));
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

    public static int rollBack(int floor, int direction, int destiny) {
        int ret;
        if (direction == 1) {
            ret = 16;
            synchronized (people) {
                for (Person person : people) {
                    if (person.getDirection() == 1 && person.getFrom() < floor) {
                        ret = Math.min(person.getFrom(), ret);
                    } else if (person.getDirection() == -1 && person.getFrom() < floor &&
                            destiny + person.getTo() > floor + floor
                    ) {
                        ret = Math.min(person.getFrom(), ret);
                    }
                }
                people.notifyAll();
            }
        } else {
            ret = -1;
            synchronized (people) {
                for (Person person : people) {
                    if (person.getDirection() == -1 && person.getFrom() > floor) {
                        ret = Math.max(person.getFrom(), ret);
                    } else if (person.getDirection() == 1 && person.getFrom() < floor &&
                            destiny + person.getTo() < floor + floor
                    ) {
                        ret = Math.min(person.getFrom(), ret);
                    }
                }
                people.notifyAll();
            }
        }
        return ret;
    }

    public static int destiny(int floor, int direction) {
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
            ret = 16;
            synchronized (people) {
                for (Person person : people) {
                    if (person.getFrom() <= floor && !person.isEmpty()) {
                        ret = Math.min(ret, person.getFrom());
                    }
                }
                people.notifyAll();
            }
        }
        return (ret == -1 || ret == 16) ? floor : ret;
    }

    public static boolean isEmpty() {
        boolean ret;
        synchronized (people) {
            ret = people.size() == 0;
            people.notifyAll();
        }
        return ret;
    }
}
