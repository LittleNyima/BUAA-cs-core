import java.util.ArrayList;

public class Manager {

    private static ArrayList<ElevatorType> elevators;
    private static ArrayList<WaitQueue> typeA;
    private static ArrayList<WaitQueue> typeB;
    private static ArrayList<WaitQueue> typeC;
    private static int aTurn;
    private static int bTurn;
    private static int cTurn;

    private Manager() {
    }

    public static void init() {
        elevators = new ArrayList<>();
        typeA = new ArrayList<>();
        typeB = new ArrayList<>();
        typeC = new ArrayList<>();
        aTurn = 0;
        bTurn = 0;
        cTurn = 0;
    }

    public static void addElevator(String type, WaitQueue queue) {
        if (type.compareTo("A") == 0) {
            typeA.add(queue);
        } else if (type.compareTo("B") == 0) {
            typeB.add(queue);
        } else if (type.compareTo("C") == 0) {
            typeC.add(queue);
        }
    }

    public static void add(Person person) {
        if (person.isEmpty()) {
            for (WaitQueue queue : typeA) {
                queue.enqueue(person);
            }
            for (WaitQueue queue : typeB) {
                queue.enqueue(person);
            }
            for (WaitQueue queue : typeC) {
                queue.enqueue(person);
            }
            return;
        }
        enqueue(person);
    }

    public static void register(ElevatorType elevator) {
        elevators.add(elevator);
    }

    private static void putA(Person person) {
        typeA.get(aTurn).enqueue(person);
        aTurn = (aTurn + 1) % typeA.size();
    }

    private static void putB(Person person) {
        typeB.get(bTurn).enqueue(person);
        bTurn = (bTurn + 1) % typeB.size();
    }

    private static void putC(Person person) {
        typeC.get(cTurn).enqueue(person);
        cTurn = (cTurn + 1) % typeC.size();
    }

    public static void transfer(Person person) {
        int from = person.getTransfer();
        int to = person.getTermination();
        if ((from <= 4 || from >= 18) && (to <= 4 || to >= 18)) {
            putA(new Person(person.getId(), from, to, to));
        } else if (from >= 4 && from <= 18 && to >= 4 && to <= 18 &&
                from % 2 == 0 && to % 2 == 0) {
            putC(new Person(person.getId(), from, to, to));
        } else if (from >= 2 && from <= 18 && to >= 2 && to <= 18 && from != 6 && to != 6) {
            putB(new Person(person.getId(), from, to, to));
        } else {
            System.out.println(">>>>>TRANSFER ERROR>>>>>");
        }
    }

    private static void enqueue(Person person) {
        int from = person.getDeparture();
        int to = person.getTermination();
        if ((from <= 4 || from >= 18) && (to <= 4 || to >= 18)) {
            putA(new Person(person.getId(), from, to, to));
        } else if (from >= 4 && from <= 18 && to >= 4 && to <= 18 &&
                from % 2 == 0 && to % 2 == 0) {
            putC(new Person(person.getId(), from, to, to));
        } else if (from >= 2 && from <= 18 && to >= 2 && to <= 18 && from != 6 && to != 6) {
            putB(new Person(person.getId(), from, to, to));
        } else if ((from >= 6 && from <= 17 && to >= 19) || (from >= 19 && to >= 6 && to <= 17)) {
            Person in = new Person(person.getId(), from, 18, to);
            if (from == 6) {
                putC(in);
            } else if (from <= 17) {
                putB(in);
            } else {
                putA(in);
            }
        } else if ((from == 6 && to >= 7 && to <= 17) || (to == 6 && from >= 7 && from <= 17)) {
            Person in = new Person(person.getId(), from, 8, to);
            if (from == 6) {
                putC(in);
            } else {
                putB(in);
            }
        } else {
            Person in = new Person(person.getId(), from, 4, to);
            if ((from >= 2 && from <= 5) || (from >= 7 && from <= 17 && from % 2 == 1)) {
                putB(in);
            } else if (from >= 6 && from <= 16 && from % 2 == 0) {
                putC(in);
            } else {
                putA(in);
            }
        }
    }

    public static boolean terminate() {
        for (ElevatorType elevator : elevators) {
            if (!elevator.isTerminate()) {
                return false;
            }
        }
        for (WaitQueue queue : typeA) {
            if (!queue.isEmpty()) {
                return false;
            }
        }
        for (WaitQueue queue : typeB) {
            if (!queue.isEmpty()) {
                return false;
            }
        }
        for (WaitQueue queue : typeC) {
            if (!queue.isEmpty()) {
                return false;
            }
        }
        return true;
    }
}
